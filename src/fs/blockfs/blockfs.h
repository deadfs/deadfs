#ifndef SRC_FS_BLOCKFS_BLOCKFS_H_
#define SRC_FS_BLOCKFS_BLOCKFS_H_

#include "../../deadfs.h"


extern const struct dfs_file_operations blfs_fops;
extern const struct dfs_node_operations blfs_nops;
extern const struct dfs_super_operations blfs_sops;



struct __attribute__ ((packed)) blfs_rawnode {
	uint16_t mode;
	uint16_t links;
	uint64_t nblocks;
	uint64_t blocks[0];
};

struct blfs_context {
	uint64_t idctr;
};

#define SIZEOF_BLFS_RAWNODE(xrn) (sizeof(xrn)+(sizeof(uint64_t)*(xrn).nblocks))

char* blfs_getpath(struct dfs_super *super, nodeid_t id);
ssize_t blfs_readblock(struct dfs_super *super, nodeid_t id, void *data, size_t len);
ssize_t blfs_writeblock(struct dfs_super *super, nodeid_t id, void *data, size_t len);

void blfs_setup_node_rn(struct dfs_node *node, nodeid_t id, struct blfs_rawnode *rn);

struct blfs_rawnode* blfs_realloc_rn(struct blfs_rawnode *rn, uint64_t nblocks);

#endif /* SRC_FS_BLOCKFS_BLOCKFS_H_ */
