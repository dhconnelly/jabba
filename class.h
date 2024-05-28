#ifndef CLASS_H_
#define CLASS_H_

#include <stdio.h>

#include "buffer.h"
#include "result.h"

/*
 * Definitions for working with the Java class file format:
 * https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html
 */

typedef struct cp_info {
    uint8_t tag;
    uint8_t *info;
} cp_info;

typedef struct attribute_info {
    uint16_t attribute_name_index;
    uint32_t attribute_length;
    uint8_t *info;
} attribute_info;

typedef struct field_info {
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t descriptor_index;
    uint16_t attributes_count;
    attribute_info *attributes;
} field_info;

typedef struct method_info {
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t descriptor_index;
    uint16_t attributes_count;
    attribute_info *attributes;
} method_info;

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

#endif
