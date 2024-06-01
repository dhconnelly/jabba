#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buffer.h"
#include "class.h"
#include "logging.h"
#include "result.h"

static const int BUF_LEN = 512;

static void print_method(method_info *method, cp_info *cp) {
    printf("method: %s\n", cp[method->name_index].info.utf8.s);
    printf("access flags: 0x%X\n", method->access_flags);
    printf("method descriptor: %s\n", cp[method->descriptor_index].info.utf8.s);
    int i;
    char s[BUF_LEN];
    for (i = 0; i < method->attributes_count; i++) {
        assert(attribute_info_str(&method->attributes[i], s, BUF_LEN) < BUF_LEN);
        printf("method attr[%d] = %s\n", i, s);
    }
}

static void print_field(field_info *field, cp_info *cp) {
    printf("field: %s\n", cp[field->name_index].info.utf8.s);
    printf("access flags: 0x%X\n", field->access_flags);
    printf("field descriptor: %s\n", cp[field->descriptor_index].info.utf8.s);
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
    return 0;
}
