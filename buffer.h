#ifndef BUFFER_H_
#define BUFFER_H_

#include <stdint.h>
#include <stdio.h>

/* Utilities for reading binary data */

typedef enum buffer_result {
    RESULT_OK,
    RESULT_EOF,
    RESULT_ERRNO,
} buffer_result;

const char *result_str(buffer_result result);

typedef struct buffer {
    FILE *f;
    int offset;
} buffer;

void buffer_init(buffer *buf, FILE *f);

buffer_result buffer_uint8(buffer *buf, uint8_t *x);
buffer_result buffer_uint32(buffer *buf, uint32_t *x);
buffer_result buffer_uint16(buffer *buf, uint16_t *x);

uint8_t buffer_uint8_or_die(buffer *buf);
uint32_t buffer_uint32_or_die(buffer *buf);
uint16_t buffer_uint16_or_die(buffer *buf);

#endif
