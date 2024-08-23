#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "blob.h"

GitBlob *blob_create(const char *data, size_t size) {
    GitBlob *blob = malloc(sizeof(GitBlob));
    if(blob == NULL) {
        fprintf(stderr, "Error allocating memory for GitBlob\n");
        return NULL;
    }

    blob->object.data = malloc(size);
    if(blob->object.data == NULL) {
        fprintf(stderr, "Error allocating memory for data\n");
        free(blob);
        return NULL;
    }

    memcpy(blob->object.data, data, size);
    blob->object.size = size;

    blob->object.fmt = "blob";

    return blob;
}

void blob_free(GitBlob *blob) {
    if(blob != NULL) {
        free(blob->object.data);
        free(blob->object.fmt);
        free(blob);
    }
}
