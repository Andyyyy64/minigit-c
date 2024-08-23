#include <stdio.h>
#include <stdlib.h>

#include "command.h"

int main(int argc, char *argv[]) {
    if (argc < 2) { // if no command is given, print usage
        fprintf(stderr, "Usage %s <command> [options]\n", argv[0]);
        return 1;
    }

    if(strcmp(argv[1], "add") == 0) {
        // add command
        printf("Add command\n");
    } else if(strcmp(argv[1], "cat-file") == 0) {
        // cat-file command
        if(argc < 4) {
            fprintf(stderr, "Usage: %s cat-file <type> <object>\n", argv[0]);
            return 1;
        }
        const char *type = argv[2];
        if(strcmp(type, "blob") != 0 && strcmp(type, "commit") != 0 && strcmp(type, "tag") != 0 && strcmp(type, "tree") != 0) {
            fprintf(stderr, "Unknown type %s\n", type);
            return 1;
        }
        const char *object = argv[3];

        GitRepository *repo = repo_find(".", 1);
        if(repo == NULL) {
            fprintf(stderr, "Not a git repository found\n");
            return 1;
        }

        cat_file(repo, object, type);
        repo_free(repo);

    } else if(strcmp(argv[1], "check-ignore") == 0) {
        // check-ignore command
    } else if(strcmp(argv[1], "checkout") == 0) {
        // checkout command
    } else if(strcmp(argv[1], "commit") == 0) {
        // commit command
    } else if(strcmp(argv[1], "hash-object") == 0) {
        // hash-object command
    } else if(strcmp(argv[1], "init") == 0)  {
        // repo init command
        const char *path = ".";
        if(argc > 2) {
            path = argv[2];
        }
        if(repo_create(path, 0) == NULL) {
            fprintf(stderr, "Failed to initialize repository\n");
            return 1;
        } else {
            printf("Initialized empty minigit-c repository in %s\n", path);
        }
    } else if(strcmp(argv[1], "log") == 0) {
        // log command
    } else if(strcmp(argv[1], "ls-files") == 0) {
        // ls-files command
    } else if(strcmp(argv[1], "ls-tree") == 0) {
        // ls-tree command
    } else if(strcmp(argv[1], "rev-parse") == 0) {
        // rev-parse command
    } else if(strcmp(argv[1], "rm") == 0) {
        // rm command
    } else if(strcmp(argv[1], "show-ref") == 0) {
        // show-ref command
    } else if(strcmp(argv[1], "status") == 0) {
        // status command
    } else if(strcmp(argv[1], "tag") == 0) {
        // tag command
    } else {
        fprintf(stderr, "Unknown command %s\n", argv[1]);
        return 1;
    }

    return 0;
}