#include <stdio.h>
#include <stdlib.h>

#include "deadfs.h"
#include "fuse/fuse.h"
#include "ops/ops.h"


static struct dfs_context dfs_ctx = {
		.basepath = "/Users/francesco/Downloads/tmp/a"
};

static int fuse_getattr(const char *path, struct stat *st)
{
	return dfs_fuse_getattr(&dfs_ctx, path, st);
}

static int fuse_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		off_t offset, struct fuse_file_info *fi)
{
	return dfs_fuse_readdir(&dfs_ctx, path, buf, filler, offset, fi);
}

static int fuse_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
	return dfs_fuse_create(&dfs_ctx, path, mode, fi);
}

static int fuse_write(const char *path, const char *buf, size_t len, off_t offset, struct fuse_file_info *fi)
{
	return dfs_fuse_write(&dfs_ctx, path, buf, len, offset, fi);
}

static int fuse_read(const char *path, char *buf, size_t len, off_t offset, struct fuse_file_info *fi)
{
	return dfs_fuse_read(&dfs_ctx, path, buf, len, offset, fi);
}

static int fuse_truncate(const char *path, off_t offset)
{
	return dfs_fuse_truncate(&dfs_ctx, path, offset);
}

static int fuse_rmdir(const char *path)
{
	return dfs_fuse_rmdir(&dfs_ctx, path);
}

static int fuse_mkdir(const char *path, mode_t mode)
{
	return dfs_fuse_mkdir(&dfs_ctx, path, mode);
}

static int fuse_unlink(const char *path)
{
	return dfs_fuse_unlink(&dfs_ctx, path);
}

static int fuse_rename(const char *old_path, const char *new_path)
{
	return dfs_fuse_rename(&dfs_ctx, old_path, new_path, 0);
}

static struct fuse_operations fuseops = {
	.rename = fuse_rename,
	.unlink = fuse_unlink,
	.mkdir = fuse_mkdir,
	.rmdir = fuse_rmdir,
	.truncate = fuse_truncate,
	.read = fuse_read,
	.write = fuse_write,
	.create = fuse_create,
	.getattr = fuse_getattr,
	.readdir = fuse_readdir
};

int main(int argc, char **argv)
{
    return fuse_main(argc, argv, &fuseops, NULL);
}
