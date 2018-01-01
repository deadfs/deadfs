#include <stdio.h>

#include "../deadfs.h"
#include "../file.h"
#include "../err.h"

// TODO: Check precise ferror for return

int dfs_read(struct dfs_file *file, unsigned char *buf, size_t len, off_t offset)
{
	int r = DFS_ERR_GENERIC, rr;
	FILE *fp = NULL;

	fp = fopen(file->appath, "rb");
	if (!fp)
		goto fail_fopen;

	if (fseek(fp, offset, SEEK_SET) != 0)
		goto fail_fseek;

	if ((rr=fread(buf, 1, len, fp)) >= 0) {
		r = rr;
		goto partial_fread;
	} else {
		goto fail_fread;
	}

	r = 0;

partial_fread:
fail_fread:
fail_fseek:
	fclose(fp);
fail_fopen:
	return r;
}
