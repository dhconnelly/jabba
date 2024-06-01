#ifndef BUFFER_H_
#define BUFFER_H_

#include <stdint.h>
#include <stdio.h>

#include "result.h"

/* Utilities for reading binary data */

typedef struct buffer {
    FILE *f;
    int offset;
} buffer;

void buffer_init(buffer *buf, FILE *f);

result buffer_uint8(buffer *buf, uint8_t *x);
result buffer_uint16(buffer *buf, uint16_t *x);
result buffer_uint32(buffer *buf, uint32_t *x);

#define UINT_OR_RETURN(bits, lvalue, b) \
    do { \
        result r = buffer_uint##bits(b, &lvalue); \
        if (r != RESULT_OK) return r; \
    } while (0)

#endif
