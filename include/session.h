#ifndef _SESSION_H_
#define _SESSION_H_
struct user_session {
    int sock;
    char income_buffer[1 * 1024 * 1024];
    char outcome_buffer[100 * 1024];
    unsigned int income_buffer_size;
    unsigned int outcome_buffer_size;
};

struct user_session *create_user_session(int sock);
void destroy_session(struct user_session *session);
char *get_line_from_session(struct user_session *session);
#endif
