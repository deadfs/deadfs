#include <stdio.h>

#include "../deadfs.h"
#include "../path.h"
#include "../err.h"
#include "../intl.h"

int dfs_create(struct dfs_context *ctx, const char *vpath, int trunc, struct dfs_file **retfile)
{
	int r = DFS_ERR_GENERIC;
	const char *mode = (trunc ? "wb" : "wb+");
	char *appath = NULL;
	FILE *fp = NULL;
	struct dfs_file *file = NULL;
	struct dfs_filehdr hdr = {0};

	appath = dfs_path_vtoap_dup(ctx, vpath);
	if (dfs_writefile(ctx, appath, &hdr, NULL) != 0)
		goto fail_writefile;
/*
	fp = fopen(appath, mode);
	if (!fp)
		goto fail_fopen;
*/
	file = dfs_add_file_fast(ctx, NULL, vpath, hdr.size, hdr.nb, NULL);

	if (retfile)
		*retfile = file;

	r = 0;
fail_writefile:
fail_fopen:
	free(appath);
	return r;
}
