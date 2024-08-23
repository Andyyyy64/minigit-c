#ifndef BLOB_H
#define BLOB_H

#include "object.h"

// blob object(binary large object)
typedef struct {
    GitObject object;
} GitBlob;

// for blob
GitBlob *blob_create(const char *data, size_t size);
void blob_free(GitBlob *blob);

#endif