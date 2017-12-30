#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>


#include "../path.h"
#include "../deadfs.h"
#include "../err.h"

int dfs_getattr(struct dfs_context *ctx, const char *path, struct stat *st)
{
	int r = DFS_ERR_GENERIC;
	char *encpath = dfs_encpath_dup(ctx, path);
	char *fullpath = dfs_get_fullpath_dup(ctx, encpath);

	if (access(fullpath, F_OK) == -1) {
		r = DFS_ERR_NOENT;
		goto fail_access;
	}

	if (stat(fullpath, st) != 0)
		goto fail_stat;

	r = 0;
fail_stat:
fail_access:
	free(encpath);
	free(fullpath);
	return r;
}
