#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>


#include "../deadfs.h"
#include "../path.h"
#include "../err.h"

int dfs_getattr(struct dfs_context *ctx, const char *vpath, struct stat *st)
{
	int r = DFS_ERR_GENERIC;
	char *appath = NULL;

	appath = dfs_path_vtoap_dup(ctx, vpath);

	if (access(appath, F_OK) == -1) {
		r = DFS_ERR_NOENT;
		goto fail_access;
	}

	if (stat(appath, st) != 0)
		goto fail_stat;

	r = 0;
fail_stat:
fail_access:
	free(appath);
	return r;
}
