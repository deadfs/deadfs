#include "blockfs.h"

#include "../../err.h"

static void release(struct dfs_node *node)
{
	free(node->private_data);
	free(node);
}

static int save(struct dfs_node *node)
{
	struct blockfs_rawnode *rn = node->private_data;

	if (blockfs_writeblock(node->super, node->id, rn, SIZEOF_BLOCKFS_RAWNODE(*rn) ) != 0)
		return DFS_ERR_DENIED;

	return 0;
}

static struct dfs_entry* lookup(struct dfs_node *node)
{
	return NULL;
}

const struct dfs_node_operations blockfs_nops = {
		.release = release,
		.lookup = lookup,
		.save = save
};
