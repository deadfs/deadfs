#ifndef SRC_DEADFS_H_
#define SRC_DEADFS_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <uthash.h>

#include "config.h"


typedef uint64_t nodeid_t;
typedef uint64_t nodesize_t;

struct dfs_node;
struct dfs_file;
struct dfs_dentry;
struct dfs_super;

#define SIZEOF_DENTRY sizeof(struct dfs_dentry)
#define SIZEOF_NODE sizeof(struct dfs_node)
#define SIZEOF_SUPER sizeof(struct dfs_super)
#define SIZEOF_FILE sizeof(struct dfs_file)

struct dfs_superops {

	int (*init)(struct dfs_super*);
	void (*destroy)(struct dfs_super*);

	struct dfs_node* (*read_node)(struct dfs_super*, nodeid_t);
	//struct dfs_node* (*alloc_node)(struct dfs_super*);
	struct dfs_node* (*get_root)(struct dfs_super*);
	struct dfs_node* (*create_node)(struct dfs_super*);

	int (*exist_node)(struct dfs_super*, nodeid_t);
};

struct dfs_super {
	const struct dfs_superops *ops;
	void *private_data;
	struct dfs_context *ctx;
};


struct dfs_dentry {

	const char *name;

	nodeid_t nodeid;
	struct dfs_node *node;

	struct dfs_dentry *children;
	struct dfs_dentry *prev, *next;
};

struct dfs_nodeops {
	int (*init)(struct dfs_node*);
	void (*destroy)(struct dfs_node*);
	int (*save)(struct dfs_node*);

	struct dfs_dentry* 	(*lookup)(struct dfs_node*);
	int (*save_dentry)(struct dfs_node*, struct dfs_dentry*);

	struct dfs_file*	(*open_file)(struct dfs_node*);
};

struct dfs_node {

	nodeid_t	id;

	mode_t			mode;
	uid_t			uid;
	gid_t			gid;
	nodesize_t		size;
	unsigned int	links;

	const struct dfs_nodeops *ops;
	void *private_data;

	struct dfs_super *super;
};


struct dfs_fileops {
	int (*init)(struct dfs_file*);
	void (*destroy)(struct dfs_file*);
	off_t (*seek)(struct dfs_file*, off_t, int);
	ssize_t (*read)(struct dfs_file*, unsigned char*, size_t);
	ssize_t (*write)(struct dfs_file*, const unsigned char*, size_t);
};

struct dfs_file {

	struct dfs_context *ctx;
	struct dfs_node *node;

	const struct dfs_fileops *ops;
	void *private_data;
};

struct dfs_context {

	struct dfs_config	*config;

	struct dfs_super *super;
	struct dfs_node *node;
	struct dfs_dentry *dentry;
	//const struct dfs_super_operations *sops;

};

int dfs_init(struct dfs_context *ctx, const struct dfs_superops *sops);
void dfs_destroy(struct dfs_context *ctx);

struct dfs_dentry* new_dentry(const char *name, nodeid_t nodeid, struct dfs_node *node);
void free_dentry(struct dfs_dentry *dentry);

struct dfs_node* new_node(nodeid_t id, const struct dfs_nodeops *ops, struct dfs_super *super);
void free_node(struct dfs_node *node);
void node_default_dir(struct dfs_node *node);
void node_default_reg(struct dfs_node *node);


struct dfs_file* new_file(struct dfs_node *node, const struct dfs_fileops *ops);
void free_file(struct dfs_file *file);

int dfs_getattr(struct dfs_context *ctx, const char *vpath, struct stat *st);
struct dfs_dentry* dfs_get_dentry(struct dfs_context *ctx, const char *vpath);

#endif /* SRC_DEADFS_H_ */
