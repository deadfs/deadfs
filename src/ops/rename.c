#include <unistd.h>

#include "ops.h"
#include "../deadfs.h"
#include "../path.h"
#include "../file.h"

int dfs_rename(struct dfs_file *file, const char *new_path)
{
	int r = -1;
	char *new_appath = dfs_path_vtoap_dup(file->dfs_ctx, new_path);

	r = rename(file->appath, new_appath);
	free((void*)file->appath);
	file->appath = new_appath;

	return r;
}
