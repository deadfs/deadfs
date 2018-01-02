#ifndef SRC_UTILS_LOG_H_
#define SRC_UTILS_LOG_H_

#include <stdarg.h>

#include "../deadfs.h"

#define DFS_LOG_STATUS(ctx, ...) dfs_log_status(ctx, __FUNCTION__, __VA_ARGS__)
#define DFS_LOG_ERROR(ctx, ...) dfs_log_error(ctx, __FUNCTION__, __VA_ARGS__)

void dfs_log_status(struct dfs_context *ctx, const char *fn, const char *fmt, ...);
void dfs_log_error(struct dfs_context *ctx, const char *fn, const char *fmt, ...);

#endif /* SRC_UTILS_LOG_H_ */
