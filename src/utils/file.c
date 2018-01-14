#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

#include "file.h"

size_t dfs_readfile(const char *path, unsigned char *data, size_t len)
{
	size_t ret = 0;
	FILE *fp = NULL;
	struct stat st;

	if (stat(path, &st) != 0)
		goto fail_stat;

	if (!data || !len)
		return st.st_size;

	fp = fopen(path, "rb");
	if (!fp)
		goto fail_fopen;

	ret = fread(data, 1, len, fp);

	fclose(fp);
fail_fopen:
fail_stat:
	return ret;
}


size_t dfs_writefile(const char *path, const unsigned char *data, size_t len)
{
	size_t ret = 0, rr;
	FILE *fp = NULL;

	fp = fopen(path, "wb");
	if (!fp)
		goto fail_fopen;

	ret = fwrite(data, 1, len, fp);

	fclose(fp);
fail_fopen:
	return ret;
}