#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include <openssl/sha.h>
#include <sys/stat.h>
#include <errno.h>

#include "object.h"

GitObject *object_read(GitRepository *repo, const char *sha) {
    char path[512];
    snprintf(path, sizeof(path), "%s/objects/%.2s/%s", repo->gitdir, sha, sha + 2);
    // open the file in read mode
    FILE *f = fopen(path, "rb");
    if(f == NULL) {
        fprintf(stderr, "Error opening file %s: %s\n", path, strerror(errno));
        return NULL;
    }

    // decompress the file
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    // allocate memory for the compressed data
    char *compressed = malloc(fsize);
    // read the compressed data from the file
    fread(compressed, 1, fsize, f);

    // we are done with the file
    fclose(f);

    // allocate memory for the raw data(decompressed content)
    unsigned char raw[1024];
    uLongf decompressed_size = sizeof(raw);

    // decompress the data
    if(uncompress(raw, decompressed_size, (const Bytef *)compressed, fsize) != Z_OK) {
        fprintf(stderr, "Error decompressing file %s\n", path);
        free(compressed);
        return NULL;
    }
    free(compressed);
    
    // read header

    // find the space separator
    char *space = strchr(raw, ' ');
    if(!space) {
        fprintf(stderr, "Malformed object: missing space separator\n");        
        return NULL;
    }
    // find the null byte
    char *nullbyte = strchr(space + 1, '\0');
    if(!nullbyte) {
        fprintf(stderr, "Malformed object: missing null byte\n");
        return NULL;
    }
    // find the format(commit, tree, blob, tag)
    size_t fmt_len = space - (char *)raw;
    char *fmt = malloc(fmt_len + 1);
    if(fmt == NULL) {
        fprintf(stderr, "Error allocating memory for fmt\n");
        free(fmt);
        return NULL;
    }
    strncpy(fmt, (char *)raw, fmt_len);
    fmt[fmt_len] = '\0';
    
    // get the size of the object
    size_t size_len = nullbyte - (space + 1);
    char size_str[size_len + 1];
    strncpy(size_str, space + 1, size_len);
    size_str[size_len] = '\0';

    size_t size = atol(size_str);

    size_t data_len = decompressed_size - (nullbyte - (char *)raw + 1);
    if(size != data_len) {
        fprintf(stderr, "Malformed object: incorrect data length\n");
        free(fmt);
        return NULL;
    }

    GitObject *obj = malloc(sizeof(GitObject));
    if(obj == NULL) {
        fprintf(stderr, "Error allocating memory for GitObject\n");
        free(fmt);
        return NULL;
    }

    obj->data = malloc(size);
    if(obj->data == NULL) {
        fprintf(stderr, "Error allocating memory for data\n");
        free(fmt);
        free(obj);
        return NULL;
    }

    memcpy(obj->data, nullbyte + 1, size);

    obj->size = size;
    obj->fmt = fmt;

    if(strcmp(fmt, "commit") == 0) {

    } else if(strcmp(fmt, "tree") == 0) {

    } else if(strcmp(fmt, "tag") == 0) {
        
    } else if(strcmp(fmt, "blob") == 0) {

    } else {
        fprintf(stderr, "Unknown object type %s\n", fmt);
        free(fmt);
        free(obj->data);
        free(obj);
        return NULL;
    }

    return obj;
}

char *object_write(GitObject *object, GitRepository *repo) {
    // add header
    size_t header_size = strlen(object->fmt) + 1 + snprintf(NULL, 0, "%zu", object->size) + 1;
    size_t total_size = header_size + object->size;
    char *result = malloc(total_size);
    if(result == NULL) {
        fprintf(stderr, "Error allocating memory for result\n");
        free(result);
        return NULL;
    }

    snprintf(result, header_size, "%s %zu", object->fmt, object->size);
    memcpy(result + header_size, object->data, object->size);

    // compute SHA1
    unsigned char sha1[SHA_DIGEST_LENGTH];
    SHA1((unsigned char *)result, total_size, sha1);

    char *sha_str = malloc(SHA_DIGEST_LENGTH * 2 + 1);
    if(sha_str == NULL) {
        fprintf(stderr, "Error allocating memory for sha_str\n");
        free(result);
        return NULL;
    }

    for(int i = 0; i < SHA_DIGEST_LENGTH; i++) {
        sprintf(sha_str + i * 2, "%02x", sha1[i]);
    }
    sha_str[SHA_DIGEST_LENGTH * 2] = '\0';

    if(repo) {
        // compute path: repo->gitdir/objects/xx/yyyyyyyyyyyyyyyy
        char path[512];
        snprintf(path, sizeof(path), "%s/objects/%.2s/%s", repo->gitdir, sha_str, sha_str + 2);

        // create dir if it doesn't exist
        char dir_path[512];
        snprintf(dir_path, sizeof(dir_path), "%s/objects/%.2s", repo->gitdir, sha_str);
        mkdir(dir_path, 0755);

        // wirte the object to the file
        FILE *f = fopen(path, "wb");
        if(f == NULL) {
            fprintf(stderr, "Error opening file %s: %s\n", path, strerror(errno));
            free(result);
            free(sha_str);
            return NULL;
        }

        // allocate memory for the compressed data
        uLongf compressed_size = compressBound(total_size);
        unsigned char *raw = malloc(compressed_size);
        if(raw == NULL) {
            fprintf(stderr, "Error allocating memory for raw\n");
            fclose(f);
            free(result);
            free(sha_str);
            return NULL;
        }

        // compress the data
        if(compress(raw, &compressed_size, (const Bytef *)result, total_size) != Z_OK) {
            fprintf(stderr, "Error compressing data\n");
            fclose(f);
            free(result);
            free(sha_str);
            free(raw);
            return NULL;
        }

        // write the compressed data to the file       
        fwrite(raw, 1, compressed_size, f);

        // clean up
        fclose(f);
        free(raw);
    }
    free(result);
    return sha_str;
}