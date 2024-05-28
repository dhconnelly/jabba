#include "class.h"

#include <assert.h>
#include <stdlib.h>

#include "logging.h"

#define ASSIGN_UINT_OR_RETURN(bits, lvalue, b) \
    do { \
        result r = buffer_uint##bits(b, &lvalue); \
        if (r != RESULT_OK) return r; \
    } while (0)

#define ASSIGN_OR_RETURN(obj, lvalue, b) \
    do { \
        result r = parse_##obj(b, &lvalue); \
        if (r != RESULT_OK) return r; \
    } while (0)

const char *cp_tag_str(cp_tag tag) {
    switch (tag) {
        case CP_METHODREF: return "CONSTANT_Methodref";
        case CP_CLASS: return "CONSTANT_Class";
    }
    assert(0);
}

static result parse_cp_methodref(buffer *buf, cp_methodref *methodref) {
    ASSIGN_UINT_OR_RETURN(16, methodref->class_index, buf);
    ASSIGN_UINT_OR_RETURN(16, methodref->name_and_type_index, buf);
    return RESULT_OK;
}

static result parse_cp_class(buffer *buf, cp_class *class) {
    ASSIGN_UINT_OR_RETURN(16, class->name_index, buf);
    return RESULT_OK;
}

static const int kMaxStrLen = 128;

const char *cp_info_str(cp_info cp) {
    char *s = malloc(kMaxStrLen);
    int written;
    switch (cp.tag) {
        case CP_METHODREF:
            written = sprintf(s, "cp_methodref { class_index: %d, name_and_type_index: %d }", cp.info.methodref.class_index, cp.info.methodref.name_and_type_index);
            break;
        case CP_CLASS:
            written = sprintf(s, "cp_class { name_index: %d }", cp.info.class.name_index);
            break;
        default:
            fatal("unknown constant pool tag: %d", cp.tag);
            break;
    }
    assert(written < kMaxStrLen);
    return s;
}

static result parse_cp_info(buffer *buf, cp_info *x) {
    uint8_t tag;
    ASSIGN_UINT_OR_RETURN(8, tag, buf);
    switch (tag) {
        case CP_METHODREF:
            ASSIGN_OR_RETURN(cp_methodref, x->info.methodref, buf);
            break;
        case CP_CLASS:
            ASSIGN_OR_RETURN(cp_class, x->info.class, buf);
            break;
        default:
            fatal("unknown constant pool tag: %d", tag);
            break;
    }
    x->tag = tag;
    return RESULT_OK;
}

result parse_class(buffer *buf, class_file *class) {
    ASSIGN_UINT_OR_RETURN(32, class->magic, buf);
    ASSIGN_UINT_OR_RETURN(16, class->minor_version, buf);
    ASSIGN_UINT_OR_RETURN(16, class->major_version, buf);
    ASSIGN_UINT_OR_RETURN(16, class->constant_pool_count, buf);

    class->constant_pool =
        malloc(class->constant_pool_count * sizeof(cp_info));
    int i;
    for (i = 1; i < class->constant_pool_count; i++) {
        ASSIGN_OR_RETURN(cp_info, class->constant_pool[i], buf);
    }

    return RESULT_OK;
}
