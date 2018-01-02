#include <unistd.h>

#include "ops.h"
#include "../deadfs.h"
#include "../path.h"

int dfs_mkdir(struct dfs_context *ctx, const char *vpath, mode_t mode)
{
	int r = -1;
	char *appath = dfs_path_vtoap_dup(ctx, vpath);

	r = mkdir(appath, mode);

	free(appath);
	return r;
}
