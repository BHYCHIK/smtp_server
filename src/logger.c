#include <mqueue.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libconfig.h>
#include <time.h>

static mqd_t logger_fd;
static int inited = 0;

static void do_work(config_t *cfg) {
    cfg = NULL;
    while(1) {
        char log_str[10240];
        int size = mq_receive(logger_fd, log_str, sizeof(log_str), NULL);
        fprintf(stderr, "LOG: %.*s\n", size, log_str);
    }
}

void send_to_log(const char *msg) {
    mq_send(logger_fd, msg, strlen(msg), 0);
}

int prepare_logger(config_t *cfg, int sock) {
    inited = 0;
    struct mq_attr attr;
    attr.mq_flags   = 0;
    attr.mq_maxmsg  = 100;
    attr.mq_msgsize = 10240;
    attr.mq_curmsgs = 0;
    logger_fd = mq_open("/smtplogger", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, &attr);
    if (logger_fd < 0) {
        perror("Cannot make logger");
        exit(EXIT_FAILURE);
    }
    pid_t pid = fork();
    if (pid < 0) {
        fprintf(stderr, "Cannot fork logger\n");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        close(sock);
        do_work(cfg);
    }
    return 0;
}
