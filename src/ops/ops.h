#ifndef SRC_OPS_OPS_H_
#define SRC_OPS_OPS_H_

#include <stdlib.h>
#include <sys/stat.h>

#include "../deadfs.h"
#include "../file.h"

int dfs_getattr(struct dfs_file *file, struct stat *st);
int dfs_readdir(struct dfs_file *file,
		void (*cb)(const char *name, const struct stat *st, void *p), void *p);
int dfs_create(struct dfs_file *file, int trunc);
int dfs_write(struct dfs_file *file, const unsigned char *buf, size_t len, off_t offset);
int dfs_read(struct dfs_file *file, unsigned char *buf, size_t len, off_t offset);
int dfs_truncate(struct dfs_file *file, off_t offset);
int dfs_rmdir(struct dfs_file *file);
int dfs_mkdir(struct dfs_file *file, mode_t mode);
int dfs_unlink(struct dfs_file *file);
int dfs_rename(struct dfs_file *file, const char *new_path);

#endif /* SRC_OPS_OPS_H_ */
