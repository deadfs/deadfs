#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>

#include "../../utils/log.h"
#include "../../err.h"

#include "blockfs.h"

static int exist_node(struct dfs_super *super, uint64_t id);
static struct dfs_node* alloc_node(struct dfs_super *super);

static void init_node(struct dfs_node *node, struct dfs_super *super)
{
	node->ops = &blockfs_nops;
	node->gid = getgid();
	node->uid = getuid();
	node->super = super;
	node->private_data = calloc(1, sizeof(struct blockfs_rawnode));
}

static int init(struct dfs_super *super)
{
	return 0;
}

static void destroy(struct dfs_super *super)
{
	return;
}

static struct dfs_node* read_node(struct dfs_super *super, uint64_t id)
{
	struct dfs_node *node = NULL;
	struct blockfs_rawnode sn;

	if (!exist_node(super, id))
		return NULL;

	if (blockfs_readblock(super, id, &sn, sizeof(sn)) < sizeof(sn))
		return NULL;

	node = alloc_node(super);
	node->mode = sn.mode;

	return node;
}

static struct dfs_node* alloc_node(struct dfs_super *super)
{
	struct dfs_node *node = NULL;

	node = calloc(1, sizeof(struct dfs_node));
	init_node(node, super);

	return node;
}

static int exist_node(struct dfs_super *super, uint64_t id)
{
	int r = 0;
	char *path = blockfs_getpath(super, id);

	if (access(path, R_OK) == 0)
		r = 1;

	free(path);

	return r;
}

const struct dfs_super_operations blockfs_sops = {
		.init = init,
		.destroy = destroy,
		.read_node = read_node,
		.alloc_node = alloc_node,
		.exist_node = exist_node
};
