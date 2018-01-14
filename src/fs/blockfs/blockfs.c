#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>

#include "../../utils/file.h"

#include "blockfs.h"

char* blockfs_getpath(struct dfs_super *super, uint64_t id)
{
	int len = snprintf(NULL, 0, "%s/%"PRIu64, super->ctx->config->basepath, id);
	char *ret = malloc(len+1);

	snprintf(ret, len+1, "%s/%"PRIu64, super->ctx->config->basepath, id);

	return ret;
}

size_t blockfs_readblock(struct dfs_super *super, uint64_t id, void *data, size_t len)
{
	size_t r = 0;
	char *path = blockfs_getpath(super, id);

	r = dfs_readfile(path, data, len);

	free(path);
	return r;
}

size_t blockfs_writeblock(struct dfs_super *super, uint64_t id, void *data, size_t len)
{
	size_t r = 0;
	char *path = blockfs_getpath(super, id);

	r = dfs_writefile(path, data, len);

	free(path);
	return r;
}
