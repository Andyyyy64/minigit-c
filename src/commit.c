#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commit.h"

GitCommit *commit_read(GitRepository *repo, const char *sha) {
    // get repo's object from sha
    GitObject *obj = object_read(repo, sha);
    if(obj == NULL) {
        fprintf(stderr, "Error reading object %s\n", sha);
        return NULL;
    }
    printf("Object %s read\n", sha);
    // check if object type is 'commit'
    if(strcmp(obj->fmt, "commit") != 0) {
        fprintf(stderr, "Error object %s is not a commit\n", sha);
        object_free(obj);
        return NULL;
    }
    printf("Object %s is a commit\n", sha);
    // create commit object
    GitCommit *commit = malloc(sizeof(GitCommit));
    if(commit == NULL) {
        fprintf(stderr, "Error allocating memory for GitCommit\n");
        object_free(obj);
        return NULL;
    }

    // copy the object and kvlm_data to new commit object
    commit->object = *obj;
    commit->kvlm_data = kvlm_read(obj->data, obj->size);

    return commit;
}

void commit_free(GitCommit *commit) {
    if(commit) {
        kvlm_free(commit->kvlm_data);
        object_free(&commit->object);
        free(commit);
    }
}