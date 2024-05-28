#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buffer.h"
#include "class.h"
#include "logging.h"
#include "result.h"

static void disasm(FILE *f) {
    trace("disassembling...");

    buffer buf;
    buffer_init(&buf, f);
    class_file class;
    result r = parse_class(&buf, &class);
    if (r != RESULT_OK) {
        fatal("failed to read class at offset %d: %s",
                buf.offset, result_str(r));
    }

    int i;
    printf("magic: 0x%X\n", class.magic);
    printf("minor_version: %d\n", class.minor_version);
    printf("major version: %d\n", class.major_version);
    printf("constant pool count %d\n", class.constant_pool_count);
    for (i = 1; i < class.constant_pool_count; i++) {
        printf("cp_info[%d] = %s\n", i, cp_info_str(class.constant_pool[i]));
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) die("usage: disasm <class-file>");
    FILE *f = fopen(argv[1], "r");
    if (f == NULL) fatal("failed to open %s: %s", argv[1], strerror(errno));
    disasm(f);
    return 0;
}
