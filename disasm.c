#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* logging */

static void trace(const char *msg) {
    printf("LOG: %s\n", msg);
}

static void die(const char *msg) {
    fprintf(stderr, "disasm: fatal error: %s\n", msg);
    exit(1);
}

/*
 * The Java class file format. For more details see:
 * https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html
 */

typedef struct ClassFile {
    uint32_t magic;
} ClassFile;

/* Utilities for reading binary data */

typedef enum result {
    RESULT_OK,
    RESULT_EOF,
    RESULT_ERR,
} result;

static const char *result_str(result r) {
    switch (r) {
        case RESULT_OK: return "ok";
        case RESULT_EOF: return "eof";
        case RESULT_ERR: return strerror(errno);
    }
    assert(0);
}

typedef struct buffer {
    FILE *f;
    int offset;
} buffer;

static void buffer_init(buffer *buf, FILE *f) {
    buf->f = f;
    buf->offset = 0;
}

static result buffer_uint32(buffer *buf, uint32_t *x) {
    int c, i;
    *x = 0;
    for (i = 0; i < 4; i++) {
        if ((c = fgetc(buf->f)) == EOF && ferror(buf->f)) {
            return RESULT_ERR;
        } else if (c == EOF) {
            return RESULT_EOF;
        } else {
            buf->offset++;
            *x = (*x << 8) | c;
        }
    }
    return RESULT_OK;
}

/* Disassembling */

static void disasm(FILE *f) {
    trace("disassembling...");
    buffer buf;
    buffer_init(&buf, f);
    uint32_t magic;
    result ok = buffer_uint32(&buf, &magic);
    if (ok != RESULT_OK) die(result_str(ok));
    printf("0x%X\n", magic);
}

/* Driver */

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
