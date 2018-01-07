#include <unistd.h>
#include <fcntl.h>

#include "ops.h"
#include "../err.h"
#include "../path.h"

int dfs_open(struct dfs_context *ctx, const char *vpath, unsigned int flags, struct dfs_file **retfile)
{
	return dfs_open_file(ctx, vpath, retfile);
}
