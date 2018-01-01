#include <stdio.h>

#include "../deadfs.h"
#include "../file.h"
#include "../err.h"


int dfs_create(struct dfs_file *file, int trunc)
{
	int r = DFS_ERR_GENERIC;
	char *mode = (trunc ? "wb" : "wb+");

	FILE *fp = fopen(file->appath, mode);
	if (!fp)
		goto fail_fopen;

	r = 0;
	fclose(fp);
fail_fopen:
	return r;
}
