#ifndef _SESSION_H_
#define _SESSION_H_

#include "server-fsm.h"

struct user_session {
    int sock;
    char income_buffer[1 * 1024 * 1024];
    char outcome_buffer[100 * 1024];
    unsigned int income_buffer_size;
    unsigned int outcome_buffer_size;
    int in_progress;
    te_server_fsm_state state;
    int timedout;
    int reading_data;
    int welcomed;
    char *from;
    char *ehlo;
};

struct user_session *create_user_session(int sock);
void destroy_session(struct user_session *session);
char *get_line_from_session(struct user_session *session);
void append_to_output(struct user_session *session, char *data, int data_size);
#endif
