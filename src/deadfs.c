#include <unistd.h>
#include <string.h>

#include <uthash.h>

#include "deadfs.h"
#include "path.h"


int dfs_init(struct dfs_context *ctx, const char *basepath)
{
	ctx->basepath = strdup(basepath);
	return 0;
}

void dfs_destroy(struct dfs_context *ctx)
{
	if (!ctx)
		return;

	free(ctx->basepath);
	// TODO: Free all other elements! (files, nencs, etc)
}

struct dfs_file* dfs_get_file(struct dfs_context *ctx, const char *vpath)
{
	struct dfs_file *file = NULL;

	HASH_FIND_STR(ctx->files, vpath, file);

	return file;
}

struct dfs_file* dfs_add_file_fast(struct dfs_context *ctx, FILE *fp, const char *vpath, uint64_t size, uint64_t nb, uint64_t *blocks)
{
	struct dfs_file *file = NULL;

	file = calloc(1, sizeof(struct dfs_file));

	file->dfs_ctx = ctx;
	file->fp = fp;
	file->vpath = strdup(vpath);
	file->appath = dfs_path_vtoap_dup(ctx, vpath);
	file->size = size;
	file->nb = nb;
	file->blocks = blocks;

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
	free(file->blocks);
	free(file);
}

void dfs_add_nenc_fast(struct dfs_context *ctx, struct dfs_nenc_ops *ops, int id)
{
	struct dfs_nenc *nenc = NULL;


	nenc = calloc(1, sizeof(struct dfs_nenc));

	nenc->id = id;
	nenc->ops = *ops;

	HASH_ADD_INT(ctx->nencs, id, nenc);
}

static struct dfs_nenc* get_cur_nenc(struct dfs_context *ctx)
{
	struct dfs_nenc *nenc = NULL;

	HASH_FIND_INT(ctx->nencs, &ctx->nenc_id, nenc);

	return nenc;
}

int dfs_nenc_calc_enclen(struct dfs_context *ctx, const char *in)
{
	struct dfs_nenc *nenc = get_cur_nenc(ctx);

	return nenc->ops.calc_enclen(in, nenc->ctx);
}

int dfs_nenc_calc_declen(struct dfs_context *ctx, const char *in)
{
	struct dfs_nenc *nenc = get_cur_nenc(ctx);

	return nenc->ops.calc_declen(in, nenc->ctx);
}

void dfs_nenc_encode(struct dfs_context *ctx, const char *in, char *out, int len)
{
	struct dfs_nenc *nenc = get_cur_nenc(ctx);

	nenc->ops.encode(in, out, len, nenc->ctx);
}

void dfs_nenc_decode(struct dfs_context *ctx, const char *in, char *out, int len)
{
	struct dfs_nenc *nenc = get_cur_nenc(ctx);

	nenc->ops.decode(in, out, len, nenc->ctx);
}
