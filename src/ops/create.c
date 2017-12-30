#include <stdio.h>

#include "../deadfs.h"
#include "../path.h"
#include "../err.h"


int dfs_create(struct dfs_context *ctx, const char *path, int trunc)
{
	int r = DFS_ERR_GENERIC;
	char *encpath = dfs_encpath_dup(ctx, path);
	char *fullpath = dfs_get_fullpath_dup(ctx, encpath);
	char *mode = (trunc ? "wb" : "wb+");

	FILE *fp = fopen(fullpath, mode);
	if (!fp)
		goto fail_fopen;

	r = 0;
	fclose(fp);
fail_fopen:
	free(encpath);
	free(fullpath);
	return r;
}
