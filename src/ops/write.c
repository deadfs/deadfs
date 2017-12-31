#include <stdio.h>

#include "../deadfs.h"
#include "../path.h"
#include "../err.h"

// TODO: Check precise ferror for return

int dfs_write(struct dfs_context *ctx, const char *path, const unsigned char *buf, size_t len, off_t offset)
{
	int r = DFS_ERR_GENERIC, rr;
	char *appath = dfs_path_vtoap_dup(ctx, path);
	FILE *fp = NULL;

	fp = fopen(appath, "rb+");
	if (!fp)
		goto fail_fopen;

	if (fseek(fp, offset, SEEK_SET) != 0)
		goto fail_fseek;

	if ((rr=fwrite(buf, 1, len, fp)) >= 0) {
		r = rr;
		goto partial_fwrite;
	} else {
		goto fail_fwrite;
	}

	r = 0;

partial_fwrite:
fail_fwrite:
fail_fseek:
	fclose(fp);
fail_fopen:
	free(appath);
	return r;
}