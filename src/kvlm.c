#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "kvlm.h"

// initialize a new kvlm structure
kvlm *kvlm_init() {
    kvlm *new_kvlm = malloc(sizeof(kvlm));
    if(new_kvlm == NULL) {
        fprintf(stderr, "Error allocating memory for kvlm\n");
        return NULL;
    }
    if(new_kvlm) {
        new_kvlm->head = NULL;
        new_kvlm->message = NULL;
    }
    return new_kvlm;
}

// add a new entry to the kvlm structure
static kvlm_entry *kvlm_entry_create(const char *key, const char *value) {
    // allocate memory for the new entry
    kvlm_entry *entry = malloc(sizeof(kvlm_entry));
    if(entry == NULL) {
        fprintf(stderr, "Error allocating memory for kvlm_entry\n");
        return NULL;
    }
    if(!entry) return NULL;

    // copy the key and value
    entry->key = malloc(strlen(key) + 1);
    entry->value = malloc(strlen(value) + 1);
    if(entry->key == NULL || entry->value == NULL) {
        fprintf(stderr, "Error allocating memory for key or value\n");
        free(entry->key);
        free(entry->value);
        free(entry);
        return NULL;
    }
    memcpy(entry->key, key, strlen(key) + 1);
    memcpy(entry->value, value, strlen(value) + 1);    

    entry->next = NULL;

    return entry;
}

// read a kvlm structure from a raw string
kvlm *kvlm_read(const char *raw, size_t length) {
    kvlm *kvlm_data = kvlm_init();
    if(!kvlm_data) return NULL;

    size_t pos = 0;
    kvlm_entry *last_entry = NULL;

    while(pos < length) {
        // if find the brank line, the rest is the message
        if(raw[pos] == '\n') {
            pos++;
            // copy the message
            kvlm_data->message = malloc(length - pos + 1);
            if(kvlm_data->message == NULL) {
                fprintf(stderr, "Error allocating memory for message\n");
                kvlm_free(kvlm_data);
                return NULL;
            }
            memcpy(kvlm_data->message, raw + pos, length - pos);
            break;
        }

        // read the key

        // find the end of the key
        size_t key_end = pos;
        while(key_end < length && raw[key_end] != ' ') key_end++;
        
        // copy the key
        char *key = malloc(key_end - pos + 1);
        if(key == NULL) {
            fprintf(stderr, "Error allocating memory for key\n");
            kvlm_free(kvlm_data);
            return NULL;
        }
        memcpy(key, raw + pos, key_end - pos);

        // read the value

        // find the end of the value
        pos = key_end + 1;
        size_t value_end = pos;
        while(value_end < length && raw[value_end] != '\n') value_end++;

        // copy the value
        char *value = malloc(value_end - pos + 1);
        if(value == NULL) {
            fprintf(stderr, "Error allocating memory for value\n");
            free(key);
            kvlm_free(kvlm_data);
            return NULL;
        }
        memcpy(value, raw + pos, value_end - pos);

        // create a new entry
        kvlm_entry *entry = kvlm_entry_create(key, value);

        // if the list already has an entry(last_entry != NULL), link the new entry to the *next
        // otherwise, set the new entry as the head of the list
        if(last_entry) {
            last_entry->next = entry;
        } else {
            kvlm_data->head = entry;
        }
        last_entry = entry;

        // free the key and value
        free(key);
        free(value);


        // find if there is a next line
        while(pos < length && raw[pos] == ' ') {
            // find the start of the continuation value
            size_t continuation_end = pos + 1;
            while(continuation_end < length && raw[continuation_end] != '\n') continuation_end++;

            // copy the continuation value
            char *continuation_value = malloc(continuation_end - pos - 1);
            if(continuation_value == NULL) {
                fprintf(stderr, "Error allocating memory for continuation value\n");
                kvlm_free(kvlm_data);
                return NULL;
            }
            memcpy(continuation_value, raw + pos + 1, continuation_end - pos - 1);

            // append the continuation value to the last entry
            char *new_value = malloc(strlen(value) + strlen(continuation_value) + 2);
            sprintf(new_value, "%s\n%s", value, continuation_value);

            // free the old value and the continuation value
            free(entry->value);
            entry->value = new_value;
            free(continuation_value);
             
            // update the position
            pos = continuation_end + 1;
        }
    }

    return kvlm_data;
}

// write a kvlm structure to a raw string
char *kvlm_write(const kvlm *kvlm_data, size_t *length) {
    size_t total_length = 0;
    kvlm_entry *entry = kvlm_data->head;

    // calculate the total length of the key value pair(while will stop when entry is NULL)
    while(entry) {
        total_length += strlen(entry->key) + 1 + strlen(entry->value) + 1;
        entry = entry->next;
    }

    // add the length of the message
    if(kvlm_data->message) {
        total_length += strlen(kvlm_data->message) + 1;
    }

    // allocate memory for the raw string
    char *raw = malloc(total_length);
    // pointer to the current position in the raw string
    char *ptr = raw;

    entry = kvlm_data->head;
    // write the key value pair to the raw string
    while(entry) {
        sprintf(ptr, "%s %s\n", entry->key, entry->value);
        ptr += strlen(entry->key) + 1 + strlen(entry->value) + 1;
        entry = entry->next;
    }
    // write the message to the raw string
    if(kvlm_data->message) {
        sprintf(ptr, "\n%s", kvlm_data->message);
    }

    *length = total_length;
    return raw;
}

// get a value from the kvlm structure based on the key
char *kvlm_get(kvlm *kvlm_data, const char *key) {
    printf("key: %s\n", key);
    kvlm_entry *entry = kvlm_data->head;
    while(entry) {
        if((key == NULL && entry->key == NULL) || (key != NULL && strcmp(key, entry->key) == 0)) {
            return entry->value;
        }
        entry = entry->next;
    }
    return NULL;
}

// free all the memory allocated for the kvlm structure
void kvlm_free(kvlm *kvlm_data) {
    kvlm_entry *entry = kvlm_data->head;
    while(entry) {
        kvlm_entry *next = entry->next;
        free(entry->key);
        free(entry->value);
        free(entry);
        entry = next;
    }
    if(kvlm_data->message) {
        free(kvlm_data->message);
    }
    free(kvlm_data);
}
