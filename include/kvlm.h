#ifndef KVLM_H
#define KVLM_H

#include <stdio.h>

typedef struct kvlm_entry {
    char *key;
    char *value;
    struct kvlm_entry *next;
} kvlm_entry;

typedef struct {
    kvlm_entry *head;
    char *message;
} kvlm;

kvlm *kvlm_init();

kvlm *kvlm_read(const char *raw, size_t length);

char *kvlm_write(const kvlm *kvlm_data, size_t *length);

void kvlm_free(kvlm *kvlm_data);

#endif
