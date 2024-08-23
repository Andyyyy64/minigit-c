#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// this is for mkdir(syscall)
#include <sys/stat.h> 
#include <errno.h>
// for access to the file system
#include <unistd.h>

#include "repo.h"
#include "command.h"

// create a new minigit-c repository
GitRepository *repo_create(const char *path, int force) {
    // allocate memory for the repository
    GitRepository *repo = malloc(sizeof(GitRepository)); 
    if(repo == NULL) {
        fprintf(stderr, "Error allocating memory for GitRepository\n");
        return NULL;
    }

    // copy the path to the worktree
    repo->worktree = malloc(strlen(path) + 1);
    if(repo->worktree == NULL) {
        fprintf(stderr, "Error allocating memory for directory\n");
        return 0;
    }
    strcpy(repo->worktree, path);
    
    // allocate memory for the gitdir path
    repo->gitdir = malloc(strlen(path) + strlen("./minigit-c") + 1);

    if(repo->gitdir == NULL) {
        fprintf(stderr, "Error allocating memory for gitdir\n");
        free(repo->worktree);
        free(repo);
        return NULL;
    }

    // make the full path for the minigit-c directory
    sprintf(repo->gitdir, "%s/.minigit-c", path);

    // check if the directory already exists
    if(!force && !repo_dir(repo, "", 1)) {
        fprintf(stderr, "Not a git repository %s\n", repo->gitdir);
        repo_free(repo);
        return NULL;
    }

    // create the directories
    if(!repo_dir(repo, "branches", 1)) return NULL;
    if(!repo_dir(repo, "objects", 1)) return NULL;
    if(!repo_dir(repo, "refs", 1)) return NULL;
    if(!repo_dir(repo, "refs/tags", 1)) return NULL;
    if(!repo_dir(repo, "refs/heads", 1)) return NULL;

    // create the files
    if(!repo_file(repo, "description", "Unnamed repository; edit this file 'description' to name the repository.\n", 1)) {
        return NULL;
    }

    if(!repo_file(repo, "HEAD", "ref: refs/heads/master\n", 1)) {
        return NULL;
    }

    if(!repo_file(repo, "config", "[core]\n\trepositoryformatversion = 0\n\tfilemode = true\n\tbare = false\n\tlogallrefupdates = true\n", 1)) {
        return NULL;
    }

    return repo;
}

void cat_file(GitRepository *repo, GitObject *object, const char *fmt) {
    GitObject *obj = malloc(sizeof(GitObject));
    obj = object_read(repo, object_find(repo, object, fmt, 1));
    if(obj == NULL) {
        fprintf(stderr, "Error reading object\n");
        return;
    }

    // compare if is the same type
    if(strcmp(obj->fmt, fmt) != 0) {
        fprintf(stderr, "object type mismatch, exprected %s, got %s\n", fmt, obj->fmt);
        object_free(obj);
    }

    // output the object data to standard output
    fwrite(obj->data, 1, obj->size, stdout);
    object_free(obj);
}