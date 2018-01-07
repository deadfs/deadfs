#include "fuse.h"

#include <errno.h>
#include <fcntl.h>

#include "deadfs.h"
#include "err.h"
#include "ops/ops.h"
#include "utils/log.h"


struct cbdata {
	void *buf;
	fuse_fill_dir_t filler;
	off_t offset;
	struct fuse_file_info *fi;
};

static void cb(const char *name, const struct stat *st, void *p)
{
	struct cbdata *cd = p;

	cd->filler(cd->buf, name, st, 0);
}

int dfs_fuse_readdir(struct dfs_context *ctx, const char *path, void *buf, fuse_fill_dir_t filler,
		off_t offset, struct fuse_file_info *fi)
{
	int r = -1;

	DFS_LOG_STATUS(ctx, "vpath: %s", path);

	struct cbdata cd = {
		.buf = buf,
		.filler = filler,
		.offset = offset,
		.fi = fi
	};


	r = dfs_readdir(ctx, path, cb, &cd);


	return r;
}

int dfs_fuse_getattr(struct dfs_context *ctx, const char *path, struct stat *st)
{
	int r;

	DFS_LOG_STATUS(ctx, "vpath: %s", path);

	r = dfs_getattr(ctx, path, st);

	if (r < 0) {

		if (r == DFS_ERR_NOENT)
			return -ENOENT;

		return -1;
	}

	return 0;
}

int dfs_fuse_create(struct dfs_context *ctx, const char *path, mode_t mode, struct fuse_file_info *fi)
{
	int r;
	struct dfs_file *file = NULL;

	DFS_LOG_STATUS(ctx, "vpath: %s", path, &file);

	r = dfs_create(ctx, path, 0, &file);

	if (r < 0) {
		return -1;
	}

	fi->fh = (uint64_t)file;

	return r;
}

int dfs_fuse_open(struct dfs_context *ctx, const char *path, struct fuse_file_info *fi)
{
	int r = -1;
	struct dfs_file *file = NULL;

	DFS_LOG_STATUS(ctx, "vpath: %s", path);

	r = dfs_open(ctx, path, fi->flags, &file);

	if (r < 0) {
		return -1;
	}

	fi->fh = (uint64_t)file;

	return r;
}

int dfs_fuse_release(struct dfs_context *ctx, const char *path, struct fuse_file_info *fi)
{
	int r = -1;
	struct dfs_file *file = (struct dfs_file*)fi->fh;


	DFS_LOG_STATUS(ctx, "vpath: %s", path);

	r = dfs_release(file);

	if (r < 0)
		return -1;

	return r;
}

int dfs_fuse_write(struct dfs_context *ctx, const char *path, const char *buf, size_t len, off_t offset, struct fuse_file_info *fi)
{
	struct dfs_file *file = (struct dfs_file*)fi->fh;
	int r;

	DFS_LOG_STATUS(ctx, "vpath: %s, file: %p, buf: %p, len: %u, off: %llu", path, fi->fh, buf, len, offset);

	r = dfs_write(file, (const unsigned char*)buf, len, offset);

	if (r < 0) {
		return -1;
	}

	return r;
}

int dfs_fuse_read(struct dfs_context *ctx, const char *path, char *buf, size_t len, off_t offset, struct fuse_file_info *fi)
{
	int r;
	struct dfs_file *file = (struct dfs_file*)fi->fh;

	DFS_LOG_STATUS(ctx, "vpath: %s, file: %p, buf: %p, len: %u, off: %llu", path, fi->fh, buf, len, offset);

	r = dfs_read(file, (unsigned char*)buf, len, offset);

	if (r < 0) {
		return -1;
	}

	return r;
}

int dfs_fuse_truncate(struct dfs_context *ctx, const char *path, off_t offset)
{
	int r;
	struct dfs_file *file = NULL;

	DFS_LOG_STATUS(ctx, "vpath: %s", path);

	file = dfs_get_file(ctx, path);
	if (!file)
		return -1;

	r = dfs_truncate(file, offset);

	return r;
}

int dfs_fuse_rmdir(struct dfs_context *ctx, const char *path)
{
	int r;

	DFS_LOG_STATUS(ctx, "vpath: %s", path);

	r = dfs_rmdir(ctx, path);

	return r;
}

int dfs_fuse_mkdir(struct dfs_context *ctx, const char *path, mode_t mode)
{
	int r;

	DFS_LOG_STATUS(ctx, "vpath: %s", path);

	r = dfs_mkdir(ctx, path, mode);

	return r;
}

int dfs_fuse_unlink(struct dfs_context *ctx, const char *path)
{
	int r;

	DFS_LOG_STATUS(ctx, "vpath: %s", path);


	r = dfs_unlink(ctx, path);


	return r;
}

int dfs_fuse_rename(struct dfs_context *ctx, const char *old_path, const char *new_path, unsigned int flags)
{
	int r;

	DFS_LOG_STATUS(ctx, "old_vpath: %s new_vpath: %s", old_path, new_path);

	r = dfs_rename(ctx, old_path, new_path);

	return r;
}
