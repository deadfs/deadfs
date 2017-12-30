#ifndef SRC_PATH_H_
#define SRC_PATH_H_

#include <stdlib.h>

#include "deadfs.h"

int dfs_encpath(struct dfs_context *ctx, const char *path, char *encpath, size_t len);
char* dfs_encpath_dup(struct dfs_context *ctx, const char *path);

int dfs_decpath(struct dfs_context *ctx, const char *encpath, char *path, size_t len);
char* dfs_decpath_dup(struct dfs_context *ctx, const char *path);


int dfs_get_fullpath(struct dfs_context *ctx, const char *decpath, char *fullpath, size_t len);
char *dfs_get_fullpath_dup(struct dfs_context *ctx, const char *decpath);

#endif /* SRC_PATH_H_ */
