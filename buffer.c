#include "buffer.h"

#include <assert.h>
#include <errno.h>
#include <string.h>

#include "logging.h"

void buffer_init(buffer *buf, FILE *f) {
    buf->f = f;
    buf->offset = 0;
}

result buffer_uint8(buffer *buf, uint8_t *x) {
    int c;
    if ((c = fgetc(buf->f)) == EOF && ferror(buf->f)) {
        return RESULT_ERRNO;
    } else if (c == EOF) {
        return RESULT_EOF;
    } else {
        *x = c;
        buf->offset++;
        return RESULT_OK;
    }
}

#define DEFINE_READ(bytes, bits) \
    result buffer_uint##bits (buffer *buf, uint##bits##_t *x) { \
        int i; \
        result result; \
        uint8_t b = 0; \
        for (i = 0; i < bytes; i++) { \
            if ((result = buffer_uint8(buf, &b)) != RESULT_OK) return result; \
            *x = (*x << 8) | b; \
        } \
        return RESULT_OK; \
    }

DEFINE_READ(4, 32)
DEFINE_READ(2, 16)
