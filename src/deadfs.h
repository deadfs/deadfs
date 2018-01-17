#ifndef SRC_DEADFS_H_
#define SRC_DEADFS_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <uthash.h>

#include "config.h"

struct dfs_node;
struct dfs_file;
struct dfs_dentry;
struct dfs_super;


struct dfs_super_operations {

	int (*init)(struct dfs_super*);
	void (*destroy)(struct dfs_super*);

	struct dfs_node* (*read_node)(struct dfs_super*, uint64_t);
	struct dfs_node* (*alloc_node)(struct dfs_super*);

	int (*exist_node)(struct dfs_super*, uint64_t);

};

struct dfs_super {
	const struct dfs_super_operations *ops;
	void *private_data;
	struct dfs_context *ctx;
};


struct dfs_dentry {

	char *name;

	struct dfs_node *node;

	struct dfs_dentry *parent;
	struct dfs_dentry *children;
	struct dfs_dentry *prev, *next;
};

struct dfs_node_operations {
	void (*release)(struct dfs_node*);
	int (*save)(struct dfs_node*);
	struct dfs_dentry* 	(*lookup)(struct dfs_node*);
	struct dfs_file*	(*open_file)(struct dfs_node*);
};

struct dfs_node {

	uint64_t	id;

	mode_t		mode;
	uid_t		uid;
	gid_t		gid;
	uint64_t	size;
	uint16_t	links;

	const struct dfs_node_operations *ops;
	void *private_data;

	struct dfs_super *super;
};


struct dfs_file_operations {
	int (*release)(struct dfs_file*, struct dfs_node*);
	ssize_t (*read)(struct dfs_file*, unsigned char*, size_t);
	ssize_t (*write)(struct dfs_file*, unsigned char*, size_t);
};

struct dfs_file {

	struct dfs_node *node;

	const struct dfs_file_operations *ops;
	void *private_data;
};

struct dfs_context {

	struct dfs_config	*config;

	struct dfs_super *super;
	struct dfs_node *node;
	struct dfs_dentry *dentry;
	//const struct dfs_super_operations *sops;

};

int dfs_init(struct dfs_context *ctx, const struct dfs_super_operations *sops);
void dfs_destroy(struct dfs_context *ctx);

int dfs_getattr(struct dfs_context *ctx, const char *vpath, struct stat *st);
struct dfs_dentry* dfs_get_dentry(struct dfs_context *ctx, const char *vpath);

#endif /* SRC_DEADFS_H_ */
