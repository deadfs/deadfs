#ifndef SRC_DEADFS_H_
#define SRC_DEADFS_H_

#include <stdlib.h>
#include <sys/stat.h>

#include "config.h"
#include "file.h"


struct dfs_context {
	char *basepath;

	struct dfs_file *files;
};

// Planning to get the ctx from file->ctx
int dfs_add_file(struct dfs_file *file);

struct dfs_file* dfs_add_file_fast(struct dfs_context *ctx, FILE *fp, const char *vpath, const char *appath);
void dfs_del_file(struct dfs_file *file);

struct dfs_file* dfs_get_file(struct dfs_context *ctx, const char *vpath);

#endif /* SRC_DEADFS_H_ */
