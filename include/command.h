#ifndef COMMAND_H
#define COMMAND_H

#include "repo.h"
#include "object.h"
#include "blob.h"

GitRepository *repo_create(const char *path, int force);
void cat_file(GitRepository *repo, GitObject *object, const char *fmt);


#endif
