#include <unistd.h>

#include "ops.h"
#include "../deadfs.h"
#include "../path.h"

int dfs_rmdir(struct dfs_context *ctx, const char *vpath)
{
	int r = -1;
	char *appath = dfs_path_vtoap_dup(ctx, vpath);

	r = rmdir(appath);

	free(appath);
	return r;
}
