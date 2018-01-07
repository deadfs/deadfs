#include <unistd.h>
#include <sys/types.h>

#include "../deadfs.h"


int dfs_truncate(struct dfs_file *file, off_t offset)
{
	int r = truncate(file->appath, offset);

	return r;
}
