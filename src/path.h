#ifndef SRC_PATH_H_
#define SRC_PATH_H_

#include <stdlib.h>

#include "deadfs.h"

int dfs_path_vtop(struct dfs_context *ctx, const char *vpath, char *ppath, size_t len);
char* dfs_path_vtop_dup(struct dfs_context *ctx, const char *path);

int dfs_path_ptov(struct dfs_context *ctx, const char *ppath, char *vpath, size_t len);
char* dfs_path_ptov_dup(struct dfs_context *ctx, const char *vpath);

int dfs_path_vtoap(struct dfs_context *ctx, const char *vpath, char *appath, size_t len);
char* dfs_path_vtoap_dup(struct dfs_context *ctx, const char *vpath);

#endif /* SRC_PATH_H_ */
