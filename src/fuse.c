#include <errno.h>
#include <fcntl.h>

#include <utlist.h>

#include "fuse.h"
#include "deadfs.h"
#include "err.h"
#include "utils/log.h"


int dfs_fuse_readdir(struct dfs_context *ctx, const char *path, void *buf, fuse_fill_dir_t filler,
		off_t offset, struct fuse_file_info *fi)
{
	struct dfs_dentry *dentry = NULL, *cur;

	DFS_LOG_STATUS(ctx, "vpath: %s", path);

	dentry = dfs_get_dentry(ctx, path);

	// TODO: FIXME
	if (dentry && dentry->children) {
		DL_FOREACH (dentry->children, cur) {
			printf("LOL: %s\n", cur->name);
			filler(buf, cur->name, NULL, 0);
		}
	}

	return 0;
}

int dfs_fuse_getattr(struct dfs_context *ctx, const char *path, struct stat *st)
{
	int r;

	r = dfs_getattr(ctx, path, st);
	DFS_LOG_STATUS(ctx, "vpath: %s, r: %d", path, r);
	return r;
}

int dfs_fuse_create(struct dfs_context *ctx, const char *path, mode_t mode, struct fuse_file_info *fi)
{
	return 0;
}

int dfs_fuse_open(struct dfs_context *ctx, const char *path, struct fuse_file_info *fi)
{
	return 0;
}

int dfs_fuse_release(struct dfs_context *ctx, const char *path, struct fuse_file_info *fi)
{
	return 0;
}

int dfs_fuse_write(struct dfs_context *ctx, const char *path, const char *buf, size_t len, off_t offset, struct fuse_file_info *fi)
{
	return len;
}

int dfs_fuse_read(struct dfs_context *ctx, const char *path, char *buf, size_t len, off_t offset, struct fuse_file_info *fi)
{
	return len;
}

int dfs_fuse_truncate(struct dfs_context *ctx, const char *path, off_t offset)
{
	return 0;
}

int dfs_fuse_rmdir(struct dfs_context *ctx, const char *path)
{
	return 0;
}

int dfs_fuse_mkdir(struct dfs_context *ctx, const char *path, mode_t mode)
{
	DFS_LOG_STATUS(ctx, "vpath: %s, mode: %o", path, mode);

	return dfs_mkdir(ctx, path, mode);
}

int dfs_fuse_unlink(struct dfs_context *ctx, const char *path)
{
	return 0;
}

int dfs_fuse_rename(struct dfs_context *ctx, const char *old_path, const char *new_path, unsigned int flags)
{
	return 0;
}
