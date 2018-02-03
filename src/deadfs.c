#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <uthash.h>
#include <utlist.h>

#include "deadfs.h"
#include "err.h"
#include "utils/log.h"

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

	if (!ctx->node) {
		DFS_LOG_ERROR(ctx, "Failed to get the root node");
		goto cleanup;
	}

	ctx->dentry = new_dentry("r00t", ctx->node->id, ctx->node);

	ctx->dentry->children = ctx->node->ops->lookup(ctx->node);

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

	DFS_LOG_STATUS(ctx, "super: %p, ops: %p, destroy: %p", ctx->super, ctx->super->ops, ctx->super->ops->destroy);

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

void node_default_dir(struct dfs_node *node)
{
	node->mode = S_IFDIR | 0755;
	node->links = 2;
}

void node_default_reg(struct dfs_node *node)
{
	node->mode = S_IFREG | 0664;
	node->links = 1;
}

struct dfs_file* new_file(struct dfs_node *node, const struct dfs_fileops *ops)
{
	struct dfs_file *file = NULL;

	file = calloc(1, SIZEOF_FILE);
	file->ctx = node->super->ctx;
	file->node = node;
	file->ops = ops;

	if (ops->init)
		if (ops->init(file) != 0) {
			free(file);
			file = NULL;
		}

	return file;
}

void free_file(struct dfs_file *file)
{
	if (!file)
		return;

	if (file->ops->destroy)
		file->ops->destroy(file);

	free(file);
}

int dfs_getattr(struct dfs_context *ctx, const char *vpath, struct stat *st)
{
	struct dfs_dentry *dentry = dfs_get_dentry(ctx, vpath);
	struct dfs_node *node = NULL;

	if (!dentry)
		return -ENOENT;

	if (!dentry->node)
		dentry->node = ctx->super->ops->read_node(ctx->super, dentry->nodeid);

	if (!dentry->node)
		return -1;

	node = dentry->node;

	st->st_ino = node->id;
	st->st_mode = node->mode;
	st->st_uid = node->uid;
	st->st_gid = node->gid;
	st->st_size = node->size;
	st->st_nlink = node->links;

	return 0;
}

static int ends_with_slash(const char *path)
{
	return path[strlen(path)-1] == '/';
}

static void split_path(char *path, char **dir, char **name)
{
	char *f;

	if (ends_with_slash(path))
		return;

	f = strrchr(path, '/');
	*f = '\0';

	*dir = path;
	*name = f+1;
}

int dfs_mkdir(struct dfs_context *ctx, const char *vpath, mode_t mode)
{
	int r = -1;
	struct dfs_super *super = ctx->super;
	struct dfs_node *node;
	struct dfs_dentry *de, *newde;
	char *tmp, *name, *dir;

	if (ends_with_slash(vpath))
		return DFS_ERR_GENERIC;

	tmp = strdup(vpath);

	split_path(tmp, &dir, &name);

	de = dfs_get_dentry(ctx, dir);
	if (!de)
		goto cleanup;

	node = super->ops->create_node(super);
	// No need to check for erros

	node_default_dir(node);

	if (node->ops->save(node) != 0) {
		free_node(node);
		return DFS_ERR_GENERIC;
	}

	newde = new_dentry(name, node->id, node);

	DL_APPEND(de->children, newde);

	r = 0;
cleanup:
	free(tmp);
	return r;
}

struct dfs_node* dfs_get_node(struct dfs_context *ctx, const char *vpath)
{
	struct dfs_dentry *d = dfs_get_dentry(ctx, vpath);
	if (!d)
		return NULL;

	return d->node;
}

static struct dfs_dentry* search_dentry(struct dfs_dentry *list, const char *name)
{
	struct dfs_dentry *cur;

	DL_FOREACH (list, cur) {
		if (strcmp(cur->name, name) == 0)
			return cur;
	}

	return NULL;
}

static struct dfs_dentry* walk_dentry(struct dfs_dentry *dentry, char *token, char **save)
{
	struct dfs_dentry *found = search_dentry(dentry, token);

	if (!found)
		return NULL;

	token = strtok_r(NULL, "/", save);
	if (!token)
		return found;

	if (!found->children)
		return NULL;

	return walk_dentry(found->children, token, save);
}

struct dfs_dentry* dfs_get_dentry(struct dfs_context *ctx, const char *vpath)
{
	char *tmp = NULL;
	char *save = NULL;
	char *token = NULL;
	struct dfs_dentry *ret;

	if (vpath[0] == '\0' || strcmp(vpath, "/") == 0)
		return ctx->dentry;

	// else walk dentries
	tmp = strdup(vpath);
	token = strtok_r(tmp, "/", &save);

	ret = walk_dentry(ctx->dentry->children, token, &save);

	free(tmp);

	return ret;
}
