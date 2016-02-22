#include <libconfig.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "listener.h"

static config_t cfg;

volatile int is_run = 1;

static void exit_handler() {
    config_destroy(&cfg);
}

static void int_handler(int signum) {
    signum = signum;
    is_run = 0;
}

int main(int argc, char *argv[]) {
    signal(SIGINT, int_handler);
    config_init(&cfg);
    atexit(exit_handler);
    if(!config_read_file(&cfg, "smtp.cfg")) {
        fprintf(stderr, "cannot read config\n");
        exit(EXIT_FAILURE);
    }
    server(&cfg);
    return 0;
}
