#include "class.h"

#define READ_OR_RETURN(bits, lvalue, b) \
    do { \
        uint##bits##_t x; \
        result r = buffer_uint##bits(b, &x); \
        if (r != RESULT_OK) return r; \
        lvalue = x; \
    } while (0)

result parse_class(buffer *buf, class_file *class) {
    READ_OR_RETURN(32, class->magic, buf);
    READ_OR_RETURN(16, class->minor_version, buf);
    READ_OR_RETURN(16, class->major_version, buf);
    READ_OR_RETURN(16, class->constant_pool_count, buf);
    return RESULT_OK;
}
