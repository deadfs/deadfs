#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

#include "file.h"

int dfs_readfile(const char *path, unsigned char **data, size_t *len)
{
	int r = -1;
	FILE *fp = NULL;
	struct stat st;

	if (stat(path, &st) != 0)
		goto fail_stat;

	*len = st.st_size;

	fp = fopen(path, "rb");
	if (!fp)
		goto fail_fopen;

	*data = malloc(*len);
	if (fread(*data, 1, *len, fp) != *len) {
		free(*data);
		goto fail_fread;
	}

	r = 0;

fail_fread:
	fclose(fp);
fail_stat:
fail_fopen:
	return r;
}

int dfs_writefile(const char *path, const unsigned char *data, size_t len)
{
	int r = -1;
	FILE *fp = NULL;
	struct stat st;

	fp = fopen(path, "wb");
	if (!fp)
		goto fail_fopen;

	if (fwrite(data, 1, len, fp) != len)
		goto fail_fwrite;

	r = 0;

fail_fwrite:
	fclose(fp);
fail_stat:
fail_fopen:
	return r;
}
