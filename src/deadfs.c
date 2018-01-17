#include <unistd.h>
#include <string.h>

#include <uthash.h>

#include "deadfs.h"
#include "err.h"
#include "utils/log.h"


int dfs_init(struct dfs_context *ctx, const struct dfs_super_operations *sops)
{
	int r = DFS_ERR_GENERIC;

	ctx->super = calloc(1, sizeof(struct dfs_super));
	ctx->super->ops = sops;
	ctx->super->ctx = ctx;

	if (sops->init)
		if (sops->init(ctx->super) != 0)
			goto cleanup;

	if (sops->exist_node(ctx->super, 1)) {

		ctx->node = sops->read_node(ctx->super, 1);

		DFS_LOG_STATUS(ctx, "Node read: %p", ctx->node);

		if (!ctx->node)
			goto cleanup;

	} else {

		ctx->node = sops->alloc_node(ctx->super);

		if (!ctx->node)
			goto cleanup;

		ctx->node->id = 1;
		ctx->node->mode = S_IFDIR | 755;
		ctx->node->links = 2;

		// Saving it
		ctx->node->ops->save(ctx->node);

		DFS_LOG_STATUS(ctx, "Node wrote %p", ctx->node);

	}

	ctx->dentry = calloc(1, sizeof(struct dfs_dentry));
	ctx->dentry->node = ctx->node;

	r = 0;
cleanup:
	if (r != 0)
		dfs_destroy(ctx);
	return r;
}

void dfs_destroy(struct dfs_context *ctx)
{
	if (!ctx)
		return;

	if (ctx->super->ops->destroy)
		ctx->super->ops->destroy(ctx->super);

	free(ctx->super);
}


int dfs_getattr(struct dfs_context *ctx, const char *vpath, struct stat *st)
{
	struct dfs_dentry *dentry = dfs_get_dentry(ctx, vpath);
	struct dfs_node *node = NULL;

	if (!dentry)
		return -1;

	node = dentry->node;

	st->st_mode = node->mode;
	st->st_uid = node->uid;
	st->st_gid = node->gid;
	st->st_size = node->size;
	st->st_nlink = node->links;

	return 0;
}

struct dfs_node* dfs_get_node(struct dfs_context *ctx, const char *vpath)
{
	struct dfs_dentry *d = dfs_get_dentry(ctx, vpath);
	if (!d)
		return NULL;

	return d->node;
}

struct dfs_dentry* dfs_get_dentry(struct dfs_context *ctx, const char *vpath)
{
	char *tmp = NULL;
	char *save = NULL;
	char *token = NULL;
	struct dfs_dentry* cur = NULL;

	if (strcmp(vpath, "/") == 0)
		return ctx->dentry;

	if (!ctx->dentry->children)
		return NULL;

	tmp = strdup(vpath);
	token = strtok_r(tmp, "/", &save);
	cur = ctx->dentry->children;

	while (token && cur) {

		token = strtok_r(NULL, "/", &save);
		cur = cur->children;
	}

	free(tmp);
	return NULL;
}
