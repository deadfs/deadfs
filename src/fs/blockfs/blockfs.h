#ifndef SRC_FS_BLOCKFS_BLOCKFS_H_
#define SRC_FS_BLOCKFS_BLOCKFS_H_

#include "../../deadfs.h"


extern const struct dfs_file_operations blockfs_fops;
extern const struct dfs_node_operations blockfs_nops;
extern const struct dfs_super_operations blockfs_sops;

struct __attribute__ ((packed)) blockfs_rawnode {
	uint16_t mode;
};

char* blockfs_getpath(struct dfs_super *super, uint64_t id);
size_t blockfs_readblock(struct dfs_super *super, uint64_t id, void *data, size_t len);
size_t blockfs_writeblock(struct dfs_super *super, uint64_t id, void *data, size_t len);


#endif /* SRC_FS_BLOCKFS_BLOCKFS_H_ */
