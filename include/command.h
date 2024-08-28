#ifndef COMMAND_H
#define COMMAND_H

#include "repo.h"
#include "object.h"
#include "blob.h"

// for command
GitRepository *repo_create(const char *path, int force);
void cat_file(GitRepository *repo, GitObject *object, const char *fmt);
void cmd_hash_object(const char *path, const char *fmt, int write);
void cmd_log(GitRepository *repo, const char *sha);
#endif
