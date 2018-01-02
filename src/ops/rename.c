#include <unistd.h>

#include "ops.h"
#include "../deadfs.h"
#include "../path.h"
#include "../file.h"

int dfs_rename(struct dfs_context *ctx, const char *old_path, const char *new_path)
{
	int r = -1;
	char *old_appath = dfs_path_vtoap_dup(ctx, old_path);
	char *new_appath = dfs_path_vtoap_dup(ctx, new_path);

	r = rename(old_appath, new_appath);

	free(old_appath);
	free(new_appath);

	return r;
}
