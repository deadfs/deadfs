#include <unistd.h>

#include "ops.h"
#include "../deadfs.h"
#include "../file.h"

int dfs_rmdir(struct dfs_file *file)
{
	int r;

	r = rmdir(file->appath);

	return r;
}
