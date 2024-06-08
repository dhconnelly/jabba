#include "hashtable.h"

#include <stdlib.h>
#include <string.h>

#define NUM_ENTRIES 101

static unsigned hash(char *s) {
    unsigned hashval;
    for (hashval = 0; *s != '\0'; s++) {
        hashval = *s + 31 * hashval;
    }
    return hashval % NUM_ENTRIES;
}

static char *strdup(char *s) {
    char *t = malloc(strlen(s));
    strcpy(t, s);
    return t;
}

#define DEFINE_HASHTABLE(name, type, handle_existing) \
    void init_table_##name(table_##name *tab) { \
        int i; \
        tab->free = free; \
        for (i = 0; i < NUM_ENTRIES; i++) tab->entries[i] = NULL; \
    } \
    \
    entry_##name *find_entry_##name(table_##name *tab, char *s) { \
        entry_##name *e; \
        for (e = tab->entries[hash(s)]; e != NULL; e = e->next) { \
            if (strcmp(s, e->key) == 0) return e; \
        } \
        return NULL; \
    } \
    \
    type *lookup_##name(table_##name *tab, char *s) { \
        entry_##name *e = find_entry_##name(tab, s); \
        return (e == NULL) ? NULL : &e->value; \
    } \
    static void noop(type) {} \
    \
    void install_##name(table_##name *tab, char *s, type value) { \
        entry_##name *e; \
        unsigned hashval; \
        if ((e = find_entry_##name(tab, s)) == NULL) { \
            /* not found */ \
            e = malloc(sizeof(entry_##name)); \
            e->key = strdup(s); \
            hashval = hash(s); \
            e->next = tab->entries[hashval]; \
            tab->entries[hashval] = e; \
        } else { \
            /* already present */ \
            handle_existing(e->value); \
        } \
        e->value = value; \
    } \

DEFINE_HASHTABLE(int, int, noop);
