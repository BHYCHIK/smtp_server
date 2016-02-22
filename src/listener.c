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
#include "worker.h"

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

int is_run = 1;

void server(config_t *cfg) {
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

    struct pollfd fds[1];
    fds[0].fd = sock;
    fds[0].events = POLLIN;

    while(is_run) {
        rc = poll(fds, 1, -1);
        if (rc < 0) {
            perror("poll failed");
            close(sock);
            exit(EXIT_FAILURE);
        }

        assert(rc > 0);

        if (fds[0].revents & POLLIN) {
            struct sockaddr_in incoming_addr;
            socklen_t socklen = sizeof(incoming_addr);
            printf("sock = %d\n", sock);
            int child_socket = accept(sock, (struct sockaddr *)&incoming_addr, &socklen);
            if (child_socket < 0) {
                close(sock);
                perror("Acception fail");
            }
            if (!set_block_mode(child_socket, 0)) {
                close(sock);
                perror("Unable to make blocking child socket");
                exit(EXIT_FAILURE);
            }
            if (!run_worker(sock, child_socket, cfg)) {
                close(child_socket);
                close(sock);
                perror("worker run failed");
                exit(EXIT_FAILURE);
            }
        }
    }
    close(sock);
}
