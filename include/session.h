#ifndef _SESSION_H_
#define _SESSION_H_

#include "server-fsm.h"
#include <libconfig.h>

#define DATA_SIZE 70 * 1024 * 1024

struct recipients_list {
    char* recipient;
    struct recipients_list *next;
};

struct user_session {
    int sock;
    struct sockaddr_in *addr;
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
    struct recipients_list *recipients;
    char *data;
    int data_size;
    config_t *cfg;
};

struct user_session *create_user_session(int sock, config_t *cfg, struct sockaddr_in *incoming_addr);
void destroy_session(struct user_session *session);
char *get_line_from_session(struct user_session *session);
void append_to_output(struct user_session *session, char *data, int data_size);
void free_recipients_list(struct recipients_list *list);
struct recipients_list *add_to_recipients(struct recipients_list *list, char *recipient);
int append_data(struct user_session *session, const char *data, int data_size);
#endif
