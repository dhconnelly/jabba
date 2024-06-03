#ifndef RESULT_H_
#define RESULT_H_

/* TODO: replace this with a string reason and success/failure */
typedef enum result {
    RESULT_OK,
    RESULT_EOF,
    RESULT_ERRNO,
    RESULT_INVALID_ATTR,
    RESULT_INVALID_CP,
} result;

const char *result_str(result r);

#endif
