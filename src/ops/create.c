#include <stdio.h>

#include "../deadfs.h"
#include "../path.h"
#include "../err.h"
#include "../intl.h"


int dfs_create(struct dfs_context *ctx, const char *vpath, int trunc, struct dfs_file **retfile)
{
	return dfs_create_file(ctx, vpath, retfile);
}
