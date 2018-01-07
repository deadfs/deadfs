#ifndef SRC_INTL_H_
#define SRC_INTL_H_

// Deadfs internal functions

#include "deadfs.h"
#include "file.h"

// Raw file header structure present on the disk

struct dfs_filehdr {
	// TODO: Put junk somewhere as the structure size leaks
	// the number of blocks, and then the file size is leaked.
	uint64_t size;

	// Number of blocks
	uint64_t nb;
};


int dfs_readfile(struct dfs_context *ctx, const char *path, struct dfs_filehdr *hdr, uint64_t **blockids);
int dfs_writefile(struct dfs_context *ctx, const char *path, const struct dfs_filehdr *hdr, const uint64_t *blockids);

int dfs_readblock(struct dfs_context *ctx, const char *path, unsigned char **data);
int dfs_writeblock(struct dfs_context *ctx, const char *path, const unsigned char *data);


#endif /* SRC_INTL_H_ */
