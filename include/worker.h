#ifndef _WORKER_H_
#define _WORKER_H_
#include <libconfig.h>
int run_worker(int parent_socket, int child_socket, config_t *cfg);
#endif
