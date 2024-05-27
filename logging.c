#include "logging.h"

#include <stdio.h>
#include <stdlib.h>

void trace(const char *msg) {
    fprintf(stderr, "LOG: %s\n", msg);
}

void fatal(const char *msg) {
    fprintf(stderr, "FATAL: %s\n", msg);
    exit(1);
}

void die(const char *msg) {
    fprintf(stderr, "%s\n", msg);
    exit(1);
}
