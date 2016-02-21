#define _GNU_SOURCE
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "session.h"
#include <stdio.h>

struct user_session *create_user_session(int sock) {
    struct user_session *session = malloc(sizeof(struct user_session));
    session->sock = sock;
    session->income_buffer_size = 0;
    session->outcome_buffer_size = 0;
    session->income_buffer[0] = '\0';
    session->outcome_buffer[0] = '\0';
    session->in_progress = 1;
    session->state = SERVER_FSM_ST_INIT;
    session->timedout = 0;
    session->reading_data = 0;
    return session;
}

void destroy_session(struct user_session *session) {
    close(session->sock);
    free(session);
}

char *get_line_from_session(struct user_session *session) {
    char *line = strstr(session->income_buffer, "\r\n");
    if (!line) {
        return NULL;
    }

    char *result = strndup(session->income_buffer, line - session->income_buffer);

    memmove(session->income_buffer, line + 2, session->income_buffer_size - (line + 2 - session->income_buffer) + 1);
    session->income_buffer_size -= line + 2 - session->income_buffer;

    return result;
}
