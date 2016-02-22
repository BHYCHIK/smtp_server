#include <stdlib.h>
#include <stdio.h>
#include <pcre.h>
#include <string.h>
#include "session.h"
#include "server-fsm.h"

#define SMTP_WELCOME "220 iremen.ru ESMTP\r\n"
#define SMTP_EHLO_REPLY "250-mx.iremen.ru ready to serve\r\n250 SIZE 73400320\r\n"
#define SMTP_OK_REPLY "250 OK\r\n"
#define SMTP_UNKNOWN_COMMAND_REPLY "500 Unknown command\r\n"
#define SMTP_DATA_REPLY "354 Enter mail, end with \".\" on a line by itself\r\n"
#define SMTP_QUIT_REPLY "221 mx.iremen.ru closing connection\r\n"
#define SMTP_VRFY_REPLY "550 User unknown\r\n"


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
    mail_from_regex = pcre_compile("(^MAIL FROM:.*?\\<([\\w\\.]+@[\\w\\.]+)\\>\\s*|^MAIL FROM:\\s*(\\w+@[\\w\\.]+)\\s*)$", PCRE_CASELESS, &pcreErrorStr, &pcreErrorOffset, NULL);
    rcpt_to_regex = pcre_compile("(^RCPT TO:.*?\\<([\\w\\.]+@[\\w\\.]+)\\>\\s*|^RCPT TO:\\s*(\\w+@[\\w\\.]+)\\s*)$", PCRE_CASELESS, &pcreErrorStr, &pcreErrorOffset, NULL);
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
    memset(vec, 0, 30*sizeof(int));

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
    append_to_output(session, SMTP_QUIT_REPLY, sizeof(SMTP_QUIT_REPLY) - 1);
    session->in_progress = 0;
    return 0;
}

int smtp_timeout(struct user_session *session) {
    session->in_progress = 0;
    return 0;
}

int smtp_helo(struct user_session *session, const char *cmd) {
    int vec[30];
    pcre_exec(ehlo_regex, 0, cmd, strlen(cmd), 0,  0, vec, 30);
    if (session->ehlo) free(session->ehlo);
    session->ehlo = strndup(cmd+ vec[2], vec[3]);
    printf("Setting helo: %s\n", session->ehlo);
    append_to_output(session, SMTP_EHLO_REPLY, sizeof(SMTP_EHLO_REPLY) - 1);
    return 0;
}

int smtp_ehlo(struct user_session *session, const char *cmd) {
    int vec[30];
    pcre_exec(ehlo_regex, 0, cmd, strlen(cmd), 0,  0, vec, 30);
    if (session->ehlo) free(session->ehlo);
    session->ehlo = strndup(cmd+ vec[2], vec[3]);
    printf("Setting ehlo: %s\n", session->ehlo);
    append_to_output(session, SMTP_EHLO_REPLY, sizeof(SMTP_EHLO_REPLY) - 1);
    return 0;
}

int smtp_verify(struct user_session *session, const char *cmd) {
    append_to_output(session, SMTP_VRFY_REPLY , sizeof(SMTP_VRFY_REPLY) - 1);
    return 0;
}

int smtp_mail_from(struct user_session *session, const char *cmd) {
    //DO NOT FORGET TO CLEAR RCPT ALSO
    if (session->from) free(session->from);
    int vec[30];
    pcre_exec(mail_from_regex, 0, cmd, strlen(cmd), 0,  0, vec, 30);
    if (vec[4] == -1) {
        session->from = strndup(cmd + vec[6], vec[7] - vec[6]);
    } else if (vec[6] == -1) {
        session->from = strndup(cmd + vec[4], vec[5] - vec[4]);
    }
    printf("FROM IS :\"%s\"\n", session->from);

    append_to_output(session, SMTP_OK_REPLY, sizeof(SMTP_OK_REPLY) - 1);
    return 0;
}

int smtp_rcpt_to(struct user_session *session, const char *cmd) {
    append_to_output(session, SMTP_OK_REPLY, sizeof(SMTP_OK_REPLY) - 1);
    printf("RCPT TO HANDLE: %s\n", cmd);
    return 0;
}

int smtp_rset(struct user_session *session, const char *cmd) {
    append_to_output(session, SMTP_OK_REPLY, sizeof(SMTP_OK_REPLY) - 1);
    printf("RSET HANDLE: %s\n", cmd);
    return 0;
}

int smtp_unknown_command(struct user_session *session, const char *cmd) {
    append_to_output(session, SMTP_UNKNOWN_COMMAND_REPLY, sizeof(SMTP_UNKNOWN_COMMAND_REPLY) - 1);
    printf("Unknown command: %s\n", cmd);
    return 0;
}

int smtp_data_start(struct user_session *session, const char *cmd) {
    append_to_output(session, SMTP_DATA_REPLY, sizeof(SMTP_DATA_REPLY) - 1);
    printf("START DATA\n");
    return 0;
}

int smtp_data_cont(struct user_session *session, const char *cmd) {
    printf("WRITING: %s\r\n", cmd);
    return 0;
}

int smtp_data_end(struct user_session *session, const char *cmd) {
    append_to_output(session, SMTP_OK_REPLY, sizeof(SMTP_OK_REPLY) - 1);
    printf("DATA FINISHED\n");
    return 0;
}

int do_smtp(struct user_session *session) {
    if (!session->welcomed) {
        session->welcomed = 1;
        append_to_output(session, SMTP_WELCOME, sizeof(SMTP_WELCOME) - 1);
    }

    char *line = NULL;
    while ((line = get_line_from_session(session))) {
        te_server_fsm_event cmd = get_cmd(session, line);
        session->state = server_fsm_step(session->state, cmd, line, session);
        printf("NEW STATE: %d\n", session->state);
        free(line);
    }
    return 0;
}
