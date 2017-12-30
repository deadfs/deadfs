
#include <stdio.h>
#include <string.h>

#include "err.h"
#include "path.h"



static size_t calc_enclen(struct dfs_context *ctx, const char *path)
{
	return strlen(path)+1;
}

int dfs_encpath(struct dfs_context *ctx, const char *path, char *encpath, size_t len)
{
	int enclen = calc_enclen(ctx, path);

	if (enclen > len)
		return DFS_ERR_NOSPACE;

	strncpy(encpath, path, enclen);

	return 0;
}

char* dfs_encpath_dup(struct dfs_context *ctx, const char *path)
{
	int enclen = calc_enclen(ctx, path);
	char *ret = malloc(enclen+1);

	dfs_encpath(ctx, path, ret, enclen+1);


	return ret;
}

static size_t calc_declen(struct dfs_context *ctx, const char *encpath)
{
	return strlen(encpath)+1;
}

int dfs_decpath(struct dfs_context *ctx, const char *encpath, char *path, size_t len)
{
	int declen = calc_declen(ctx, encpath);

	if (declen > len)
		return DFS_ERR_NOSPACE;

	strncpy(path, encpath, declen);

	return 0;
}

char* dfs_decpath_dup(struct dfs_context *ctx, const char *encpath)
{
	int declen = calc_declen(ctx, encpath);
	char *ret = malloc(declen);

	//strncpy(ret, encpath, declen+1);
	dfs_decpath(ctx, encpath, ret, declen);

	return ret;
}

// Calculate fullpath length
static size_t calc_flen(struct dfs_context *ctx, const char *path)
{
	return snprintf(NULL, 0, "%s%s", ctx->basepath, path)+1;
}

int dfs_get_fullpath(struct dfs_context *ctx, const char *decpath, char *fullpath, size_t len)
{
	int flen = calc_flen(ctx, decpath);

	if (flen > len)
		return DFS_ERR_NOSPACE;

	snprintf(fullpath, len, "%s%s", ctx->basepath, decpath);

	return 0;
}

char *dfs_get_fullpath_dup(struct dfs_context *ctx, const char *decpath)
{
	int flen = calc_flen(ctx, decpath);
	char *ret = malloc(flen);

	dfs_get_fullpath(ctx, decpath, ret, flen);

	return ret;
}
