#include <string.h>

#include "deadfs.h"
#include "path.h"
#include "file.h"

struct dfs_file* dfs_get_file(struct dfs_context *ctx, const char *vpath)
{
	struct dfs_file *file = malloc(sizeof(struct dfs_file));

	file->dfs_ctx = ctx;
	file->vpath = strdup(vpath);
	file->appath = dfs_path_vtoap_dup(ctx, vpath);

	return file;
}

void dfs_free_file(struct dfs_file *file)
{
	if (!file)
		return;

	free((void*)file->vpath);
	free((void*)file->appath);
	free(file);
}
