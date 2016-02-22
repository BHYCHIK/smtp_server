#ifndef _LOGGER_H_
#define _LOGGER_H_
#include <libconfig.h>
void send_to_log(const char *msg);
int prepare_logger(config_t *cfg, int sock);
#endif
