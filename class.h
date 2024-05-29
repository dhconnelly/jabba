#ifndef CLASS_H_
#define CLASS_H_

#include <stdio.h>

#include "buffer.h"
#include "result.h"

/*
 * Definitions for working with the Java class file format:
 * https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html
 */

typedef struct cp_methodref {
    uint16_t class_index;
    uint16_t name_and_type_index;
} cp_methodref;

typedef struct cp_fieldref {
    uint16_t class_index;
    uint16_t name_and_type_index;
} cp_fieldref;

typedef struct cp_class {
    uint16_t name_index;
} cp_class;

typedef struct cp_name_and_type {
    uint16_t name_index;
    uint16_t descriptor_index;
} cp_name_and_type;

typedef struct cp_utf8 {
    uint16_t length;
    uint8_t *bytes;
} cp_utf8;

typedef struct cp_string {
    uint16_t string_index;
} cp_string;

typedef enum cp_tag {
    CP_CLASS = 7,
    CP_METHODREF = 10,
    CP_NAME_AND_TYPE = 12,
    CP_UTF8 = 1,
    CP_FIELDREF = 9,
    CP_STRING = 8,
} cp_tag;

const char *cp_tag_str(cp_tag tag);

typedef struct cp_info {
    cp_tag tag;
    union {
        cp_methodref methodref;
        cp_class class;
        cp_name_and_type name_and_type;
        cp_utf8 utf8;
        cp_fieldref fieldref;
        cp_string string;
    } info;
} cp_info;

int cp_info_str(cp_info info, char s[], int max_len);

typedef struct attribute_info {
    uint16_t attribute_name_index;
    uint32_t attribute_length;

    /* TODO: parse all attribute types */
    uint8_t *info;
} attribute_info;

int attribute_info_str(attribute_info field, char s[], int max_len);

typedef struct field_info {
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t descriptor_index;
    uint16_t attributes_count;
    attribute_info *attributes;
} field_info;

int field_info_str(field_info field, char s[], int max_len);

typedef struct method_info {
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t descriptor_index;
    uint16_t attributes_count;
    attribute_info *attributes;
} method_info;

int method_info_str(method_info method, char s[], int max_len);

typedef struct class_file {
    uint32_t magic;
    uint16_t minor_version;
    uint16_t major_version;
    uint16_t constant_pool_count;
    cp_info *constant_pool;
    uint16_t access_flags;
    uint16_t this_class;
    uint16_t super_class;
    uint16_t interfaces_count;
    uint16_t *interfaces;
    uint16_t fields_count;
    field_info *fields;
    uint16_t methods_count;
    method_info *methods;
    uint16_t attributes_count;
    attribute_info *attributes;
} class_file;

result parse_class(buffer *buf, class_file *class);

/* TODO: free */

#endif
