#ifndef HASHTABLE_H_
#define HASHTABLE_H_

/* 
 * a hashtable...
 *
 * k&r section 6.6
 *
 * usage:
 *     DECLARE_HASHTABLE(int); // in header file
 *     DEFINE_HASHTABLE(int);  // in impl file
 *
 *     table_int tab;
 *     int *x = lookup_int("foo");
 */

#define DECLARE_HASHTABLE(name, type) \
    typedef struct entry_##name { \
        struct entry_##name *next; \
        char *key; \
        type value; \
    } entry_##name; \
    \
    typedef struct { \
        void (*free)(void *); \
        entry_##name **entries; \
    } table_##name; \
    void init_table_##name(table_##name *tab); \
    type *lookup_##name(table_##name *tab, char *s); \
    void install_##name(table_##name *tab, char *s, type value); \

/* TODO: free table */

DECLARE_HASHTABLE(str, char *);
DECLARE_HASHTABLE(int, int);

#endif
