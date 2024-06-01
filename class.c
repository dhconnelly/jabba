#include "class.h"

#include <assert.h>
#include <memory.h>
#include <stdlib.h>

#include "logging.h"

#define PARSE_OR_RETURN(obj, lvalue, b) \
    do { \
        result r = parse_##obj(b, &lvalue); \
        if (r != RESULT_OK) return r; \
    } while (0)

#define PARSE_OR_RETURN1(obj, lvalue, b, arg) \
    do { \
        result r = parse_##obj(b, &lvalue, arg); \
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
    UINT_OR_RETURN(16, methodref->class_index, buf);
    UINT_OR_RETURN(16, methodref->name_and_type_index, buf);
    return RESULT_OK;
}

static result parse_cp_fieldref(buffer *buf, cp_fieldref *fieldref) {
    UINT_OR_RETURN(16, fieldref->class_index, buf);
    UINT_OR_RETURN(16, fieldref->name_and_type_index, buf);
    return RESULT_OK;
}

static result parse_cp_class(buffer *buf, cp_class *class) {
    UINT_OR_RETURN(16, class->name_index, buf);
    return RESULT_OK;
}

static result parse_cp_name_and_type(buffer *buf, cp_name_and_type *nat) {
    UINT_OR_RETURN(16, nat->name_index, buf);
    UINT_OR_RETURN(16, nat->descriptor_index, buf);
    return RESULT_OK;
}

static result parse_cp_utf8(buffer *buf, cp_utf8 *utf8) {
    uint16_t len;
    UINT_OR_RETURN(16, len, buf);
    utf8->s = malloc(len+1);
    uint8_t i, c;
    for (i = 0; i < len; i++) {
        UINT_OR_RETURN(8, c, buf);
        utf8->s[i] = c;
    }
    utf8->s[len] = '\0';
    return RESULT_OK;
}

static result parse_cp_string(buffer *buf, cp_string *string) {
    UINT_OR_RETURN(16, string->string_index, buf);
    return RESULT_OK;
}

static result parse_source_file_attr(
        buffer *buf,
        source_file_attr *source_file) {
    UINT_OR_RETURN(16, source_file->source_file_idx, buf);
    return RESULT_OK;
}

static result parse_exception_info(buffer *buf, exception_info *exception) {
    UINT_OR_RETURN(16, exception->start_pc, buf);
    UINT_OR_RETURN(16, exception->end_pc, buf);
    UINT_OR_RETURN(16, exception->handler_pc, buf);
    UINT_OR_RETURN(16, exception->catch_type, buf);
    return RESULT_OK;
}

static result parse_attribute_info(
        buffer *buf,
        attribute_info *attribute,
        cp_info *cp);

static result parse_code_attr(buffer *buf, code_attr *code, cp_info *cp) {
    UINT_OR_RETURN(16, code->max_stack, buf);
    UINT_OR_RETURN(16, code->max_locals, buf);
    UINT_OR_RETURN(32, code->code_length, buf);
    int i;
    code->code = malloc(code->code_length);
    for (i = 0; i < code->code_length; i++) {
        UINT_OR_RETURN(8, code->code[i], buf);
    }
    UINT_OR_RETURN(16, code->exceptions_length, buf);
    code->exceptions = malloc(sizeof(exception_info) * code->exceptions_length);
    for (i = 0; i < code->exceptions_length; i++) {
        PARSE_OR_RETURN(exception_info, code->exceptions[i], buf);
    }
    UINT_OR_RETURN(16, code->attributes_count, buf);
    code->attributes = malloc(sizeof(attribute_info) * code->attributes_count);
    for (i = 0; i < code->attributes_count; i++) {
        PARSE_OR_RETURN1(attribute_info, code->attributes[i], buf, cp);
    }
    return RESULT_OK;
}

static result parse_line_number_table(
        buffer *buf,
        line_number_table_attr *lines) {
    UINT_OR_RETURN(16, lines->length, buf);
    lines->lines = malloc(sizeof(line_number) * lines->length);
    int i;
    for (i = 0; i < lines->length; i++) {
        UINT_OR_RETURN(16, lines->lines[i].start_pc, buf);
        UINT_OR_RETURN(16, lines->lines[i].line, buf);
    }
    return RESULT_OK;
}

static result parse_attribute_info(
        buffer *buf,
        attribute_info *attribute,
        cp_info *cp) {
    uint16_t attr_idx;
    UINT_OR_RETURN(16, attr_idx, buf);
    uint32_t attr_len;
    UINT_OR_RETURN(32, attr_len, buf);

    const char *type = cp[attr_idx].info.utf8.s;
    if (strcmp(type, "Code") == 0) {
        return parse_code_attr(buf, &attribute->info.code, cp);
    } else if (strcmp(type, "SourceFile") == 0) {
        return parse_source_file_attr(buf, &attribute->info.source_file);
    } else if (strcmp(type, "LineNumberTable") == 0) {
        return parse_line_number_table(buf, &attribute->info.line_numbers);
    } else {
        fatal("unsupported attribute type: %s", type);
    }

    return RESULT_OK;
}

static result parse_field_info(buffer *buf, field_info *field, cp_info *cp) {
    UINT_OR_RETURN(16, field->access_flags, buf);
    UINT_OR_RETURN(16, field->name_index, buf);
    UINT_OR_RETURN(16, field->descriptor_index, buf);
    UINT_OR_RETURN(16, field->attributes_count, buf);
    field->attributes = malloc(field->attributes_count * sizeof(attribute_info));
    int i;
    for (i = 0; i < field->attributes_count; i++) {
        PARSE_OR_RETURN1(attribute_info, field->attributes[i], buf, cp);
    }
    return RESULT_OK;
}

static result parse_method_info(buffer *buf, method_info *method, cp_info *cp) {
    UINT_OR_RETURN(16, method->access_flags, buf);
    UINT_OR_RETURN(16, method->name_index, buf);
    UINT_OR_RETURN(16, method->descriptor_index, buf);
    UINT_OR_RETURN(16, method->attributes_count, buf);
    method->attributes = malloc(method->attributes_count * sizeof(attribute_info));
    int i;
    for (i = 0; i < method->attributes_count; i++) {
        PARSE_OR_RETURN1(attribute_info, method->attributes[i], buf, cp);
    }
    return RESULT_OK;
}

static int line_number_table_str(
        line_number_table_attr *lines,
        char s[],
        int max_len) {
    return sprintf(s, "<LINES>");
}

static int source_file_str(
        source_file_attr *source_file,
        char s[],
        int max_len) {
    return sprintf(s, "<SOURCE>");
}

static int code_str(
        code_attr *code,
        char s[],
        int max_len) {
    return sprintf(s, "<CODE>");
}

static const char *attr_type(attribute_type type) {
    switch (type) {
        case ATTR_CODE: return "Code";
        case ATTR_SOURCE_FILE: return "SourceFile";
        case ATTR_LINE_NUMBERS: return "LineNumberTable";
    }
    assert(0);
}

int attribute_info_str(attribute_info *attr, char s[], int max_len) {
    printf("type! %s\n", attr_type(attr->type));
    switch (attr->type) {
        case ATTR_CODE: return code_str(&attr->info.code, s, max_len);
        case ATTR_SOURCE_FILE: return source_file_str(&attr->info.source_file, s, max_len);
        case ATTR_LINE_NUMBERS: return line_number_table_str(&attr->info.line_numbers, s, max_len);
    }
    fatal("unsupported attribute type: %s", attr_type(attr->type));
    return 0;
}

int cp_info_str(cp_info cp, char s[], int max_len) {
    int written = 0;
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

        case CP_UTF8: {
            written = sprintf(s, "cp_utf8 { '%s' }", cp.info.utf8.s);
            break;
        }

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
    assert(written < max_len);
    return written;
}

static result parse_cp_info(buffer *buf, cp_info *x) {
    uint8_t tag;
    UINT_OR_RETURN(8, tag, buf);
    switch (tag) {
        case CP_METHODREF:
            PARSE_OR_RETURN(cp_methodref, x->info.methodref, buf);
            break;
        case CP_CLASS:
            PARSE_OR_RETURN(cp_class, x->info.class, buf);
            break;
        case CP_NAME_AND_TYPE:
            PARSE_OR_RETURN(cp_name_and_type, x->info.name_and_type, buf);
            break;
        case CP_UTF8:
            PARSE_OR_RETURN(cp_utf8, x->info.utf8, buf);
            break;
        case CP_FIELDREF:
            PARSE_OR_RETURN(cp_fieldref, x->info.fieldref, buf);
            break;
        case CP_STRING:
            PARSE_OR_RETURN(cp_string, x->info.string, buf);
            break;
        default:
            fatal("bad cp tag at offset %d: %d", buf->offset, tag);
            break;
    }
    x->tag = tag;
    return RESULT_OK;
}

result parse_class(buffer *buf, class_file *class) {
    UINT_OR_RETURN(32, class->magic, buf);
    UINT_OR_RETURN(16, class->minor_version, buf);
    UINT_OR_RETURN(16, class->major_version, buf);
    UINT_OR_RETURN(16, class->constant_pool_count, buf);

    class->constant_pool =
        malloc(class->constant_pool_count * sizeof(cp_info));
    int i;
    for (i = 1; i < class->constant_pool_count; i++) {
        PARSE_OR_RETURN(cp_info, class->constant_pool[i], buf);
    }

    UINT_OR_RETURN(16, class->access_flags, buf);
    UINT_OR_RETURN(16, class->this_class, buf);
    UINT_OR_RETURN(16, class->super_class, buf);

    UINT_OR_RETURN(16, class->interfaces_count, buf);
    class->interfaces = malloc(class->interfaces_count * sizeof(uint16_t));
    for (i = 0; i < class->interfaces_count; i++) {
        UINT_OR_RETURN(16, class->interfaces[i], buf);
    }

    UINT_OR_RETURN(16, class->fields_count, buf);
    class->fields = malloc(class->fields_count * sizeof(field_info));
    for (i = 0; i < class->fields_count; i++) {
        PARSE_OR_RETURN1(
                field_info,
                class->fields[i],
                buf,
                class->constant_pool);
    }

    UINT_OR_RETURN(16, class->methods_count, buf);
    class->methods = malloc(class->methods_count * sizeof(method_info));
    for (i = 0; i < class->methods_count; i++) {
        PARSE_OR_RETURN1(
                method_info,
                class->methods[i],
                buf,
                class->constant_pool);
    }

    UINT_OR_RETURN(16, class->attributes_count, buf);
    class->attributes = malloc(class->attributes_count * sizeof(attribute_info));
    for (i = 0; i < class->attributes_count; i++) {
        PARSE_OR_RETURN1(
                attribute_info,
                class->attributes[i],
                buf,
                class->constant_pool);
    }

    return RESULT_OK;
}
