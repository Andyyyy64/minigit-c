#ifndef COMMAND_H
#define COMMAND_H

#include "repo.h"
#include "object.h"
#include "blob.h"

GitRepository *repo_create(const char *path, int force);
void cat_file(GitRepository *repo, GitObject *object, const char *fmt);
void cmd_hash_object(const char *path, const char *fmt, int write);

#endif
