#ifndef SRC_DEADFS_H_
#define SRC_DEADFS_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "config.h"

#include "nenc.h"


struct dfs_file {
	UT_hash_handle hh;

	int nref;
	const char *vpath;
	const char *appath;

	struct dfs_context *dfs_ctx;

	uint64_t size;
	uint64_t nb;

	uint64_t *blockids;
};

struct dfs_context {

	struct dfs_config *config;

	char *basepath;

	// Tracked files
	struct dfs_file *files;

	int nenc_id;
	struct dfs_nenc *nencs;


};

int dfs_init(struct dfs_context *ctx, const char *basepath);
void dfs_destroy(struct dfs_context *ctx);

int dfs_open_file(struct dfs_context *ctx, const char *vpath, struct dfs_file **retfile);
int dfs_create_file(struct dfs_context *ctx, const char *vpath, struct dfs_file **retfile);
int dfs_save_file(struct dfs_file *file);
void dfs_close_file(struct dfs_file *file);

struct dfs_file* dfs_get_file(struct dfs_context *ctx, const char *vpath);

void dfs_add_nenc_fast(struct dfs_context *ctx, struct dfs_nenc_ops *ops, int id);
int dfs_nenc_calc_enclen(struct dfs_context *ctx, const char *in);
int dfs_nenc_calc_declen(struct dfs_context *ctx, const char *in);
void dfs_nenc_encode(struct dfs_context *ctx, const char *in, char *out, int len);
void dfs_nenc_decode(struct dfs_context *ctx, const char *in, char *out, int len);

#endif /* SRC_DEADFS_H_ */
