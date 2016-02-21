#include <stdlib.h>
#include <stdio.h>
#include <pcre.h>
#include <string.h>
#include "session.h"
#include "server-fsm.h"

pcre *mail_from_regex = NULL;
pcre *rcpt_to_regex = NULL;
pcre *quit_regex = NULL;
pcre *ehlo_regex = NULL;
pcre *helo_regex = NULL;
pcre *rset_regex = NULL;
pcre *data_regex = NULL;
pcre *dot_regex = NULL;
pcre *vrfy_regex = NULL;

__attribute__((constructor)) void regex_compile(){
    const char *pcreErrorStr = NULL;
    int pcreErrorOffset = 0;
    mail_from_regex = pcre_compile("(^MAIL FROM:.*?\\<(\\w+@[\\w\\.]+)\\>\\s*|^MAIL FROM:\\s*(\\w+@[\\w\\.]+)\\s*)$", PCRE_CASELESS, &pcreErrorStr, &pcreErrorOffset, NULL);
    rcpt_to_regex = pcre_compile("(^RCPT TO:.*?\\<(\\w+@[\\w\\.]+)\\>\\s*|^RCPT TO:\\s*(\\w+@[\\w\\.]+)\\s*)$", PCRE_CASELESS, &pcreErrorStr, &pcreErrorOffset, NULL);
    quit_regex = pcre_compile("^QUIT\\s*$", PCRE_CASELESS, &pcreErrorStr, &pcreErrorOffset, NULL);
    ehlo_regex = pcre_compile("^EHLO\\s+([^\\s]+)\\s*$", PCRE_CASELESS, &pcreErrorStr, &pcreErrorOffset, NULL);
    helo_regex = pcre_compile("^HELO\\s+([^\\s]+)\\s*$", PCRE_CASELESS, &pcreErrorStr, &pcreErrorOffset, NULL);
    vrfy_regex = pcre_compile("^VRFY\\s+([^\\s]+)\\s*$", PCRE_CASELESS, &pcreErrorStr, &pcreErrorOffset, NULL);
    rset_regex = pcre_compile("^RSET\\s*$", PCRE_CASELESS, &pcreErrorStr, &pcreErrorOffset, NULL);
    data_regex = pcre_compile("^DATA\\s*$", PCRE_CASELESS, &pcreErrorStr, &pcreErrorOffset, NULL);
    dot_regex = pcre_compile("^\\.$", PCRE_CASELESS, &pcreErrorStr, &pcreErrorOffset, NULL);
}

static te_server_fsm_event get_cmd(struct user_session *session, const char *line) {
    int vec[30];

    if (session->timedout)
        return SERVER_FSM_EV_TIMEOUT;

    if (session->state == SERVER_FSM_ST_CONTENT) {
        if (pcre_exec(dot_regex, 0, line, strlen(line), 0,  0, vec, 30) >= 0) {
            return SERVER_FSM_EV_DATA_TERMINATE;
        }
        return SERVER_FSM_EV_DATA;
    }

    if (pcre_exec(mail_from_regex, 0, line, strlen(line), 0,  0, vec, 30) >= 0) {
        printf("MAIL FROM FOUND\n");
        return SERVER_FSM_EV_MAIL;
    }

    if (pcre_exec(rcpt_to_regex, 0, line, strlen(line), 0,  0, vec, 30) >= 0) {
        printf("RCPT TO FOUND\n");
        return SERVER_FSM_EV_RCPT;
    }

    if (pcre_exec(quit_regex, 0, line, strlen(line), 0,  0, vec, 30) >= 0) {
        printf("QUIT FOUND\n");
        return SERVER_FSM_EV_QUIT;
    }

    if (pcre_exec(helo_regex, 0, line, strlen(line), 0,  0, vec, 30) >= 0) {
        printf("HELO FOUND\n");
        return SERVER_FSM_EV_HELO;
    }

    if (pcre_exec(ehlo_regex, 0, line, strlen(line), 0,  0, vec, 30) >= 0) {
        printf("EHLO FOUND\n");
        return SERVER_FSM_EV_EHLO;
    }

    if (pcre_exec(data_regex, 0, line, strlen(line), 0,  0, vec, 30) >= 0) {
        printf("DATA FOUND\n");
        return SERVER_FSM_EV_DATA;
    }

    if (pcre_exec(vrfy_regex, 0, line, strlen(line), 0,  0, vec, 30) >= 0) {
        printf("VRFY FOUND\n");
        return SERVER_FSM_EV_VRFY;
    }

    if (pcre_exec(rset_regex, 0, line, strlen(line), 0,  0, vec, 30) >= 0) {
        printf("RSET FOUND\n");
        return SERVER_FSM_EV_RSET;
    }

    printf("syntax ERROR\n");
    return SERVER_FSM_EV_ERROR;
}

int smtp_quit(struct user_session *session) {
    session->in_progress = 0;
    return 0;
}

int smtp_timeout(struct user_session *session) {
    session->in_progress = 0;
    return 0;
}

int smtp_helo(struct user_session *session, const char *cmd) {
    printf("HELLO HANDLE: %s", cmd);
    return 0;
}

int smtp_ehlo(struct user_session *session, const char *cmd) {
    printf("EHLO HANDLE: %s", cmd);
    return 0;
}

int do_smtp(struct user_session *session) {
    char *line = NULL;
    while ((line = get_line_from_session(session))) {
        te_server_fsm_event cmd = get_cmd(session, line);
        session->state = server_fsm_step(session->state, cmd, line, session);
        printf("NEW STATE: %d\n", session->state);
        free(line);
    }
    return 0;
}
