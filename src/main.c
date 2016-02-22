#include <libconfig.h>
#include <stdio.h>
#include <stdlib.h>
#include "listener.h"

static config_t cfg;

static void exit_handler() {
    config_destroy(&cfg);
}

int main(int argc, char *argv[]) {
    config_init(&cfg);
    atexit(exit_handler);
    if(!config_read_file(&cfg, "smtp.cfg")) {
        fprintf(stderr, "cannot read config\n");
        exit(EXIT_FAILURE);
    }
    server(&cfg);
    return 0;
}
