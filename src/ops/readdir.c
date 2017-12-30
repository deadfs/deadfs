#include <dirent.h>

#include "../deadfs.h"
#include "../path.h"
#include "../err.h"

static DIR* getdir(struct dfs_context *ctx, const char *path)
{
	char *appath = dfs_path_vtoap_dup(ctx, path);

	DIR *ret = NULL;

	ret = opendir(appath);

	free(appath);

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
		char vname[sizeof(dp->d_name)];
		dfs_path_ptov(ctx, dp->d_name, vname, sizeof(vname));
		cb(vname, NULL, p);
	}

	r = 0;
	closedir(dirp);
fail_getdir:
	return r;
}
