#ifndef SRC_CONFIG_H_
#define SRC_CONFIG_H_

#include <stdint.h>

#define DFS_MASTERKEY_LEN 32

struct dfs_config {
	uint8_t version;
	uint32_t blocksize;

	unsigned char master_key[DFS_MASTERKEY_LEN];
	// TODO: IV for blocks and filenodes

	char nenc_name[20];
	char denc_name[20];
};

#endif /* SRC_CONFIG_H_ */
