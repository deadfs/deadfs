#include "ops.h"


int dfs_release(struct dfs_file *file)
{
	dfs_close_file(file);
	return 0;
}
