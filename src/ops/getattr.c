#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>


#include "../deadfs.h"
#include "../file.h"
#include "../err.h"

int dfs_getattr(struct dfs_file *file, struct stat *st)
{
	int r = DFS_ERR_GENERIC;

	if (access(file->appath, F_OK) == -1) {
		r = DFS_ERR_NOENT;
		goto fail_access;
	}

	if (stat(file->appath, st) != 0)
		goto fail_stat;

	r = 0;
fail_stat:
fail_access:
	return r;
}
