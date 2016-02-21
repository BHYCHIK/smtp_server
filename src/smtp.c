#include <stdlib.h>
#include <stdio.h>
#include "session.h"

int do_smtp(struct user_session *session) {
    char *line = NULL;
    int iter = 0;
    while ((line = get_line_from_session(session))) {
        printf("%d) %s\n", iter, line);
        ++iter;
        free(line);
    }
    return 0;
}
