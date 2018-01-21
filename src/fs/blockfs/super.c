#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>

#include <utlist.h>

#include "../../utils/log.h"
#include "../../err.h"

#include "blockfs.h"

#define FIRST_NODEID 1000

static int exist_node(struct dfs_super *super, nodeid_t id);

static size_t dentry_to_rawdentry(struct dfs_dentry *dentry, unsigned char *rd)
{
	size_t r = 0;
	struct dfs_dentry *cur;

	DL_FOREACH (dentry, cur) {
		unsigned char nlen = 0;
		size_t curlen = 0;

		curlen += sizeof(uint64_t) + 1;

		if (cur->name)
			nlen = strlen(cur->name);

		curlen += nlen;

		if (rd) {

			*((uint64_t*)rd) = cur->nodeid;

			rd[8] = nlen;

			if (nlen)
				strncpy((char*)&rd[9], cur->name, nlen);
		}

		rd += curlen;
		r += curlen;
	}

	return r;
}

static void node_default_dir(struct dfs_node *node)
{
	node->mode = S_IFDIR | 0755;
	node->links = 2;
}

static void node_default_reg(struct dfs_node *node)
{
	node->mode = S_IFREG | 0664;
	node->links = 1;
}

static int create_root_node(struct dfs_super *super)
{
	int r = -1;

	struct dfs_node *node = NULL;

	node = new_node(FIRST_NODEID, &blfs_nops, super);
	if (!node) {
		DFS_LOG_ERROR(super->ctx, "Can't allocate the node");
		goto fail_alloc;
	}

	node_default_dir(node);

	blfs_realloc_rn(node, 0);


	if (node->ops->save(node) != 0) {
		DFS_LOG_ERROR(super->ctx, "Can't save the root node");
		goto fail_save;
	}

	r = 0;

fail_save:
	free_node(node);
fail_alloc:
	return r;
}

static int init(struct dfs_super *super)
{
	struct blfs_context *blctx = NULL;

	if (!exist_node(super, FIRST_NODEID)) {
		DFS_LOG_STATUS(super->ctx, "blockfs_init: Root node doesn't exist. Creating it");
		if (create_root_node(super) != 0) {
			DFS_LOG_ERROR(super->ctx, "Failed to create the root node");
			return -1;
		}
	}

	blctx = calloc(1, sizeof(struct blfs_context));
	blctx->idctr = FIRST_NODEID+1;
	super->private_data = blctx;

	return 0;
}

static void destroy(struct dfs_super *super)
{
	if (!super)
		return;

	free(super->private_data);
}

static struct dfs_node* read_node(struct dfs_super *super, nodeid_t id)
{
	struct dfs_node *node = NULL;
	ssize_t len;

	if (!exist_node(super, id))
		return NULL;

	len = blfs_readblock(super, id, NULL, 0);
	if (len < sizeof(struct blfs_rawnode))
		return NULL;

	//rn = blfs_realloc_rn(NULL, (len-sizeof(struct blfs_rawnode))/sizeof(uint64_t));

	node = new_node(id, &blfs_nops, super);
	blfs_realloc_rn(node, (len-sizeof(struct blfs_rawnode))/sizeof(uint64_t));

	if (blfs_readblock(super, id, node->private_data, len) < len) {
		free_node(node);
		return NULL;
	}

	// TODO: Security check: Check if nblocks is greater than len

	//node = new_node(id, &blfs_nops, super);
	blfs_setup_node_rn(node, id, node->private_data);

	return node;
}

static struct dfs_node* get_root(struct dfs_super *super)
{
	return read_node(super, FIRST_NODEID);
}

static int exist_node(struct dfs_super *super, nodeid_t id)
{
	int r = 0;
	char *path = blfs_getpath(super, id);

	if (access(path, R_OK) == 0)
		r = 1;

	free(path);

	return r;
}

uint64_t blfs_inc_idctr(struct dfs_super *super)
{
	struct blfs_context *bctx = super->private_data;
	return ++bctx->idctr;
}


const struct dfs_superops blfs_sops = {
		.init = init,
		.destroy = destroy,
		.read_node = read_node,
		.get_root = get_root,
		//.alloc_node = alloc_node,
		.exist_node = exist_node
};
