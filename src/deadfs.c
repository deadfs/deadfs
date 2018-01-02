#include <unistd.h>
#include <string.h>

#include <uthash.h>

#include "deadfs.h"
#include "path.h"
#include "file.h"


struct dfs_file* dfs_get_file(struct dfs_context *ctx, const char *vpath)
{
	struct dfs_file *file = NULL;

	HASH_FIND_STR(ctx->files, vpath, file);

	return file;
}

struct dfs_file* dfs_add_file_fast(struct dfs_context *ctx, FILE *fp, const char *vpath, const char *appath)
{
	struct dfs_file *file = NULL;

	file = calloc(1, sizeof(struct dfs_file));

	file->dfs_ctx = ctx;
	file->fp = fp;
	file->vpath = strdup(vpath);
	file->appath = appath ? strdup(appath) : dfs_path_vtoap_dup(ctx, vpath);

	HASH_ADD_KEYPTR(hh, ctx->files, file->vpath, strlen(file->vpath), file);

	return file;
}

void dfs_del_file(struct dfs_file *file)
{
	if (!file)
		return;

	HASH_DEL(file->dfs_ctx->files, file);

	if (file->fp)
		fclose(file->fp);

	free((void*)file->vpath);
	free((void*)file->appath);
	free(file);
}
