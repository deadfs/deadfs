#include <stdio.h>
#include <stdlib.h>

#include "deadfs.h"
#include "fuse/fuse.h"


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

static struct fuse_operations fuseops = {
	.create = fuse_create,
	.getattr = fuse_getattr,
	.readdir = fuse_readdir
};

int main(int argc, char **argv)
{
    return fuse_main(argc, argv, &fuseops, NULL);
}
