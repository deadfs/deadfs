#include <stdio.h>
#include <string.h>

#include "err.h"
#include "path.h"

static size_t calc_ppath_len(struct dfs_context *ctx, const char *vpath)
{
	return strlen(vpath)+1;
}

int dfs_path_vtop(struct dfs_context *ctx, const char *path, char *ppath, size_t len)
{
	int ppath_len = calc_ppath_len(ctx, path);

	if (ppath_len > len)
		return DFS_ERR_NOSPACE;

	strncpy(ppath, path, ppath_len);

	return 0;
}

char* dfs_path_vtop_dup(struct dfs_context *ctx, const char *vpath)
{
	int ppath_len = calc_ppath_len(ctx, vpath);
	char *ret = malloc(ppath_len+1);

	dfs_path_vtop(ctx, vpath, ret, ppath_len+1);


	return ret;
}

static size_t calc_vpath_len(struct dfs_context *ctx, const char *ppath)
{
	return strlen(ppath)+1;
}

int dfs_path_ptov(struct dfs_context *ctx, const char *ppath, char *path, size_t len)
{
	int vpath_len = calc_vpath_len(ctx, ppath);

	if (vpath_len > len)
		return DFS_ERR_NOSPACE;

	strncpy(path, ppath, vpath_len);

	return 0;
}

char* dfs_path_ptov_dup(struct dfs_context *ctx, const char *ppath)
{
	int vpath_len = calc_vpath_len(ctx, ppath);
	char *ret = malloc(vpath_len);

	//strncpy(ret, encpath, declen+1);
	dfs_path_ptov(ctx, ppath, ret, vpath_len);

	return ret;
}

// Calculate physical path length
size_t calc_appath_len(struct dfs_context *ctx, const char *vpath)
{
	return strlen(ctx->basepath)+calc_ppath_len(ctx, vpath);
}

int dfs_path_vtoap(struct dfs_context *ctx, const char *vpath, char *appath, size_t len)
{
	int r = -1;
	int appath_len = calc_appath_len(ctx, vpath);
	char *ppath = NULL;

	if (appath_len > len)
		goto fail_len;

	ppath = dfs_path_vtop_dup(ctx, vpath);

	snprintf(appath, appath_len, "%s%s", ctx->basepath, ppath);

	r = 0;
	free(ppath);
fail_len:
	return r;
}

char* dfs_path_vtoap_dup(struct dfs_context *ctx, const char *vpath)
{
	int appath_len = calc_appath_len(ctx, vpath);
	char *ret = malloc(appath_len);

	dfs_path_vtoap(ctx, vpath, ret, appath_len);

	return ret;
}
