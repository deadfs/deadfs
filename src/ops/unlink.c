#include <unistd.h>

#include "ops.h"
#include "../deadfs.h"
#include "../path.h"

int dfs_unlink(struct dfs_file *file)
{
	int r;
	r = unlink(file->appath);
	return r;
}
