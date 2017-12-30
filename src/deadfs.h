#ifndef SRC_DEADFS_H_
#define SRC_DEADFS_H_

#include <sys/stat.h>

#include "config.h"

struct dfs_context {
	char *basepath;
};


int dfs_getattr(struct dfs_context *ctx, const char *path, struct stat *st);

int dfs_readdir(struct dfs_context *ctx, const char *path,
		void (*cb)(const char *name, const struct stat *st, void *p), void *p);

int dfs_create(struct dfs_context *ctx, const char *path, int trunc);

int dfs_open(struct dfs_context *ctx, const char *path, int flags);



#endif /* SRC_DEADFS_H_ */
