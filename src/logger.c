#include <mqueue.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libconfig.h>
#include <time.h>
#include <signal.h>

static int proc_creator = 0;
static pid_t logger_pid = 0;
static mqd_t logger_fd;
static int inited = 0;

static void logger_killer() {
    if (proc_creator == getpid()) {
        kill(logger_pid, SIGTERM);
    }
    mq_close(logger_fd);
}

static int logger_work = 1;

static void sig_handler(int signum) {
    signum = 0;
    logger_work = 0;
}

static void do_work(config_t *cfg) {
    struct mq_attr attr;
    attr.mq_flags   = 0;
    attr.mq_maxmsg  = 100;
    attr.mq_msgsize = 10240;
    attr.mq_curmsgs = 0;
    logger_fd = mq_open("/smtplogger", O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, &attr);

    const char *log_path = NULL;
    if (!config_lookup_string(cfg, "LogFile", &log_path)) log_path = "/var/log/smtp.log";
    struct sigaction act;
    memset (&act, '\0', sizeof(act));
    act.sa_handler = &sig_handler;
    sigemptyset(&act.sa_mask);
    sigaction(SIGTERM, &act, NULL);
    FILE *logfile = fopen(log_path, "a");

    while (logger_work) {
        char log_str[10240];
        int size = mq_receive(logger_fd, log_str, sizeof(log_str), NULL);
        if (size > 0) {
            fprintf(logfile, "%ld %.*s\n", time(NULL), size, log_str);
            fflush(logfile);
        }
    }
    mq_close(logger_fd);
    fclose(logfile);
    sync();
}


void send_to_log(const char *msg) {
    if (inited != getpid()) {
        mq_close(logger_fd);
        logger_fd = mq_open("/smtplogger", O_WRONLY);
        inited = getpid();
    }
    char log_str[10240];
    int size = snprintf(log_str, sizeof(log_str), "%d %s", getpid(), msg);
    mq_send(logger_fd, log_str, size, 0);
}

int prepare_logger(config_t *cfg, int sock) {
    proc_creator = getpid();
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
        exit(EXIT_SUCCESS);
    }

    logger_pid = pid;
    atexit(logger_killer);

    return pid;
}
