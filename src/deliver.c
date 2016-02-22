#define _GNU_SOURCE
#include <libconfig.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "session.h"
#include "deliver.h"

static int is_local(struct user_session *session, const char *recipient) {
    const char *domain = strrchr(recipient, '@');
    assert(domain);
    domain++;

    config_t *cfg = session->cfg;
    config_setting_t *setting;

    setting = config_lookup(cfg, "LocalDomains");
    if (!setting) return 0;

    int count = config_setting_length(setting);
    int i = 0;
    for(i = 0; i < count; ++i) {
        const char *local_domain = config_setting_get_string_elem (setting, i);
        if (!strcasecmp(domain, local_domain)) return 1;
    }
    return 0;
}

static int any_remote(struct user_session *session) {
    struct recipients_list* list = session->recipients;
    while (list) {
        if (!is_local(session, list->recipient)) {
            return 1;
        }
        list = list->next;
    }
    return 0;
}

static int get_fd(const char *dirpath, char *filepath, int filepath_size) {
    snprintf(filepath, filepath_size, "%s/tmp/mailXXXXXX", dirpath);
    return mkstemp(filepath);
}

static int write_file(struct user_session *session, const char *delivered_to, int fd) {
    FILE *file = fdopen(fd, "w");
    int size = session->data_size ? session->data_size - 2 : 0;
    fprintf(file, "Return-Path: %s\r\n%s\r\n%.*s", session->from, delivered_to, size, session->data);
    printf("%s\n", session->data);
    fflush(file);
    fclose(file);
    return 0;
}

static int make_permament(const char *mailbox, const char *fname) {
    char *name = strrchr(fname, '/') + 1;
    char perm_path[1024];
    snprintf(perm_path, sizeof(perm_path), "%s/new/%s", mailbox, name);
    if (!rename(fname, perm_path)) {
        perror("CANNOT RENAME");
    }
    return 0;
}

static int remote_deliver(struct user_session *session) {
    const char *remote_mailbox = NULL;
    if (!config_lookup_string(session->cfg, "RemoteMailbox", &remote_mailbox)) assert(0);

    char filepath_tmp[1024];
    int tmp_fd = get_fd(remote_mailbox, filepath_tmp, sizeof(filepath_tmp));

    char *delivered_to = NULL;
    asprintf(&delivered_to, "Delivered-To: %s", session->recipients->recipient);
    struct recipients_list* list = session->recipients->next;
    while (list) {
        char *tmp = NULL;
        asprintf(&tmp, "%s, %s", delivered_to, list->recipient);
        free(delivered_to);
        delivered_to = tmp;
        list = list->next;
    }
    write_file(session,delivered_to, tmp_fd);
    free(delivered_to);
    make_permament(remote_mailbox, filepath_tmp);

    return 0;
}

static int local_deliver(struct user_session *session, const char *recipient) {
    const char *local_mailbox = NULL;
    if (!config_lookup_string(session->cfg, "LocalMailbox", &local_mailbox)) assert(0);

    char filepath_tmp[1024];
    int tmp_fd = get_fd(local_mailbox, filepath_tmp, sizeof(filepath_tmp));

    char *delivered_to = NULL;
    asprintf(&delivered_to, "Delivered-To: %s", recipient);
    write_file(session,delivered_to, tmp_fd);
    free(delivered_to);
    make_permament(local_mailbox, filepath_tmp);

    return 0;
}

int deliver(struct user_session *session) {
    if (any_remote(session)) remote_deliver(session);
    struct recipients_list* list = session->recipients;
    while (list) {
        if (is_local(session, list->recipient)) {
            local_deliver(session, list->recipient);
        }
        list = list->next;
    }
    return 0;
}
