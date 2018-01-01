#include <unistd.h>

#include "ops.h"
#include "../deadfs.h"
#include "../path.h"

int dfs_mkdir(struct dfs_file *file, mode_t mode)
{
	int r = -1;

	r = mkdir(file->appath, mode);

	return r;
}
