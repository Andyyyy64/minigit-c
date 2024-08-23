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


#endif




