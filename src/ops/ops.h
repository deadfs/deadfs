#ifndef SRC_OPS_OPS_H_
#define SRC_OPS_OPS_H_

#include <stdlib.h>
#include <sys/stat.h>

#include "../deadfs.h"
#include "../file.h"

int dfs_getattr(struct dfs_context *ctx, const char *vpath, struct stat *st);
int dfs_readdir(struct dfs_context *ctx, const char *vpath,
		void (*cb)(const char *name, const struct stat *st, void *p), void *p);

int dfs_open(struct dfs_context *ctx, const char *vpath, unsigned int flags, struct dfs_file **retfile);
int dfs_release(struct dfs_file *file);
int dfs_create(struct dfs_context *ctx, const char *vpath, int trunc, struct dfs_file **retfile);

int dfs_write(struct dfs_file *file, const unsigned char *buf, size_t len, off_t offset);
int dfs_read(struct dfs_file *file, unsigned char *buf, size_t len, off_t offset);
int dfs_truncate(struct dfs_file *file, off_t offset);

int dfs_rmdir(struct dfs_context *ctx, const char *vpath);
int dfs_mkdir(struct dfs_context *ctx, const char *vpath, mode_t mode);
int dfs_unlink(struct dfs_context *ctx, const char *vpath);

int dfs_rename(struct dfs_context *ctx, const char *old_path, const char *new_path);

#endif /* SRC_OPS_OPS_H_ */
