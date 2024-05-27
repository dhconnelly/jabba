#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void trace(char *msg) {
    printf("LOG: %s\n", msg);
}

static void disasm(FILE *f) {
    trace("disassembling...");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: disasm <class-file>\n");
        exit(1);
    }
    FILE *f = fopen(argv[1], "r");
    if (f == NULL) {
        fprintf(stderr, "disasm: failed to open class file %s: %s\n",
                argv[1], strerror(errno));
        exit(1);
    }
    disasm(f);
    return 0;
}
