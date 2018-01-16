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

		// Saving it
		ctx->node->ops->save(ctx->node);

		DFS_LOG_STATUS(ctx, "Node wrote %p", ctx->node);

	}

	if (ctx->node->ops->lookup)
		ctx->entry = ctx->node->ops->lookup(ctx->node);
	else {
		ctx->entry = calloc(1, sizeof(struct dfs_dentry));
	}

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
