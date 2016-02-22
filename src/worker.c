#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/poll.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include "session.h"
#include "smtp.h"

static void worker_loop(struct user_session *session) {
    int rc;

    struct pollfd fds[1];
    fds[0].fd = session->sock;
    fds[0].events = POLLIN | POLLOUT;

    while(session->in_progress || session->outcome_buffer_size) {
        rc = poll(fds, 1, 30000);
        if (rc < 0) {
            perror("poll failed");
            return;
        }

        if (rc == 0) {
            perror("Timeout");
            session->timedout = 1;
            do_smtp(session);
            continue;
        }

        if (fds[0].revents & POLLIN) {
            while (1) {
                int received = recv(session->sock, session->income_buffer + session->income_buffer_size, sizeof(session->income_buffer) - session->income_buffer_size - 1, 0);
                if (received > 0) {
                    session->income_buffer_size += received;
                    if (session->income_buffer_size == sizeof(session->income_buffer) - 1) {
                        perror("Income buffer overflow");
                        return;
                    }
                    session->income_buffer[session->income_buffer_size] = '\0';
                } else if (received == 0) {
                    session->in_progress = 0;
                    perror("Client has gone");
                    break;
                } else if (errno == EWOULDBLOCK) {
                    printf("Wouldblock\n");
                    break;
                }
            }
        }

        do_smtp(session);

        if (fds[0].revents & POLLOUT && session->outcome_buffer_size) {
            int total_sended = 0;
            while (total_sended < session->outcome_buffer_size) {
                int sended = send(session->sock, session->outcome_buffer + total_sended, session->outcome_buffer_size, 0);
                if (sended > 0) {
                    total_sended += sended;
                } else if (sended == 0) {
                    perror("Client has gone");
                    session->in_progress = 0;
                    break;
                } else if (errno == EWOULDBLOCK) {
                    break;
                }
            }
            memmove(session->outcome_buffer, session->outcome_buffer + total_sended, session->outcome_buffer_size - total_sended + 1);
            session->outcome_buffer_size -= total_sended;
        }
    }
}

int run_worker(int parent_socket, int child_socket, config_t *cfg) {
    int child_pid = fork();
    if (child_pid < 0) {
        return 0;
    }
    if (child_pid > 0) {
        close(child_socket);
        return 1;
    }
    close(parent_socket);

    printf("CONNECTION ACCEPTED\n");
    struct user_session *session = create_user_session(child_socket, cfg);
    worker_loop(session);
    destroy_session(session);
    exit(EXIT_SUCCESS);
}
