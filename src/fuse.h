#ifndef SRC_FUSE_FUSE_H_
#define SRC_FUSE_FUSE_H_

#define FUSE_USE_VERSION 26

#include <fuse.h>

#include "deadfs.h"

int dfs_fuse_readdir(struct dfs_context *ctx, const char *path, void *buf, fuse_fill_dir_t filler,
		off_t offset, struct fuse_file_info *fi);

int dfs_fuse_getattr(struct dfs_context *ctx, const char *path, struct stat *st);
int dfs_fuse_release(struct dfs_context *ctx, const char *path, struct fuse_file_info *fi);
int dfs_fuse_open(struct dfs_context *ctx, const char *path, struct fuse_file_info *fi);
int dfs_fuse_create(struct dfs_context *ctx, const char *path, mode_t mode, struct fuse_file_info *fi);
int dfs_fuse_write(struct dfs_context *ctx, const char *path, const char *buf, size_t len, off_t offset, struct fuse_file_info *fi);
int dfs_fuse_read(struct dfs_context *ctx, const char *path, char *buf, size_t len, off_t offset, struct fuse_file_info *fi);
int dfs_fuse_truncate(struct dfs_context *ctx, const char *path, off_t offset);
int dfs_fuse_rmdir(struct dfs_context *ctx, const char *path);
int dfs_fuse_mkdir(struct dfs_context *ctx, const char *path, mode_t mode);
int dfs_fuse_unlink(struct dfs_context *ctx, const char *path);
int dfs_fuse_rename(struct dfs_context *ctx, const char *old_path, const char *new_path, unsigned int flags);

#endif /* SRC_FUSE_FUSE_H_ */
