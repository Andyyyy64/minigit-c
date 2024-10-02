#ifndef TREE_H
#define TREE_H

#include "object.h"

// tree structure mode, sha, pathname
typedef struct {
    GitObject object;
    char *mode;
    char *sha;
    char *path;
} GitTree;

GitTree *tree_read_one(GitRepository *repo, int start);
GitTree *tree_read(GitRepository *repo, const char *sha);
