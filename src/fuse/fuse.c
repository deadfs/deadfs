#include <errno.h>
#include <fcntl.h>

#include "fuse.h"
#include "../err.h"

struct cbdata {
	void *buf;
	fuse_fill_dir_t filler;
	off_t offset;
	struct fuse_file_info *fi;
};

static void cb(const char *name, const struct stat *st, void *p)
{
	struct cbdata *cd = p;

	cd->filler(cd->buf, name, NULL, 0);
}

int dfs_fuse_readdir(struct dfs_context *ctx, const char *path, void *buf, fuse_fill_dir_t filler,
		off_t offset, struct fuse_file_info *fi)
{
	struct cbdata cd = {
		.buf = buf,
		.filler = filler,
		.offset = offset,
		.fi = fi
	};


	return dfs_readdir(ctx, path, cb, &cd);
}

int dfs_fuse_getattr(struct dfs_context *ctx, const char *path, struct stat *st)
{
	int r = dfs_getattr(ctx, path, st);

	if (r < 0) {

		if (r == DFS_ERR_NOENT)
			return -ENOENT;

		return -1;
	}

	return 0;
}

int dfs_fuse_create(struct dfs_context *ctx, const char *path, mode_t mode, struct fuse_file_info *fi)
{
	int r = dfs_create(ctx, path, 0);

	if (r < 0) {
		return -1;
	}

	return r;
}

int dfs_fuse_write(struct dfs_context *ctx, const char *path, const char *buf, size_t len, off_t offset, struct fuse_file_info *fi)
{
	int r = dfs_write(ctx, path, (const unsigned char*)buf, len, offset);

	if (r < 0) {
		return -1;
	}

	return r;
}

int dfs_fuse_read(struct dfs_context *ctx, const char *path, char *buf, size_t len, off_t offset, struct fuse_file_info *fi)
{
	int r = dfs_read(ctx, path, (unsigned char*)buf, len, offset);

	if (r < 0) {
		return -1;
	}

	return r;
}

int dfs_fuse_truncate(struct dfs_context *ctx, const char *path, off_t offset)
{
	return dfs_truncate(ctx, path, offset);
}
