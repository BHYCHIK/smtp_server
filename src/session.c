#define _GNU_SOURCE
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <strings.h>
#include <sys/mman.h>
#include "session.h"

struct user_session *create_user_session(int sock, config_t *cfg, struct sockaddr_in *incoming_addr) {
    struct user_session *session = malloc(sizeof(struct user_session));
    session->sock = sock;
    session->addr = incoming_addr;
    session->income_buffer_size = 0;
    session->outcome_buffer_size = 0;
    session->income_buffer[0] = '\0';
    session->outcome_buffer[0] = '\0';
    session->in_progress = 1;
    session->state = SERVER_FSM_ST_INIT;
    session->timedout = 0;
    session->reading_data = 0;
    session->welcomed = 0;
    session->ehlo = NULL;
    session->from = NULL;
    session->recipients = NULL;
    if (!(session->data = mmap(0, DATA_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0))) {
        perror("Cannot mmap");
    }
    session->data_size = 0;
    session->cfg = cfg;
    return session;
}

int append_data(struct user_session *session, const char *data, int data_size) {
    if (session->data_size + data_size + 2 > DATA_SIZE) return 0;
    printf("APPENDING data: %s\n", data);
    memcpy(session->data + session->data_size, data, data_size);
    memcpy(session->data + session->data_size + data_size, "\r\n", 2);
    session->data_size += data_size + 2;
    return 1;
}

struct recipients_list *add_to_recipients(struct recipients_list *list, char *recipient) {
    if (!list) {
        list = malloc(sizeof(struct recipients_list));
        list->recipient = recipient;
        list->next = NULL;
        return list;
    }
    struct recipients_list *tmp = list;
    while(tmp->next) {
        if(!strcasecmp(tmp->recipient, recipient)) {
            printf("ALREADY HAVE: %s\n", recipient);
            return list;
        }
        tmp = tmp->next;
    }
    if(!strcasecmp(tmp->recipient, recipient)) {
        printf("ALREADY HAVE: %s\n", recipient);
        return list;
    }
    tmp->next = malloc(sizeof(struct recipients_list));
    tmp = tmp->next;
    tmp->next = NULL;
    tmp->recipient = recipient;

    return list;
}

void free_recipients_list(struct recipients_list *list) {
    struct recipients_list *tmp = NULL;
    while(list) {
        tmp = list;
        list = list->next;
        free(tmp->recipient);
        free(tmp);
    }
}

void destroy_session(struct user_session *session) {
    if (session->ehlo) free(session->ehlo);
    if (session->from) free(session->from);
    munmap(session->data, DATA_SIZE);
    free_recipients_list(session->recipients);
    close(session->sock);
    free(session);
}

void append_to_output(struct user_session *session, char *data, int data_size) {
    assert(session->outcome_buffer_size + data_size <= sizeof(session->outcome_buffer));
    memcpy(session->outcome_buffer + session->outcome_buffer_size, data, data_size);
    session->outcome_buffer_size += data_size;
    session->outcome_buffer[session->outcome_buffer_size] = '\0';
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
