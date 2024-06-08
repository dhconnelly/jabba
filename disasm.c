#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buffer.h"
#include "class.h"
#include "logging.h"
#include "parse.h"
#include "result.h"
#include "hashtable.h"

static const int BUF_LEN = 512;

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

const char *attr_type(attribute_type type) {
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
            written = sprintf(s, "cp_utf8 { '%s' }", cp.info.utf8);
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


static void print_method(method_info *method, cp_info *cp) {
    printf("method: %s\n", cp[method->name_index].info.utf8);
    printf("access flags: 0x%X\n", method->access_flags);
    printf("method descriptor: %s\n", cp[method->descriptor_index].info.utf8);
    int i;
    char s[BUF_LEN];
    for (i = 0; i < method->attributes_count; i++) {
        assert(attribute_info_str(&method->attributes[i], s, BUF_LEN) < BUF_LEN);
        printf("method attr[%d] = %s\n", i, s);
    }
}

static void print_field(field_info *field, cp_info *cp) {
    printf("field: %s\n", cp[field->name_index].info.utf8);
    printf("access flags: 0x%X\n", field->access_flags);
    printf("field descriptor: %s\n", cp[field->descriptor_index].info.utf8);
    int i;
    char s[BUF_LEN];
    for (i = 0; i < field->attributes_count; i++) {
        assert(attribute_info_str(&field->attributes[i], s, BUF_LEN) < BUF_LEN);
        printf("field attr[%d] = %s\n", i, s);
    }
}

static void print_class(class_file *class) {
    int i;
    char s[BUF_LEN];
    printf("magic: 0x%X\n", class->magic);
    printf("minor_version: %d\n", class->minor_version);
    printf("major version: %d\n", class->major_version);
    printf("constant pool count %d\n", class->constant_pool_count);
    for (i = 1; i < class->constant_pool_count; i++) {
        cp_info_str(class->constant_pool[i], s, BUF_LEN);
        printf("cp_info[%d] = %s\n", i, s);
    }
    printf("access flags: 0x%X\n", class->access_flags);
    printf("this class: %d\n", class->this_class);
    printf("super class: %d\n", class->super_class);
    for (i = 0; i < class->interfaces_count; i++) {
        printf("interfaces[%d] = %d\n", i, class->interfaces[i]);
    }
    for (i = 0; i < class->fields_count; i++) {
        print_field(&class->fields[i], class->constant_pool);
    }
    for (i = 0; i < class->methods_count; i++) {
        print_method(&class->methods[i], class->constant_pool);
    }
    for (i = 0; i < class->attributes_count; i++) {
        attribute_info_str(&class->attributes[i], s, BUF_LEN);
        printf("attributes[%d] = %s\n", i, s);
    }
}

static void disasm(FILE *f) {
    trace("disassembling...");
    buffer buf;
    buffer_init(&buf, f);
    class_file class;
    result r = parse_class(&buf, &class);
    if (r != RESULT_OK) {
        fatal("failed to read class at offset %d: %s",
                buf.offset, result_str(r));
    }
    print_class(&class);
}

int main(int argc, char *argv[]) {
    if (argc != 2) die("usage: disasm <class-file>");
    FILE *f = fopen(argv[1], "r");
    if (f == NULL) fatal("failed to open %s: %s", argv[1], strerror(errno));
    disasm(f);

    table_int tab;
    init_table_int(&tab);
    int *foo = lookup_int(&tab, "foo");
    assert(foo == NULL);

    init_table_int(&tab);
    install_int(&tab, "foo", 34);
    install_int(&tab, "bar", 17);
    install_int(&tab, "baz", -1);
    char *keys[] = {"foo", "bar", "baz"};
    int i;
    for (i = 0; i < 3; i++) {
        printf("tab['%s'] == %d\n", keys[i], *lookup_int(&tab, keys[i]));
        install_int(&tab, keys[i], 0);
        printf("tab['%s'] == %d\n", keys[i], *lookup_int(&tab, keys[i]));
    }

    return 0;
}
