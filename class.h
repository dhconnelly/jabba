#ifndef CLASS_H_
#define CLASS_H_

#include <stdio.h>

#include "buffer.h"
#include "result.h"

/*
 * Definitions for working with the Java class file format:
 * https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html
 */

typedef struct {
    uint16_t class_index;
    uint16_t name_and_type_index;
} cp_methodref;

typedef struct {
    uint16_t class_index;
    uint16_t name_and_type_index;
} cp_fieldref;

typedef struct {
    uint16_t name_index;
} cp_class;

typedef struct {
    uint16_t name_index;
    uint16_t descriptor_index;
} cp_name_and_type;

typedef struct {
    uint16_t string_index;
} cp_string;

typedef enum {
    CP_CLASS = 7,
    CP_METHODREF = 10,
    CP_NAME_AND_TYPE = 12,
    CP_UTF8 = 1,
    CP_FIELDREF = 9,
    CP_STRING = 8,
} cp_tag;

typedef struct {
    cp_tag tag;
    union {
        cp_methodref methodref;
        cp_class class;
        cp_name_and_type name_and_type;
        char *utf8;
        cp_fieldref fieldref;
        cp_string string;
    } info;
} cp_info;

typedef enum {
    ATTR_CODE,
    ATTR_SOURCE_FILE,
    ATTR_LINE_NUMBERS,
} attribute_type;

typedef struct {
    uint16_t start_pc, end_pc, handler_pc, catch_type;
} exception_info;

/* forward declaration since this is a recursive structure */
struct attribute_info;

typedef struct {
    uint16_t max_stack;
    uint16_t max_locals;
    uint32_t code_length;
    uint8_t *code;
    uint16_t exceptions_length;
    exception_info *exceptions;
    uint16_t attributes_count;
    struct attribute_info *attributes;
} code_attr;

typedef struct {
    uint16_t name_idx;
    uint16_t source_file_idx;
} source_file_attr;

typedef struct {
    uint16_t start_pc;
    uint16_t line;
} line_number;

typedef struct {
    uint16_t length;
    line_number *lines;
} line_number_table_attr;

typedef struct attribute_info {
    attribute_type type;
    union {
        code_attr code;
        source_file_attr source_file;
        line_number_table_attr line_numbers;
    } info;
} attribute_info;

typedef struct {
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t descriptor_index;
    uint16_t attributes_count;
    attribute_info *attributes;
} field_info;

typedef struct {
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t descriptor_index;
    uint16_t attributes_count;
    attribute_info *attributes;
} method_info;

typedef struct {
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

/* TODO: free */

#endif
