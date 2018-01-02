#ifndef SRC_FILE_H_
#define SRC_FILE_H_

#include <stdio.h>

#include <uthash.h>

#include "deadfs.h"

struct dfs_file {
	FILE *fp;
	const char *vpath;
	const char *appath;

	struct dfs_context *dfs_ctx;

	UT_hash_handle hh;
};


#endif /* SRC_FILE_H_ */
