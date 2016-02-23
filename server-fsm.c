/*   -*- buffer-read-only: t -*- vi: set ro:
 *
 *  DO NOT EDIT THIS FILE   (server-fsm.c)
 *
 *  It has been AutoGen-ed  February 23, 2016 at 02:54:58 PM by AutoGen 5.18
 *  From the definitions    src/server.def
 *  and the template file   fsm
 *
 *  Automated Finite State Machine
 *
 *  Copyright (C) 1992-2013 Bruce Korb - all rights reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name ``Bruce Korb'' nor the name of any other
 *    contributor may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * AutoFSM IS PROVIDED BY Bruce Korb ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL Bruce Korb OR ANY OTHER CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#define DEFINE_FSM
#include "server-fsm.h"
#include <stdio.h>

/*
 *  Do not make changes to this file, except between the START/END
 *  comments, or it will be removed the next time it is generated.
 */
/* START === USER HEADERS === DO NOT CHANGE THIS COMMENT */
/* END   === USER HEADERS === DO NOT CHANGE THIS COMMENT */

#ifndef NULL
#  define NULL 0
#endif

/**
 *  Enumeration of the valid transition types
 *  Some transition types may be common to several transitions.
 */
typedef enum {
    SERVER_FSM_TR_CONTENT_DATA,
    SERVER_FSM_TR_CONTENT_DATA_TERMINATE,
    SERVER_FSM_TR_CONTENT_ERROR,
    SERVER_FSM_TR_CONTENT_MAIL,
    SERVER_FSM_TR_CONTENT_QUIT,
    SERVER_FSM_TR_CONTENT_TIMEOUT,
    SERVER_FSM_TR_FROM_DATA,
    SERVER_FSM_TR_FROM_DATA_TERMINATE,
    SERVER_FSM_TR_FROM_EHLO,
    SERVER_FSM_TR_FROM_ERROR,
    SERVER_FSM_TR_FROM_HELO,
    SERVER_FSM_TR_FROM_MAIL,
    SERVER_FSM_TR_FROM_QUIT,
    SERVER_FSM_TR_FROM_RCPT,
    SERVER_FSM_TR_FROM_RSET,
    SERVER_FSM_TR_FROM_TIMEOUT,
    SERVER_FSM_TR_FROM_VRFY,
    SERVER_FSM_TR_INIT_DATA,
    SERVER_FSM_TR_INIT_DATA_TERMINATE,
    SERVER_FSM_TR_INIT_EHLO,
    SERVER_FSM_TR_INIT_ERROR,
    SERVER_FSM_TR_INIT_HELO,
    SERVER_FSM_TR_INIT_MAIL,
    SERVER_FSM_TR_INIT_QUIT,
    SERVER_FSM_TR_INIT_RCPT,
    SERVER_FSM_TR_INIT_RSET,
    SERVER_FSM_TR_INIT_TIMEOUT,
    SERVER_FSM_TR_INIT_VRFY,
    SERVER_FSM_TR_INVALID,
    SERVER_FSM_TR_OPENED_DATA,
    SERVER_FSM_TR_OPENED_DATA_TERMINATE,
    SERVER_FSM_TR_OPENED_EHLO,
    SERVER_FSM_TR_OPENED_ERROR,
    SERVER_FSM_TR_OPENED_HELO,
    SERVER_FSM_TR_OPENED_MAIL,
    SERVER_FSM_TR_OPENED_QUIT,
    SERVER_FSM_TR_OPENED_RCPT,
    SERVER_FSM_TR_OPENED_RSET,
    SERVER_FSM_TR_OPENED_TIMEOUT,
    SERVER_FSM_TR_OPENED_VRFY,
    SERVER_FSM_TR_RECIPIENT_DATA,
    SERVER_FSM_TR_RECIPIENT_DATA_TERMINATE,
    SERVER_FSM_TR_RECIPIENT_EHLO,
    SERVER_FSM_TR_RECIPIENT_ERROR,
    SERVER_FSM_TR_RECIPIENT_HELO,
    SERVER_FSM_TR_RECIPIENT_MAIL,
    SERVER_FSM_TR_RECIPIENT_QUIT,
    SERVER_FSM_TR_RECIPIENT_RCPT,
    SERVER_FSM_TR_RECIPIENT_RSET,
    SERVER_FSM_TR_RECIPIENT_TIMEOUT,
    SERVER_FSM_TR_RECIPIENT_VRFY
} te_server_fsm_trans;
#define SERVER_FSM_TRANSITION_CT  51

/**
 *  State transition handling map.  Map the state enumeration and the event
 *  enumeration to the new state and the transition enumeration code (in that
 *  order).  It is indexed by first the current state and then the event code.
 */
typedef struct server_fsm_transition t_server_fsm_transition;
struct server_fsm_transition {
    te_server_fsm_state  next_state;
    te_server_fsm_trans  transition;
};
static const t_server_fsm_transition
server_fsm_trans_table[ SERVER_FSM_STATE_CT ][ SERVER_FSM_EVENT_CT ] = {

  /* STATE 0:  SERVER_FSM_ST_INIT */
  { { SERVER_FSM_ST_OPENED, SERVER_FSM_TR_INIT_HELO }, /* EVT:  HELO */
    { SERVER_FSM_ST_OPENED, SERVER_FSM_TR_INIT_EHLO }, /* EVT:  EHLO */
    { SERVER_FSM_ST_INIT, SERVER_FSM_TR_INIT_RSET }, /* EVT:  RSET */
    { SERVER_FSM_ST_DONE, SERVER_FSM_TR_INIT_TIMEOUT }, /* EVT:  TIMEOUT */
    { SERVER_FSM_ST_DONE, SERVER_FSM_TR_INIT_QUIT }, /* EVT:  QUIT */
    { SERVER_FSM_ST_INIT, SERVER_FSM_TR_INIT_MAIL }, /* EVT:  MAIL */
    { SERVER_FSM_ST_INIT, SERVER_FSM_TR_INIT_VRFY }, /* EVT:  VRFY */
    { SERVER_FSM_ST_INIT, SERVER_FSM_TR_INIT_RCPT }, /* EVT:  RCPT */
    { SERVER_FSM_ST_INIT, SERVER_FSM_TR_INIT_DATA }, /* EVT:  DATA */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INIT_DATA_TERMINATE }, /* EVT:  DATA_TERMINATE */
    { SERVER_FSM_ST_INIT, SERVER_FSM_TR_INIT_ERROR } /* EVT:  ERROR */
  },


  /* STATE 1:  SERVER_FSM_ST_OPENED */
  { { SERVER_FSM_ST_OPENED, SERVER_FSM_TR_OPENED_HELO }, /* EVT:  HELO */
    { SERVER_FSM_ST_OPENED, SERVER_FSM_TR_OPENED_EHLO }, /* EVT:  EHLO */
    { SERVER_FSM_ST_OPENED, SERVER_FSM_TR_OPENED_RSET }, /* EVT:  RSET */
    { SERVER_FSM_ST_DONE, SERVER_FSM_TR_OPENED_TIMEOUT }, /* EVT:  TIMEOUT */
    { SERVER_FSM_ST_DONE, SERVER_FSM_TR_OPENED_QUIT }, /* EVT:  QUIT */
    { SERVER_FSM_ST_FROM, SERVER_FSM_TR_OPENED_MAIL }, /* EVT:  MAIL */
    { SERVER_FSM_ST_OPENED, SERVER_FSM_TR_OPENED_VRFY }, /* EVT:  VRFY */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_OPENED_RCPT }, /* EVT:  RCPT */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_OPENED_DATA }, /* EVT:  DATA */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_OPENED_DATA_TERMINATE }, /* EVT:  DATA_TERMINATE */
    { SERVER_FSM_ST_OPENED, SERVER_FSM_TR_OPENED_ERROR } /* EVT:  ERROR */
  },


  /* STATE 2:  SERVER_FSM_ST_FROM */
  { { SERVER_FSM_ST_OPENED, SERVER_FSM_TR_FROM_HELO }, /* EVT:  HELO */
    { SERVER_FSM_ST_OPENED, SERVER_FSM_TR_FROM_EHLO }, /* EVT:  EHLO */
    { SERVER_FSM_ST_OPENED, SERVER_FSM_TR_FROM_RSET }, /* EVT:  RSET */
    { SERVER_FSM_ST_DONE, SERVER_FSM_TR_FROM_TIMEOUT }, /* EVT:  TIMEOUT */
    { SERVER_FSM_ST_DONE, SERVER_FSM_TR_FROM_QUIT }, /* EVT:  QUIT */
    { SERVER_FSM_ST_FROM, SERVER_FSM_TR_FROM_MAIL }, /* EVT:  MAIL */
    { SERVER_FSM_ST_FROM, SERVER_FSM_TR_FROM_VRFY }, /* EVT:  VRFY */
    { SERVER_FSM_ST_RECIPIENT, SERVER_FSM_TR_FROM_RCPT }, /* EVT:  RCPT */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_FROM_DATA }, /* EVT:  DATA */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_FROM_DATA_TERMINATE }, /* EVT:  DATA_TERMINATE */
    { SERVER_FSM_ST_OPENED, SERVER_FSM_TR_FROM_ERROR } /* EVT:  ERROR */
  },


  /* STATE 3:  SERVER_FSM_ST_RECIPIENT */
  { { SERVER_FSM_ST_OPENED, SERVER_FSM_TR_RECIPIENT_HELO }, /* EVT:  HELO */
    { SERVER_FSM_ST_OPENED, SERVER_FSM_TR_RECIPIENT_EHLO }, /* EVT:  EHLO */
    { SERVER_FSM_ST_OPENED, SERVER_FSM_TR_RECIPIENT_RSET }, /* EVT:  RSET */
    { SERVER_FSM_ST_DONE, SERVER_FSM_TR_RECIPIENT_TIMEOUT }, /* EVT:  TIMEOUT */
    { SERVER_FSM_ST_DONE, SERVER_FSM_TR_RECIPIENT_QUIT }, /* EVT:  QUIT */
    { SERVER_FSM_ST_FROM, SERVER_FSM_TR_RECIPIENT_MAIL }, /* EVT:  MAIL */
    { SERVER_FSM_ST_RECIPIENT, SERVER_FSM_TR_RECIPIENT_VRFY }, /* EVT:  VRFY */
    { SERVER_FSM_ST_RECIPIENT, SERVER_FSM_TR_RECIPIENT_RCPT }, /* EVT:  RCPT */
    { SERVER_FSM_ST_CONTENT, SERVER_FSM_TR_RECIPIENT_DATA }, /* EVT:  DATA */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_RECIPIENT_DATA_TERMINATE }, /* EVT:  DATA_TERMINATE */
    { SERVER_FSM_ST_RECIPIENT, SERVER_FSM_TR_RECIPIENT_ERROR } /* EVT:  ERROR */
  },


  /* STATE 4:  SERVER_FSM_ST_CONTENT */
  { { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  HELO */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  EHLO */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  RSET */
    { SERVER_FSM_ST_DONE, SERVER_FSM_TR_CONTENT_TIMEOUT }, /* EVT:  TIMEOUT */
    { SERVER_FSM_ST_DONE, SERVER_FSM_TR_CONTENT_QUIT }, /* EVT:  QUIT */
    { SERVER_FSM_ST_FROM, SERVER_FSM_TR_CONTENT_MAIL }, /* EVT:  MAIL */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  VRFY */
    { SERVER_FSM_ST_INVALID, SERVER_FSM_TR_INVALID }, /* EVT:  RCPT */
    { SERVER_FSM_ST_CONTENT, SERVER_FSM_TR_CONTENT_DATA }, /* EVT:  DATA */
    { SERVER_FSM_ST_OPENED, SERVER_FSM_TR_CONTENT_DATA_TERMINATE }, /* EVT:  DATA_TERMINATE */
    { SERVER_FSM_ST_OPENED, SERVER_FSM_TR_CONTENT_ERROR } /* EVT:  ERROR */
  }
};


#define Server_FsmFsmErr_off     19
#define Server_FsmEvInvalid_off  75
#define Server_FsmStInit_off     83


static char const zServer_FsmStrings[187] =
/*     0 */ "** OUT-OF-RANGE **\0"
/*    19 */ "FSM Error:  in state %d (%s), event %d (%s) is invalid\n\0"
/*    75 */ "invalid\0"
/*    83 */ "init\0"
/*    88 */ "opened\0"
/*    95 */ "from\0"
/*   100 */ "recipient\0"
/*   110 */ "content\0"
/*   118 */ "helo\0"
/*   123 */ "ehlo\0"
/*   128 */ "rset\0"
/*   133 */ "timeout\0"
/*   141 */ "quit\0"
/*   146 */ "mail\0"
/*   151 */ "vrfy\0"
/*   156 */ "rcpt\0"
/*   161 */ "data\0"
/*   166 */ "data_terminate\0"
/*   181 */ "error";

static const size_t aszServer_FsmStates[5] = {
    83,  88,  95,  100, 110 };

static const size_t aszServer_FsmEvents[12] = {
    118, 123, 128, 133, 141, 146, 151, 156, 161, 166, 181, 75 };


#define SERVER_FSM_EVT_NAME(t)   ( (((unsigned)(t)) >= 12) \
    ? zServer_FsmStrings : zServer_FsmStrings + aszServer_FsmEvents[t])

#define SERVER_FSM_STATE_NAME(s) ( (((unsigned)(s)) >= 5) \
    ? zServer_FsmStrings : zServer_FsmStrings + aszServer_FsmStates[s])

#ifndef EXIT_FAILURE
# define EXIT_FAILURE 1
#endif

static int server_fsm_invalid_transition( te_server_fsm_state st, te_server_fsm_event evt );

/* * * * * * * * * THE CODE STARTS HERE * * * * * * * */
/**
 *  Print out an invalid transition message and return EXIT_FAILURE
 */
static int
server_fsm_invalid_transition( te_server_fsm_state st, te_server_fsm_event evt )
{
    /* START == INVALID TRANS MSG == DO NOT CHANGE THIS COMMENT */
    char const * fmt = zServer_FsmStrings + Server_FsmFsmErr_off;
    fprintf( stderr, fmt, st, SERVER_FSM_STATE_NAME(st), evt, SERVER_FSM_EVT_NAME(evt));
    /* END   == INVALID TRANS MSG == DO NOT CHANGE THIS COMMENT */

    return EXIT_FAILURE;
}

/**
 *  Step the FSM.  Returns the resulting state.  If the current state is
 *  SERVER_FSM_ST_DONE or SERVER_FSM_ST_INVALID, it resets to
 *  SERVER_FSM_ST_INIT and returns SERVER_FSM_ST_INIT.
 */
te_server_fsm_state
server_fsm_step(
    te_server_fsm_state server_fsm_state,
    te_server_fsm_event trans_evt,
    const char *command,
    void *session )
{
    te_server_fsm_state nxtSt;
    te_server_fsm_trans trans;

    if ((unsigned)server_fsm_state >= SERVER_FSM_ST_INVALID) {
        return SERVER_FSM_ST_INIT;
    }

#ifndef __COVERITY__
    if (trans_evt >= SERVER_FSM_EV_INVALID) {
        nxtSt = SERVER_FSM_ST_INVALID;
        trans = SERVER_FSM_TR_INVALID;
    } else
#endif /* __COVERITY__ */
    {
        const t_server_fsm_transition* pTT =
            server_fsm_trans_table[ server_fsm_state ] + trans_evt;
        nxtSt = pTT->next_state;
        trans = pTT->transition;
    }


    switch (trans) {
    case SERVER_FSM_TR_CONTENT_DATA:
        /* START == CONTENT_DATA == DO NOT CHANGE THIS COMMENT */
        smtp_data_cont(session, command);
        /* END   == CONTENT_DATA == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_CONTENT_DATA_TERMINATE:
        /* START == CONTENT_DATA_TERMINATE == DO NOT CHANGE THIS COMMENT */
        smtp_data_end(session, command);
        /* END   == CONTENT_DATA_TERMINATE == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_CONTENT_ERROR:
        /* START == CONTENT_ERROR == DO NOT CHANGE THIS COMMENT */
        smtp_unknown_command(session, command);
        /* END   == CONTENT_ERROR == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_CONTENT_MAIL:
        /* START == CONTENT_MAIL == DO NOT CHANGE THIS COMMENT */
        smtp_mail_from(session, command);
        /* END   == CONTENT_MAIL == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_CONTENT_QUIT:
        /* START == CONTENT_QUIT == DO NOT CHANGE THIS COMMENT */
        smtp_quit(session);
        /* END   == CONTENT_QUIT == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_CONTENT_TIMEOUT:
        /* START == CONTENT_TIMEOUT == DO NOT CHANGE THIS COMMENT */
        smtp_timeout(session);
        /* END   == CONTENT_TIMEOUT == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_FROM_DATA:
        /* START == FROM_DATA == DO NOT CHANGE THIS COMMENT */
        smtp_unknown_command(session, command);
        /* END   == FROM_DATA == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_FROM_DATA_TERMINATE:
        /* START == FROM_DATA_TERMINATE == DO NOT CHANGE THIS COMMENT */
        smtp_unknown_command(session, command);
        /* END   == FROM_DATA_TERMINATE == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_FROM_EHLO:
        /* START == FROM_EHLO == DO NOT CHANGE THIS COMMENT */
        smtp_ehlo(session, command);
        /* END   == FROM_EHLO == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_FROM_ERROR:
        /* START == FROM_ERROR == DO NOT CHANGE THIS COMMENT */
        smtp_unknown_command(session, command);
        /* END   == FROM_ERROR == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_FROM_HELO:
        /* START == FROM_HELO == DO NOT CHANGE THIS COMMENT */
        smtp_helo(session, command);
        /* END   == FROM_HELO == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_FROM_MAIL:
        /* START == FROM_MAIL == DO NOT CHANGE THIS COMMENT */
        smtp_mail_from(session, command);
        /* END   == FROM_MAIL == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_FROM_QUIT:
        /* START == FROM_QUIT == DO NOT CHANGE THIS COMMENT */
        smtp_quit(session);
        /* END   == FROM_QUIT == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_FROM_RCPT:
        /* START == FROM_RCPT == DO NOT CHANGE THIS COMMENT */
        smtp_rcpt_to(session, command);
        /* END   == FROM_RCPT == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_FROM_RSET:
        /* START == FROM_RSET == DO NOT CHANGE THIS COMMENT */
        smtp_rset(session, command);
        /* END   == FROM_RSET == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_FROM_TIMEOUT:
        /* START == FROM_TIMEOUT == DO NOT CHANGE THIS COMMENT */
        smtp_timeout(session);
        /* END   == FROM_TIMEOUT == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_FROM_VRFY:
        /* START == FROM_VRFY == DO NOT CHANGE THIS COMMENT */
        smtp_verify(session, command);
        /* END   == FROM_VRFY == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_INIT_DATA:
        /* START == INIT_DATA == DO NOT CHANGE THIS COMMENT */
        smtp_unknown_command(session, command);
        /* END   == INIT_DATA == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_INIT_DATA_TERMINATE:
        /* START == INIT_DATA_TERMINATE == DO NOT CHANGE THIS COMMENT */
        smtp_unknown_command(session, command);
        /* END   == INIT_DATA_TERMINATE == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_INIT_EHLO:
        /* START == INIT_EHLO == DO NOT CHANGE THIS COMMENT */
        smtp_ehlo(session, command);
        /* END   == INIT_EHLO == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_INIT_ERROR:
        /* START == INIT_ERROR == DO NOT CHANGE THIS COMMENT */
        smtp_unknown_command(session, command);
        /* END   == INIT_ERROR == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_INIT_HELO:
        /* START == INIT_HELO == DO NOT CHANGE THIS COMMENT */
        smtp_helo(session, command);
        /* END   == INIT_HELO == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_INIT_MAIL:
        /* START == INIT_MAIL == DO NOT CHANGE THIS COMMENT */
        smtp_unknown_command(session, command);
        /* END   == INIT_MAIL == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_INIT_QUIT:
        /* START == INIT_QUIT == DO NOT CHANGE THIS COMMENT */
        smtp_quit(session);
        /* END   == INIT_QUIT == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_INIT_RCPT:
        /* START == INIT_RCPT == DO NOT CHANGE THIS COMMENT */
        smtp_unknown_command(session, command);
        /* END   == INIT_RCPT == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_INIT_RSET:
        /* START == INIT_RSET == DO NOT CHANGE THIS COMMENT */
        smtp_rset(session, command);
        /* END   == INIT_RSET == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_INIT_TIMEOUT:
        /* START == INIT_TIMEOUT == DO NOT CHANGE THIS COMMENT */
        smtp_timeout(session);
        /* END   == INIT_TIMEOUT == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_INIT_VRFY:
        /* START == INIT_VRFY == DO NOT CHANGE THIS COMMENT */
        smtp_unknown_command(session, command);
        /* END   == INIT_VRFY == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_INVALID:
        /* START == INVALID == DO NOT CHANGE THIS COMMENT */
        exit(server_fsm_invalid_transition(server_fsm_state, trans_evt));
        /* END   == INVALID == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_OPENED_DATA:
        /* START == OPENED_DATA == DO NOT CHANGE THIS COMMENT */
        smtp_unknown_command(session, command);
        /* END   == OPENED_DATA == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_OPENED_DATA_TERMINATE:
        /* START == OPENED_DATA_TERMINATE == DO NOT CHANGE THIS COMMENT */
        smtp_unknown_command(session, command);
        /* END   == OPENED_DATA_TERMINATE == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_OPENED_EHLO:
        /* START == OPENED_EHLO == DO NOT CHANGE THIS COMMENT */
        smtp_ehlo(session, command);
        /* END   == OPENED_EHLO == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_OPENED_ERROR:
        /* START == OPENED_ERROR == DO NOT CHANGE THIS COMMENT */
        smtp_unknown_command(session, command);
        /* END   == OPENED_ERROR == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_OPENED_HELO:
        /* START == OPENED_HELO == DO NOT CHANGE THIS COMMENT */
        smtp_helo(session, command);
        /* END   == OPENED_HELO == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_OPENED_MAIL:
        /* START == OPENED_MAIL == DO NOT CHANGE THIS COMMENT */
        smtp_mail_from(session, command);
        /* END   == OPENED_MAIL == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_OPENED_QUIT:
        /* START == OPENED_QUIT == DO NOT CHANGE THIS COMMENT */
        smtp_quit(session);
        /* END   == OPENED_QUIT == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_OPENED_RCPT:
        /* START == OPENED_RCPT == DO NOT CHANGE THIS COMMENT */
        smtp_unknown_command(session, command);
        /* END   == OPENED_RCPT == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_OPENED_RSET:
        /* START == OPENED_RSET == DO NOT CHANGE THIS COMMENT */
        smtp_rset(session, command);
        /* END   == OPENED_RSET == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_OPENED_TIMEOUT:
        /* START == OPENED_TIMEOUT == DO NOT CHANGE THIS COMMENT */
        smtp_timeout(session);
        /* END   == OPENED_TIMEOUT == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_OPENED_VRFY:
        /* START == OPENED_VRFY == DO NOT CHANGE THIS COMMENT */
        smtp_verify(session, command);
        /* END   == OPENED_VRFY == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_RECIPIENT_DATA:
        /* START == RECIPIENT_DATA == DO NOT CHANGE THIS COMMENT */
        smtp_data_start(session, command);
        /* END   == RECIPIENT_DATA == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_RECIPIENT_DATA_TERMINATE:
        /* START == RECIPIENT_DATA_TERMINATE == DO NOT CHANGE THIS COMMENT */
        smtp_unknown_command(session, command);
        /* END   == RECIPIENT_DATA_TERMINATE == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_RECIPIENT_EHLO:
        /* START == RECIPIENT_EHLO == DO NOT CHANGE THIS COMMENT */
        smtp_ehlo(session, command);
        /* END   == RECIPIENT_EHLO == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_RECIPIENT_ERROR:
        /* START == RECIPIENT_ERROR == DO NOT CHANGE THIS COMMENT */
        smtp_unknown_command(session, command);
        /* END   == RECIPIENT_ERROR == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_RECIPIENT_HELO:
        /* START == RECIPIENT_HELO == DO NOT CHANGE THIS COMMENT */
        smtp_helo(session, command);
        /* END   == RECIPIENT_HELO == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_RECIPIENT_MAIL:
        /* START == RECIPIENT_MAIL == DO NOT CHANGE THIS COMMENT */
        smtp_mail_from(session, command);
        /* END   == RECIPIENT_MAIL == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_RECIPIENT_QUIT:
        /* START == RECIPIENT_QUIT == DO NOT CHANGE THIS COMMENT */
        smtp_quit(session);
        /* END   == RECIPIENT_QUIT == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_RECIPIENT_RCPT:
        /* START == RECIPIENT_RCPT == DO NOT CHANGE THIS COMMENT */
        smtp_rcpt_to(session, command);
        /* END   == RECIPIENT_RCPT == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_RECIPIENT_RSET:
        /* START == RECIPIENT_RSET == DO NOT CHANGE THIS COMMENT */
        smtp_rset(session, command);
        /* END   == RECIPIENT_RSET == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_RECIPIENT_TIMEOUT:
        /* START == RECIPIENT_TIMEOUT == DO NOT CHANGE THIS COMMENT */
        smtp_timeout(session);
        /* END   == RECIPIENT_TIMEOUT == DO NOT CHANGE THIS COMMENT */
        break;


    case SERVER_FSM_TR_RECIPIENT_VRFY:
        /* START == RECIPIENT_VRFY == DO NOT CHANGE THIS COMMENT */
        smtp_verify(session, command);
        /* END   == RECIPIENT_VRFY == DO NOT CHANGE THIS COMMENT */
        break;


    default:
        /* START == BROKEN MACHINE == DO NOT CHANGE THIS COMMENT */
        exit(server_fsm_invalid_transition(server_fsm_state, trans_evt));
        /* END   == BROKEN MACHINE == DO NOT CHANGE THIS COMMENT */
    }


    /* START == FINISH STEP == DO NOT CHANGE THIS COMMENT */
    /* END   == FINISH STEP == DO NOT CHANGE THIS COMMENT */

    return nxtSt;
}
/*
 * Local Variables:
 * mode: C
 * c-file-style: "stroustrup"
 * indent-tabs-mode: nil
 * End:
 * end of server-fsm.c */
