#include "ops.h"

#include "../file.h"

int dfs_release(struct dfs_file *file)
{
	dfs_del_file(file);
	return 0;
}