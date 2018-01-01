#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../deadfs.h"
#include "../file.h"
#include "../path.h"
#include "../err.h"

int dfs_readdir(struct dfs_file *file,
		void (*cb)(const char *name, const struct stat *st, void *p), void *p)
{
	int r = DFS_ERR_GENERIC;
	DIR *dirp = NULL;
	struct dirent *dp = NULL;

	dirp = opendir(file->appath);

	if (!dirp)
		goto fail_getdir;

	for (dp=readdir(dirp); dp; dp=readdir(dirp))
	{
		struct stat st;
		char vname[sizeof(dp->d_name)];

		dfs_path_ptov(file->dfs_ctx, dp->d_name, vname, sizeof(vname));
		stat(file->appath, &st);

		cb(vname, &st, p);
	}

	r = 0;
	closedir(dirp);
fail_getdir:
	return r;
}
