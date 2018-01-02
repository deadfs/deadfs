#include <stdio.h>

#include "../deadfs.h"
#include "../path.h"
#include "../err.h"


int dfs_create(struct dfs_context *ctx, const char *vpath, int trunc, struct dfs_file **retfile)
{
	int r = DFS_ERR_GENERIC;
	const char *mode = (trunc ? "wb" : "wb+");
	char *appath = NULL;
	FILE *fp = NULL;
	struct dfs_file *file = NULL;

	appath = dfs_path_vtoap_dup(ctx, vpath);

	fp = fopen(appath, mode);
	if (!fp)
		goto fail_fopen;

	file = dfs_add_file_fast(ctx, fp, vpath, appath);

	if (retfile)
		*retfile = file;

	r = 0;
fail_fopen:
	free(appath);
	return r;
}
