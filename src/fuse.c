#include "fuse.h"

#include <errno.h>
#include <fcntl.h>

#include "deadfs.h"
#include "file.h"
#include "err.h"
#include "ops/ops.h"


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
	struct dfs_file *file = NULL;


	struct cbdata cd = {
		.buf = buf,
		.filler = filler,
		.offset = offset,
		.fi = fi
	};

	file = dfs_get_file(ctx, path);
	if (!file)
		return -1;

	r = dfs_readdir(file, cb, &cd);

	dfs_free_file(file);

	return r;
}

int dfs_fuse_getattr(struct dfs_context *ctx, const char *path, struct stat *st)
{
	int r = -1;
	struct dfs_file *file = NULL;


	file = dfs_get_file(ctx, path);
	if (!file)
		return -1;

	r = dfs_getattr(file, st);

	dfs_free_file(file);

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

	file = dfs_get_file(ctx, path);
	if (!file)
		return -1;

	r = dfs_create(file, 0);

	dfs_free_file(file);

	if (r < 0) {
		return -1;
	}

	return r;
}

int dfs_fuse_write(struct dfs_context *ctx, const char *path, const char *buf, size_t len, off_t offset, struct fuse_file_info *fi)
{
	struct dfs_file *file = NULL;
	int r;

	file = dfs_get_file(ctx, path);
	if (!file)
		return -1;

	r = dfs_write(file, (const unsigned char*)buf, len, offset);

	dfs_free_file(file);

	if (r < 0) {
		return -1;
	}

	return r;
}

int dfs_fuse_read(struct dfs_context *ctx, const char *path, char *buf, size_t len, off_t offset, struct fuse_file_info *fi)
{
	int r;
	struct dfs_file *file = NULL;

	file = dfs_get_file(ctx, path);
	if (!file)
		return -1;

	r = dfs_read(file, (unsigned char*)buf, len, offset);

	dfs_free_file(file);

	if (r < 0) {
		return -1;
	}

	return r;
}

int dfs_fuse_truncate(struct dfs_context *ctx, const char *path, off_t offset)
{
	int r;
	struct dfs_file *file = NULL;

	file = dfs_get_file(ctx, path);
	if (!file)
		return -1;

	r = dfs_truncate(file, offset);

	dfs_free_file(file);

	return r;
}

int dfs_fuse_rmdir(struct dfs_context *ctx, const char *path)
{
	int r;
	struct dfs_file *file = NULL;

	file = dfs_get_file(ctx, path);
	if (!file)
		return -1;

	r = dfs_rmdir(file);

	dfs_free_file(file);

	return r;
}

int dfs_fuse_mkdir(struct dfs_context *ctx, const char *path, mode_t mode)
{
	int r;
	struct dfs_file *file = NULL;

	file = dfs_get_file(ctx, path);
	if (!file)
		return -1;

	r = dfs_mkdir(file, mode);

	dfs_free_file(file);

	return r;
}

int dfs_fuse_unlink(struct dfs_context *ctx, const char *path)
{
	int r;
	struct dfs_file *file = NULL;

	file = dfs_get_file(ctx, path);
	if (!file)
		return -1;

	r = dfs_unlink(file);

	dfs_free_file(file);

	return r;
}

int dfs_fuse_rename(struct dfs_context *ctx, const char *old_path, const char *new_path, unsigned int flags)
{
	int r;
	struct dfs_file *file = NULL;

	file = dfs_get_file(ctx, old_path);
	if (!file)
		return -1;

	r = dfs_rename(file, new_path);

	dfs_free_file(file);

	return r;
}
