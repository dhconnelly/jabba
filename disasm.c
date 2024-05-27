#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buffer.h"
#include "class.h"
#include "logging.h"

static void disasm(FILE *f) {
    trace("disassembling...");

    buffer buf;
    buffer_init(&buf, f);
    ClassFile class;
    class.magic = buffer_uint32_or_die(&buf);
    class.minor_version = buffer_uint16_or_die(&buf);
    class.major_version = buffer_uint16_or_die(&buf);

    printf("magic: 0x%X\n", class.magic);
    printf("minor_version: %d\n", class.minor_version);
    printf("major version: %d\n", class.major_version);
}

int main(int argc, char *argv[]) {
    if (argc != 2) die("usage: disasm <class-file>");
    FILE *f = fopen(argv[1], "r");
    if (f == NULL) fatal("failed to open %s: %s", argv[1], strerror(errno));
    disasm(f);
    return 0;
}
