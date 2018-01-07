#include <stdio.h>

#include "../deadfs.h"
#include "../err.h"

// TODO: Check precise ferror for return

int dfs_write(struct dfs_file *file, const unsigned char *buf, size_t len, off_t offset)
{
	return len;

	if (fseek(file->fp, offset, SEEK_SET) != 0)
		return DFS_ERR_GENERIC;

	return fwrite(buf, 1, len, file->fp);
}
