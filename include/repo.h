#ifndef REPO_H
#define REPO_H

typedef struct {
    char *worktree; // working directory
    char *gitdir; // git directory
    void *conf; // configuration file(like .git/config)
} GitRepository;

GitRepository *repo_create(const char *path, int force);
int repo_dir(GitRepository *repo, const char *subpath, int makedir);
int repo_file(GitRepository *repo, const char *subpath, const char *content, int makedir);
void repo_free(GitRepository *repo);

#endif