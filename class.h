#ifndef CLASS_H_
#define CLASS_H_

/*
 * Definitions for working with the Java class file format:
 * https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html
 */

typedef struct ClassFile {
    uint32_t magic;
    uint16_t minor_version;
    uint16_t major_version;
} ClassFile;

#endif
