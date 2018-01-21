#include <unistd.h>
#include <sys/types.h>
#include <utlist.h>

#include "blockfs.h"


#include "../../err.h"
#include "../../utils/log.h"

static struct dfs_dentry* read_dentry(unsigned char *p, size_t len, size_t *rlen)
{
	uint8_t nlen;
	uint64_t nodeid;
	struct dfs_dentry *dentry = NULL;
	char *name = NULL;

	*rlen = 0;

	if (len < sizeof(uint64_t)+1)
		return NULL;

	nlen = p[8];

	if (len < sizeof(uint64_t)+1+nlen)
		return NULL;

	nodeid = *((uint64_t*)p);
	name = malloc(nlen+1);
	strncpy(name, (char*)p+9, nlen);
	name[nlen] = '\0';

	dentry = new_dentry(name, nodeid, NULL);

	free(name);

	*rlen = sizeof(uint64_t)+1+nlen;

	return dentry;
}

static struct dfs_dentry* rawdentry_to_dentry(unsigned char *rd, size_t len)
{
	struct dfs_dentry *list = NULL;
	struct dfs_dentry *cur = NULL;
	size_t rlen;

	while (len && (cur=read_dentry(rd, len, &rlen))) {
		DL_APPEND(list, cur);
		rd += rlen;
		len -= rlen;
	}

	return list;
}

static int init(struct dfs_node *node)
{
	node->gid = getgid();
	node->uid = getuid();
	return 0;
}

static void destroy(struct dfs_node *node)
{
	if (!node)
		return;

	free(node->private_data);
}

static int save(struct dfs_node *node)
{
	struct blfs_rawnode *rn = node->private_data;
	size_t rnlen;

	rn->mode = node->mode;
	rn->links = node->links;
	rn->size = node->size;

	rnlen = SIZEOF_BLFS_RAWNODE(*rn);

	if (blfs_writeblock(node->super, node->id, rn, rnlen) != rnlen)
		return DFS_ERR_DENIED;

	return 0;
}

static struct dfs_dentry* lookup(struct dfs_node *node)
{
	struct dfs_dentry *dret = NULL;
	struct blfs_rawnode *rn = node->private_data;
	unsigned char *rd = NULL;
	ssize_t rdlen;

	if (!node->size)
		return NULL;

	rdlen = blfs_readblock(node->super, rn->blocks[0], NULL, 0);
	if (rdlen <= 0) {
		DFS_LOG_ERROR(node->super->ctx, "Bad rdlen");
		return NULL;
	}

	rd = malloc(rdlen);


	if (blfs_readblock(node->super, rn->blocks[0], rd, rdlen) != rdlen) {
		DFS_LOG_ERROR(node->super->ctx, "Can't read rd");
		goto fail_read;
	}


	dret = rawdentry_to_dentry(rd, rdlen);
	DFS_LOG_STATUS(node->super->ctx, "DRET=%p", dret);

fail_read:
	free(rd);
	return dret;
}

int blfs_add_nodeblock(struct dfs_node *node, uint64_t blkid)
{
	struct blfs_rawnode *rn = node->private_data;


	if (!node->super->ops->exist_node(node->super, blkid))
		return -1;

	blfs_realloc_rn(node, rn->nblocks+1);

	// rn may have been changed after realloc
	rn = node->private_data;

	rn->blocks[rn->nblocks-1] = blkid;

	return 0;
}

uint64_t blfs_get_nodeblock(struct dfs_node *node, uint64_t index)
{
	struct blfs_rawnode *rn = node->private_data;

	if (index >= rn->nblocks)
		return 0;

	return rn->blocks[index];
}

uint64_t blfs_get_nblocks(struct dfs_node *node)
{
	struct blfs_rawnode *rn = node->private_data;

	if (!rn)
		return 0;

	return rn->nblocks;
}

void blfs_realloc_rn(struct dfs_node *node, uint64_t nblocks)
{
	struct blfs_rawnode *rn = node->private_data;
	rn = realloc(rn, sizeof(struct blfs_rawnode) + (nblocks * sizeof(uint64_t)));
	rn->nblocks = nblocks;
	node->private_data = rn;
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

const struct dfs_nodeops blfs_nops = {
		.init = init,
		.destroy = destroy,
		.lookup = lookup,
		.save = save
};
