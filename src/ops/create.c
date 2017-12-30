#include <stdio.h>

#include "../deadfs.h"
#include "../path.h"
#include "../err.h"


int dfs_create(struct dfs_context *ctx, const char *path, int trunc)
{
	int r = DFS_ERR_GENERIC;
	char *appath = dfs_path_vtoap_dup(ctx, path);
	char *mode = (trunc ? "wb" : "wb+");

	FILE *fp = fopen(appath, mode);
	if (!fp)
		goto fail_fopen;

	r = 0;
	fclose(fp);
fail_fopen:
	free(appath);
	return r;
}
