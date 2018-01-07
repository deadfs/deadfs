#include <unistd.h>
#include <string.h>

#include <uthash.h>

#include "deadfs.h"
#include "path.h"
#include "intl.h"
#include "err.h"


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

static struct dfs_file* new_file(struct dfs_context *ctx, const char *vpath, uint64_t size, uint64_t nb, uint64_t *blockids)
{
	struct dfs_file *file = NULL;

	file = calloc(1, sizeof(struct dfs_file));

	file->dfs_ctx = ctx;
	file->nref = 1;
	file->vpath = strdup(vpath);
	file->appath = dfs_path_vtoap_dup(ctx, vpath);
	file->size = size;
	file->nb = nb;
	file->blockids = blockids;

	return file;
}

static void free_file(struct dfs_file *file)
{
	if (!file)
		return;

	free((void*)file->vpath);
	free((void*)file->appath);
	free(file->blockids);
	free(file);
}

static void track_file(struct dfs_file *file)
{
	struct dfs_context *ctx = file->dfs_ctx;

	HASH_ADD_KEYPTR(hh, ctx->files, file->vpath, strlen(file->vpath), file);
}

static void untrack_file(struct dfs_file *file)
{
	if (!file)
		return;

	HASH_DEL(file->dfs_ctx->files, file);
}


struct dfs_file* dfs_get_file(struct dfs_context *ctx, const char *vpath)
{
	struct dfs_file *file = NULL;

	HASH_FIND_STR(ctx->files, vpath, file);

	return file;
}

int dfs_save_file(struct dfs_file *file)
{
	struct dfs_filehdr hdr = {0};

	hdr.size = file->size;
	hdr.nb = file->nb;

	return dfs_writefile(file->dfs_ctx, file->appath, &hdr, file->blockids);
}

int dfs_open_file(struct dfs_context *ctx, const char *vpath, struct dfs_file **retfile)
{
	int r = DFS_ERR_GENERIC;
	int rr;
	struct dfs_file *file = NULL;
	struct dfs_filehdr hdr;
	char *appath = NULL;
	uint64_t *blockids = NULL;

	*retfile = NULL;
	file = dfs_get_file(ctx, vpath);

	// If the file is already opened somewhere
	if (file) {
		file->nref++;
		goto file_opened;
	}

	appath = dfs_path_vtoap_dup(ctx, vpath);

	rr = dfs_readfile(ctx, appath, &hdr, &blockids);
	if (rr != 0) {
		r = rr;
		goto fail_readfile;
	}

	file = new_file(ctx, vpath, hdr.size, hdr.nb, blockids);
	track_file(file);

file_opened:
	if (retfile)
		*retfile = file;

	r = 0;
fail_readfile:
	free(appath);
	return r;
}

int dfs_create_file(struct dfs_context *ctx, const char *vpath, struct dfs_file **retfile)
{
	int r = DFS_ERR_GENERIC, rr;
	struct dfs_file *file = NULL;
	struct dfs_filehdr hdr = {0};
	char *appath = NULL;


	file = dfs_get_file(ctx, vpath);
	if (file) {
		r = DFS_ERR_ALOPEN;
		goto fail_opened;
	}
	appath = dfs_path_vtoap_dup(ctx, vpath);

	rr = dfs_writefile(ctx, appath, &hdr, NULL);
	if (rr != 0){
		r = rr;
		goto fail_writefile;
	}

	file = new_file(ctx, vpath, hdr.size, hdr.nb, NULL);

	track_file(file);

	if (retfile)
		*retfile = file;

	r = 0;
fail_writefile:
	free(appath);
fail_opened:
	return r;
}

void dfs_close_file(struct dfs_file *file)
{
	if (!file)
		return;

	file->nref--;

	// There are still references to this file
	if (file->nref > 0)
		return;

	untrack_file(file);
	free_file(file);
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
