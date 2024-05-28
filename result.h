#ifndef RESULT_H_
#define RESULT_H_

typedef enum result {
    RESULT_OK,
    RESULT_EOF,
    RESULT_ERRNO,
} result;

const char *result_str(result r);

#endif
