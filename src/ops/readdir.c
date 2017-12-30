#include <dirent.h>

#include "../deadfs.h"
#include "../path.h"
#include "../err.h"

static DIR* getdir(struct dfs_context *ctx, const char *path)
{
	char *encpath = dfs_encpath_dup(ctx, path);
	char *fullpath = dfs_get_fullpath_dup(ctx, encpath);
	DIR *ret = NULL;

	ret = opendir(fullpath);

	free(encpath);
	free(fullpath);

	return ret;
}

int dfs_readdir(struct dfs_context *ctx, const char *path,
		void (*cb)(const char *name, const struct stat *st, void *p), void *p)
{
	int r = DFS_ERR_GENERIC;
	DIR *dirp = NULL;
	struct dirent *dp = NULL;

	dirp = getdir(ctx, path);

	if (!dirp)
		goto fail_getdir;

	for (dp=readdir(dirp); dp; dp=readdir(dirp))
	{
		cb(dp->d_name, NULL, p);
	}

	r = 0;
	closedir(dirp);
fail_getdir:
	return r;
}
