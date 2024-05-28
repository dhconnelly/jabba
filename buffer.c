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
    *x = 0;
    if ((c = fgetc(buf->f)) == EOF && ferror(buf->f)) {
        return RESULT_ERRNO;
    } else if (c == EOF) {
        return RESULT_EOF;
    } else {
        *x = c;
        buf->offset++;
    }
    return RESULT_OK;
}

result buffer_uint16(buffer *buf, uint16_t *x) {
    int i;
    result result;
    uint8_t b = 0;
    for (i = 0; i < 2; i++) {
        if ((result = buffer_uint8(buf, &b)) != RESULT_OK) return result;
        *x = (*x << 8) | b;
    }
    return RESULT_OK;
}

result buffer_uint32(buffer *buf, uint32_t *x) {
    int i;
    result result;
    uint8_t b = 0;
    for (i = 0; i < 4; i++) {
        if ((result = buffer_uint8(buf, &b)) != RESULT_OK) return result;
        *x = (*x << 8) | b;
    }
    return RESULT_OK;
}
