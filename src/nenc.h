#ifndef SRC_NENC_H_
#define SRC_NENC_H_

#include <uthash.h>

#include "deadfs.h"

// Name encoder interface
// Needed for encoding filenames and such thingz

enum {
	DFS_NENC_BASE32
};


struct dfs_context;

struct dfs_nenc_ops {
	void* (*init)(struct dfs_context *dfs_ctx);
	void (*destroy)(void *ctx);

	int (*calc_enclen)(const char *in, void *ctx);
	int (*calc_declen)(const char *in, void *ctx);

	void (*encode)(const char *in, char *out, int len, void *ctx);
	void (*decode)(const char *in, char *out, int len, void *ctx);
};

struct dfs_nenc {

	char id;

	void *ctx;

	struct dfs_nenc_ops ops;


	UT_hash_handle hh;
};


#endif /* SRC_NENC_H_ */
