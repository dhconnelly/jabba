#include "logging.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void trace(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "LOG: ");
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
}

void fatal(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "FATAL: ");
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
    exit(1);
}

void die(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
    exit(1);
}
