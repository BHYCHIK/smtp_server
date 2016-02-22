#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <libconfig.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/poll.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "worker.h"
#include "logger.h"

static pid_t logger_pid = 0;
int workers_num = 0;

static void chld_handler(int signum) {
    int stat;
    pid_t pd = 1;
    while (pd > 0) {
        pd = waitpid(-1, &stat, WNOHANG);
        if ( (pd > 0 ) && (pd != logger_pid)) --workers_num;
    }

}

/** Returns true on success, or false if there was an error */
static int set_block_mode(int fd, int blocking)
{
    if (fd < 0) {
        perror("Bad sock number");
        return 0;
    }

    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) {
        perror ("flags < 0");
        return 0;
    }
    flags = blocking ? (flags&~O_NONBLOCK) : (flags|O_NONBLOCK);
    return (fcntl(fd, F_SETFL, flags) == 0) ? 1 : 0;
}

extern volatile int is_run;

static void drop_priv(config_t *cfg) {
    int user_id = 0;
    int group_id = 0;

    const char *user;
    const char *group;

    if (!config_lookup_string(cfg, "User", &user)) {
        fprintf(stderr, "No User in config\n");
        exit(EXIT_FAILURE);
    }
    if (!config_lookup_string(cfg, "Group", &group)) {
        fprintf(stderr, "No Group in config\n");
        exit(EXIT_FAILURE);
    }

    struct passwd *user_data = getpwnam(user);
    user_id = user_data->pw_uid;

    struct group *group_data = getgrnam(group);
    group_id = group_data->gr_gid;

    if (setgid(group_id) == -1) {
        fprintf(stderr, "Cannot switch group\n");
        exit(EXIT_FAILURE);
    }

    if (setuid(user_id) == -1) {
        fprintf(stderr, "Cannot switch user\n");
        exit(EXIT_FAILURE);
    }
}

void server(config_t *cfg) {
    struct sigaction act;
    memset (&act, '\0', sizeof(act));
    act.sa_handler = &chld_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &act, NULL);

    int port = 2525;
    if (!config_lookup_int(cfg, "port", &port)) port = 2525;

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    int on = 1;

    int rc = setsockopt(sock, SOL_SOCKET,  SO_REUSEADDR,
            (char *)&on, sizeof(on));
    if (rc < 0) {
        close(sock);
        perror("setsockopt() failed");
        exit(EXIT_FAILURE);
    }

    if (!set_block_mode(sock, 0)) {
        close(sock);
        perror("Unable to make blocking socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port        = htons(port);
    rc = bind(sock,
            (struct sockaddr *)&addr, sizeof(addr));
    if (rc < 0) {
        close(sock);
        perror("bind() failed");
        exit(EXIT_FAILURE);
    }

    int backlog = 32;
    if (!config_lookup_int(cfg, "backlog", &backlog)) backlog = 32;
    rc = listen(sock, backlog);
    if (rc < 0)
    {
        close(sock);
        perror("listen() failed");
        exit(EXIT_FAILURE);
    }

    drop_priv(cfg);
    logger_pid = prepare_logger(cfg, sock);

    struct pollfd fds[1];
    fds[0].fd = sock;
    fds[0].events = POLLIN;

    while(is_run) {
        rc = poll(fds, 1, -1);
        if ( (rc < 0) && (errno != EINTR)) {
            char log_line[10240];
            snprintf(log_line, sizeof(log_line), "poll failed: %s", strerror(errno));
            send_to_log(log_line);
            close(sock);
            exit(EXIT_FAILURE);
        }

        if ( (rc < 0) && (errno == EINTR)) continue;

        assert(rc > 0);

        if (fds[0].revents & POLLIN) {
            struct sockaddr_in incoming_addr;
            socklen_t socklen = sizeof(incoming_addr);
            int child_socket = accept(sock, (struct sockaddr *)&incoming_addr, &socklen);
            if (child_socket < 0) {
                close(sock);
                send_to_log("Acception fail");
            }
            if (!set_block_mode(child_socket, 0)) {
                close(sock);
                send_to_log("Unable to make blocking child socket");
                exit(EXIT_FAILURE);
            }



            if (!run_worker(sock, child_socket, cfg, &incoming_addr)) {
                close(child_socket);
                close(sock);
                send_to_log("worker run failed");
                exit(EXIT_FAILURE);
            }

            ++workers_num;

        }
    }
    close(sock);
}
