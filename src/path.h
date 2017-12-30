#ifndef SRC_PATH_H_
#define SRC_PATH_H_

#include <stdlib.h>

#include "deadfs.h"

int dfs_path_vtop(struct dfs_context *ctx, const char *path, char *encpath, size_t len);
char* dfs_path_vtop_dup(struct dfs_context *ctx, const char *path);

int dfs_path_ptov(struct dfs_context *ctx, const char *encpath, char *path, size_t len);
char* dfs_path_ptov_dup(struct dfs_context *ctx, const char *path);

/*
int dfs_get_fullpath(struct dfs_context *ctx, const char *decpath, char *fullpath, size_t len);
char *dfs_get_fullpath_dup(struct dfs_context *ctx, const char *decpath);
*/

int dfs_path_vtoap(struct dfs_context *ctx, const char *vpath, char *appath, size_t len);
char* dfs_path_vtoap_dup(struct dfs_context *ctx, const char *vpath);

#endif /* SRC_PATH_H_ */
