#ifndef SRC_FILE_H_
#define SRC_FILE_H_

#include "deadfs.h"

struct dfs_file {
	const char *vpath;
	const char *appath;

	struct dfs_context *dfs_ctx;
};


#endif /* SRC_FILE_H_ */
