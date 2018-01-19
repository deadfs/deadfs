#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>

#include "../../utils/file.h"

#include "blockfs.h"

char* blfs_getpath(struct dfs_super *super, nodeid_t id)
{
	int len = snprintf(NULL, 0, "%s/%"PRIu64, super->ctx->config->basepath, id);
	char *ret = malloc(len+1);

	snprintf(ret, len+1, "%s/%"PRIu64, super->ctx->config->basepath, id);

	return ret;
}

ssize_t blfs_readblock(struct dfs_super *super, nodeid_t id, void *data, size_t len)
{
	ssize_t r;
	char *path = blfs_getpath(super, id);

	r = dfs_readfile(path, data, len);

	free(path);
	return r;
}

ssize_t blfs_writeblock(struct dfs_super *super, nodeid_t id, void *data, size_t len)
{
	ssize_t r;
	char *path = blfs_getpath(super, id);

	r = dfs_writefile(path, data, len);

	free(path);
	return r;
}


// TODO: maybe remove id
void blfs_setup_node_rn(struct dfs_node *node, nodeid_t id, struct blfs_rawnode *rn)
{
	node->id = id;
	node->mode = rn->mode;
	node->links = rn->links;
	node->size = rn->size;
	node->private_data = rn;
}

struct blfs_rawnode* blfs_realloc_rn(struct blfs_rawnode *rn, uint64_t nblocks)
{
	struct blfs_rawnode *rnr = realloc(rn, sizeof(struct blfs_rawnode) + (nblocks * sizeof(uint64_t)));
	rnr->nblocks = nblocks;
	return rnr;
}
