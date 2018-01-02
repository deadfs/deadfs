#include <unistd.h>

#include "ops.h"
#include "../deadfs.h"
#include "../path.h"

int dfs_unlink(struct dfs_context *ctx, const char *vpath)
{
	int r;
	char *appath = dfs_path_vtoap_dup(ctx, vpath);

	r = unlink(appath);

	free(appath);
	return r;
}
