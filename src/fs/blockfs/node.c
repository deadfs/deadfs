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

static void release(struct dfs_node *node)
{
	if (!node)
		return;

	free(node->private_data);
	free(node);
}

static int save(struct dfs_node *node)
{
	struct blfs_rawnode *rn = node->private_data;
	size_t rnlen;

	rn->mode = node->mode;
	rn->links = node->links;

	rnlen = SIZEOF_BLFS_RAWNODE(*rn);
	node->size = rnlen;

	if (blfs_writeblock(node->super, node->id, rn, rnlen ) != rnlen)
		return DFS_ERR_DENIED;

	return 0;
}

static struct dfs_dentry* lookup(struct dfs_node *node)
{
	struct dfs_dentry *dret = NULL;
	struct blfs_rawnode *rn = node->private_data;
	unsigned char *rd = NULL;
	ssize_t rdlen;

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

const struct dfs_node_operations blfs_nops = {
		.release = release,
		.lookup = lookup,
		.save = save
};
