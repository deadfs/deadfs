#ifndef SRC_FILE_H_
#define SRC_FILE_H_

#include <stdint.h>
#include <stdio.h>

#include <uthash.h>

#include "deadfs.h"


struct dfs_file {
	UT_hash_handle hh;

	FILE *fp;
	const char *vpath;
	const char *appath;

	struct dfs_context *dfs_ctx;

	uint64_t size;
	uint64_t nb;

	uint64_t *blocks;
};


#endif /* SRC_FILE_H_ */
