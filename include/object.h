#ifndef OBJECT_H
#define OBJECT_H

#include "repo.h"

// base structure for git objects
typedef struct {
    char *data;
    size_t size;
    char *fmt;
} GitObject;

// for git object
GitObject *object_read(GitRepository *repo, const char *sha);
char *object_write(GitObject *object, GitRepository *repo);
char *object_hash(FILE *file, const char *fmt, GitRepository *repo);
char *object_find(GitRepository *repo, const char *name, const char *fmt, int follow);
void object_free(GitObject *object);

#endif
