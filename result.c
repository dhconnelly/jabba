#include "result.h"

#include <assert.h>
#include <string.h>
#include <errno.h>

const char *result_str(result r) {
    switch (r) {
        case RESULT_OK: return "ok";
        case RESULT_EOF: return "eof";
        case RESULT_ERRNO: return strerror(errno);
    }
    assert(0);
}
