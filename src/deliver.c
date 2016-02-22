#include <libconfig.h>

#include "session.h"
#include "deliver.h"
int deliver(struct user_session *session, const char *recipient) {
    printf("Deliver to %s\n", recipient);
    return 0;
}
