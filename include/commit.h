#ifndef COMMIT_H
#define COMMIT_H

#include "object.h"
#include "kvlm.h"

// git commit structure
typedef struct {
    GitObject object;
    kvlm *kvlm_data; // parsed commit data
} GitCommit;

GitCommit *commit_read(GitRepository *repo, const char *sha);
void commit_free(GitCommit *commit);

#endif