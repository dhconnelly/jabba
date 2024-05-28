#include "class.h"

#include <assert.h>
#include <memory.h>
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
        case CP_NAME_AND_TYPE: return "CONSTANT_NameAndType";
        case CP_UTF8: return "CONSTANT_Utf8";
        case CP_FIELDREF: return "CONSTANT_Fieldref";
        case CP_STRING: return "CONSTANT_String";
    }
    assert(0);
}

static result parse_cp_methodref(buffer *buf, cp_methodref *methodref) {
    ASSIGN_UINT_OR_RETURN(16, methodref->class_index, buf);
    ASSIGN_UINT_OR_RETURN(16, methodref->name_and_type_index, buf);
    return RESULT_OK;
}

static result parse_cp_fieldref(buffer *buf, cp_fieldref *fieldref) {
    ASSIGN_UINT_OR_RETURN(16, fieldref->class_index, buf);
    ASSIGN_UINT_OR_RETURN(16, fieldref->name_and_type_index, buf);
    return RESULT_OK;
}

static result parse_cp_class(buffer *buf, cp_class *class) {
    ASSIGN_UINT_OR_RETURN(16, class->name_index, buf);
    return RESULT_OK;
}

static result parse_cp_name_and_type(buffer *buf, cp_name_and_type *nat) {
    ASSIGN_UINT_OR_RETURN(16, nat->name_index, buf);
    ASSIGN_UINT_OR_RETURN(16, nat->descriptor_index, buf);
    return RESULT_OK;
}

static result parse_cp_utf8(buffer *buf, cp_utf8 *utf8) {
    ASSIGN_UINT_OR_RETURN(16, utf8->length, buf);
    utf8->bytes = malloc(utf8->length);
    int i;
    for (i = 0; i < utf8->length; i++) {
        ASSIGN_UINT_OR_RETURN(8, utf8->bytes[i], buf);
    }
    return RESULT_OK;
}

static result parse_cp_string(buffer *buf, cp_string *string) {
    ASSIGN_UINT_OR_RETURN(16, string->string_index, buf);
    return RESULT_OK;
}

/* TODO: fix string leak */
static char *utf8_str(cp_utf8 utf8) {
    char *s = malloc(utf8.length+1);
    memcpy(s, utf8.bytes, utf8.length);
    s[utf8.length] = '\0';
    return s;
}

static const int kMaxStrLen = 512;

/* TODO: fix string leak and make sure it doesn't overflow */
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
        case CP_NAME_AND_TYPE:
            written = sprintf(s, "cp_name_and_type { name_index: %d, descriptor_index: %d }", cp.info.name_and_type.name_index, cp.info.name_and_type.descriptor_index);
            break;
        case CP_UTF8:
            written = sprintf(s, "cp_utf8 { '%s' }", utf8_str(cp.info.utf8));
            break;
        case CP_FIELDREF:
            written = sprintf(s, "cp_fieldref { class_index: %d, name_and_type_index: %d }", cp.info.fieldref.class_index, cp.info.fieldref.name_and_type_index);
            break;
        case CP_STRING:
            written = sprintf(s, "cp_string { string_index: %d }", cp.info.string.string_index);
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
        case CP_NAME_AND_TYPE:
            ASSIGN_OR_RETURN(cp_name_and_type, x->info.name_and_type, buf);
            break;
        case CP_UTF8:
            ASSIGN_OR_RETURN(cp_utf8, x->info.utf8, buf);
            break;
        case CP_FIELDREF:
            ASSIGN_OR_RETURN(cp_fieldref, x->info.fieldref, buf);
            break;
        case CP_STRING:
            ASSIGN_OR_RETURN(cp_string, x->info.string, buf);
            break;
        default:
            fatal("bad cp tag at offset %d: %d", buf->offset, tag);
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

    ASSIGN_UINT_OR_RETURN(16, class->access_flags, buf);
    ASSIGN_UINT_OR_RETURN(16, class->this_class, buf);
    ASSIGN_UINT_OR_RETURN(16, class->super_class, buf);

    ASSIGN_UINT_OR_RETURN(16, class->interfaces_count, buf);

    return RESULT_OK;
}
