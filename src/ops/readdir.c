#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../deadfs.h"
#include "../file.h"
#include "../path.h"
#include "../err.h"

int dfs_readdir(struct dfs_context *ctx, const char *vpath,
		void (*cb)(const char *name, const struct stat *st, void *p), void *p)
{
	int r = DFS_ERR_GENERIC;
	DIR *dirp = NULL;
	struct dirent *dp = NULL;
	char *appath = NULL;

	appath = dfs_path_vtoap_dup(ctx, vpath);

	dirp = opendir(appath);

	if (!dirp)
		goto fail_getdir;

	for (dp=readdir(dirp); dp; dp=readdir(dirp))
	{
		struct stat st;
		char vname[sizeof(dp->d_name)];

		dfs_path_ptov(ctx, dp->d_name, vname, sizeof(vname));
		stat(appath, &st);

		cb(vname, &st, p);
	}

	r = 0;
	closedir(dirp);
fail_getdir:
	free(appath);
	return r;
}
