#ifndef SRC_DEADFS_H_
#define SRC_DEADFS_H_

#include <stdlib.h>
#include <sys/stat.h>

#include "config.h"

struct dfs_context {
	char *basepath;
};


struct dfs_file* dfs_get_file(struct dfs_context *ctx, const char *vpath);
void dfs_free_file(struct dfs_file *file);

#endif /* SRC_DEADFS_H_ */
