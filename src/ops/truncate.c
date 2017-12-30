#include <unistd.h>
#include <sys/types.h>

#include "../deadfs.h"
#include "../path.h"


int dfs_truncate(struct dfs_context *ctx, const char *path, off_t offset)
{
	char *appath = dfs_path_vtoap_dup(ctx, path);
	int r = truncate(appath, offset);

	free(appath);

	return r;
}
