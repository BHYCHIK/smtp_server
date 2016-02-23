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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>


#include "session.h"
#include "smtp.h"
#include "logger.h"

static double get_milis_time() {
    struct timeval  tv;
    gettimeofday(&tv, NULL);

    double time_in_mill = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000 ;
    return time_in_mill;
}

static void worker_loop(struct user_session *session) {
    int rc;

    struct pollfd fds[1];
    fds[0].fd = session->sock;

    int timeout = 30000;
    if (!config_lookup_int(session->cfg, "Timeout", &timeout)) timeout = 30000;

    double start = get_milis_time();

    fds[0].events = POLLIN | POLLOUT;
    while(session->in_progress || session->outcome_buffer_size) {
        rc = poll(fds, 1, timeout - (get_milis_time() - start));
        if (rc < 0) {
            send_to_log("poll failed");
            return;
        }
        double stop = get_milis_time();

        if ( (stop - start > (double)timeout) && (!session->timedout) ) {
            send_to_log("Timeout");
            session->timedout = 1;
            do_smtp(session);
            continue;
        }

        if (fds[0].revents & POLLIN) {
            start = get_milis_time();
            while (1) {
                int received = recv(session->sock, session->income_buffer + session->income_buffer_size, sizeof(session->income_buffer) - session->income_buffer_size - 1, 0);
                if (received > 0) {
                    session->income_buffer_size += received;
                    if (session->income_buffer_size == sizeof(session->income_buffer) - 1) {
                        send_to_log("Income buffer overflow");
                        return;
                    }
                    session->income_buffer[session->income_buffer_size] = '\0';
                } else if (received == 0) {
                    session->in_progress = 0;
                    send_to_log("client gone");
                    break;
                } else if (errno == EWOULDBLOCK) {
                    break;
                }
            }
        }

        if (!session->timedout) do_smtp(session);

        if (fds[0].revents & POLLOUT && session->outcome_buffer_size) {
            int total_sended = 0;
            while (total_sended < session->outcome_buffer_size) {
                int sended = send(session->sock, session->outcome_buffer + total_sended, session->outcome_buffer_size, 0);
                if (sended > 0) {
                    total_sended += sended;
                } else if (sended == 0) {
                    char log_line[10240];
                    snprintf(log_line, sizeof(log_line), "Client has gone");
                    send_to_log(log_line);
                    session->in_progress = 0;
                    break;
                } else if (errno == EWOULDBLOCK) {
                    break;
                }
            }
            memmove(session->outcome_buffer, session->outcome_buffer + total_sended, session->outcome_buffer_size - total_sended + 1);
            session->outcome_buffer_size -= total_sended;
        }
        /*
        if (session->outcome_buffer_size) {
            fds[0].events = POLLIN | POLLOUT;
        } else {
            fds[0].events = POLLIN;
        }*/
    }
}

int run_worker(int parent_socket, int child_socket, config_t *cfg, struct sockaddr_in *incoming_addr) {
    int child_pid = fork();
    if (child_pid < 0) {
        return 0;
    }
    if (child_pid > 0) {
        close(child_socket);
        return 1;
    }
    close(parent_socket);

    char log_line[10240];
    snprintf(log_line, sizeof(log_line), "Accepting connection from: %s", inet_ntoa(incoming_addr->sin_addr));
    send_to_log(log_line);
    struct user_session *session = create_user_session(child_socket, cfg, incoming_addr);
    worker_loop(session);
    destroy_session(session);
    exit(EXIT_SUCCESS);
}
