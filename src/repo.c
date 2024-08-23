#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// this is for mkdir(syscall)
#include <sys/stat.h> 
#include <errno.h>
// for access to the file system
#include <unistd.h>

#include "repo.h"

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

// create or get a directory in the repository
int repo_dir(GitRepository *repo, const char *subpath, int makedir) {
    char path[512];
    // make the full path for the directory
    snprintf(path, sizeof(path), "%s/%s", repo->gitdir, subpath);
    if(makedir) {
        if(mkdir(path, 0775) != 0) {
            // if the error is not because the directory already exists
            if(errno != EEXIST) { 
                fprintf(stderr, "Error creating directory %s: %s\n", path, strerror(errno));
                return 0;
            }
        }
    }
    return 1;
}

// create or get a file in the repository, if needed create the directory
int repo_file(GitRepository *repo, const char *subpath, const char *content, int makedir) {
    char path[512];
    snprintf(path, sizeof(path), "%s/%s", repo->gitdir, subpath);

    if(makedir) {
        // copy the path to the directory
        char *dir = malloc(strlen(path) + 1);
        if(dir == NULL) {
            fprintf(stderr, "Error allocating memory for directory\n");
            return 0;
        }
        strcpy(dir, path);
        // get only the directory part of the path
        char *slash = strrchr(dir, '/');
        if(slash != NULL) {
            *slash = '\0';
            if(mkdir(dir, 0755) != 0 && errno != EEXIST) {
                fprintf(stderr, "Error creating directory %s: %s\n", dir, strerror(errno));
                free(dir);
                return 0;
            }
        }
        free(dir);
    }

    if(content != NULL) {
        // open the file in write mode
        FILE *f = fopen(path, "w");
        if(f == NULL) {
            fprintf(stderr, "Error opening file %s: %s\n", path, strerror(errno));
                return 0;
        }
        // write the content to the file
        fprintf(f, "%s", content);
        fclose(f);
    }
    return 1;
}

GitRepository *repo_find(const char *path, int required) {
    // get the full path of the directory
    char full_path[512];
    realpath(path, full_path);

    // make the path for the git directory
    char git_path[512];
    snprintf(git_path, sizeof(git_path), "%s/.minigit-c", full_path);

    struct stat st;
    // stat(git_path, &st) == 0 means the directory exists
    // S_ISDIR(st.st_mode) means it is a directory
    // so basically we are checking if the directory exists and is a directory
    if(stat(git_path, &st) == 0 && S_ISDIR(st.st_mode)) {
        return repo_create(full_path, 0);
    }
    
    // if the directory does not exist, we need to find the parent directory
    char parent[512];
    realpath("..", parent);

    // if the parent is the same as the full path, we are at the root
    if(strcmp(parent, full_path) == 0) {
        if(required) {
            fprintf(stderr, "No git directory\n");
            exit(1);
        } else {
            return NULL;
        }
    }

    // recursively search in parent directories
    return repo_find(parent, required);
}

// free all the memory allocated for the repository
void repo_free(GitRepository *repo) {
    if(repo) {
        free(repo->worktree);
        free(repo->gitdir);
        if(repo->conf) free(repo->conf);
        free(repo);
    }
}
