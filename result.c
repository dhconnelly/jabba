#include "result.h"

#include <assert.h>
#include <string.h>
#include <errno.h>

const char *result_str(result r) {
    switch (r) {
        case RESULT_OK: return "ok";
        case RESULT_EOF: return "eof";
        case RESULT_ERRNO: return strerror(errno);
        case RESULT_INVALID_ATTR: return "invalid attribute type";
        case RESULT_INVALID_CP: return "invalid constant pool tag";
    }
    assert(0);
}
