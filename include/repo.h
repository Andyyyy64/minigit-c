#ifndef REPO_H
#define REPO_H

typedef struct {
    char *worktree; // working directory
    char *gitdir; // git directory
    void *conf; // configuration file(like .git/config)
} GitRepository;

int repo_dir(GitRepository *repo, const char *subpath, int makedir);
int repo_file(GitRepository *repo, const char *subpath, const char *content, int makedir);
GitRepository *repo_find(const char *path, int required);
void repo_free(GitRepository *repo);

#endif