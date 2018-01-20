#include <unistd.h>
#include <string.h>

#include <uthash.h>
#include <utlist.h>

#include "deadfs.h"
#include "err.h"
#include "utils/log.h"


static struct dfs_dentry* base_dentry(struct dfs_node *node)
{
	struct dfs_dentry *ret = NULL;
	struct dfs_dentry *d1, *d2;

	d1 = new_dentry(".", node->id, node);
	d2 = new_dentry("..", node->id, node);

	DL_APPEND(ret, d1);
	DL_APPEND(ret, d2);

	return ret;
}

int dfs_init(struct dfs_context *ctx, const struct dfs_superops *sops)
{
	int r = DFS_ERR_GENERIC;

	ctx->super = calloc(1, sizeof(struct dfs_super));
	ctx->super->ops = sops;
	ctx->super->ctx = ctx;

	if (sops->init)
		if (sops->init(ctx->super) != 0)
			goto cleanup;

	ctx->node = sops->get_root(ctx->super);

	DFS_LOG_STATUS(ctx, "Node read: %p", ctx->node);

	if (!ctx->node)
		goto cleanup;

	ctx->dentry = ctx->node->ops->lookup(ctx->node);
	if (!ctx->dentry)
		ctx->dentry = base_dentry(ctx->node);


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

struct dfs_dentry* new_dentry(const char *name, nodeid_t nodeid, struct dfs_node *node)
{
	struct dfs_dentry *dentry = calloc(1, SIZEOF_DENTRY);

	dentry->name = strdup(name);
	dentry->nodeid = nodeid;
	dentry->node = node;

	return dentry;
}

void free_dentry(struct dfs_dentry *dentry)
{
	if (!dentry)
		return;

	free((void*)dentry->name);
	free(dentry);
}

struct dfs_node* new_node(nodeid_t id, const struct dfs_nodeops *ops, struct dfs_super *super)
{
	struct dfs_node *node = calloc(1, SIZEOF_NODE);
	node->id = id;
	node->ops = ops;
	node->super = super;
	if (node->ops->init) {
		if (node->ops->init(node) != 0) {
			free(node);
			return NULL;
		}
	}

	return node;
}

void free_node(struct dfs_node *node)
{
	if (!node)
		return;

	if (node->ops->destroy)
		node->ops->destroy(node);

	free(node);
}

struct dfs_file* new_file(struct dfs_node *node, const struct dfs_fileops *ops)
{
	struct dfs_file *file = NULL;

	file = calloc(1, SIZEOF_FILE);
	file->ctx = node->super->ctx;
	file->node = node;
	file->ops = ops;

	if (ops->open)
		if (ops->open(file) != 0) {
			free(file);
			file = NULL;
		}

	return file;
}

void free_file(struct dfs_file *file)
{
	if (!file)
		return;

	if (file->ops->release)
		file->ops->release(file);

	free(file);
}

int dfs_getattr(struct dfs_context *ctx, const char *vpath, struct stat *st)
{
	struct dfs_dentry *dentry = dfs_get_dentry(ctx, vpath);
	struct dfs_node *node = NULL;

	if (!dentry)
		return -1;

	if (!dentry->node)
		dentry->node = ctx->super->ops->read_node(ctx->super, dentry->nodeid);

	if (!dentry->node)
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

	tmp = strdup(vpath);
	token = strtok_r(tmp, "/", &save);
	cur = ctx->dentry;

	while (token && cur) {

		token = strtok_r(NULL, "/", &save);
		cur = cur->children;
	}

	free(tmp);
	return NULL;
}
