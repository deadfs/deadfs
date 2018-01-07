#include <stdio.h>

#include "../deadfs.h"
#include "../err.h"

// TODO: Check precise ferror for return

int dfs_read(struct dfs_file *file, unsigned char *buf, size_t len, off_t offset)
{

	memset(buf, 'a', len);
	return len;

	if (fseek(file->fp, offset, SEEK_SET) != 0)
		return DFS_ERR_GENERIC;

	return fread(buf, 1, len, file->fp);
}
