#ifndef SRC_OPS_OPS_H_
#define SRC_OPS_OPS_H_

#include <stdlib.h>
#include <sys/stat.h>

#include "../deadfs.h"

int dfs_getattr(struct dfs_context *ctx, const char *path, struct stat *st);

int dfs_readdir(struct dfs_context *ctx, const char *path,
		void (*cb)(const char *name, const struct stat *st, void *p), void *p);

int dfs_create(struct dfs_context *ctx, const char *path, int trunc);

int dfs_open(struct dfs_context *ctx, const char *path, int flags);

int dfs_write(struct dfs_context *ctx, const char *path, const unsigned char *buf, size_t len, off_t offset);

int dfs_read(struct dfs_context *ctx, const char *path, unsigned char *buf, size_t len, off_t offset);

int dfs_truncate(struct dfs_context *ctx, const char *path, off_t offset);


#endif /* SRC_OPS_OPS_H_ */
