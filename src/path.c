#include <stdio.h>
#include <string.h>

#include <utlist.h>

#include "err.h"
#include "path.h"

struct token {
	char *tok;
	struct token *next;
};

static struct token* tokenize(const char *str, const char *sep)
{
	struct token *tlist = NULL;
	char *tmpstr = NULL;
	char *saveptr = NULL;
	char *tmptok = NULL;

	tmpstr = strdup(str);
	tmptok = strtok_r(tmpstr, sep, &saveptr);

	while (tmptok) {

		struct token *token = calloc(1, sizeof(struct token));
		token->tok = strdup(tmptok);

		LL_APPEND(tlist, token);

		tmptok = strtok_r(NULL, sep, &saveptr);
	}

	free(tmpstr);
	return tlist;
}

static char* detokenize(struct token *tlist, const char *sep)
{

	return NULL;
}

static void free_token_list(struct token *tlist)
{
	struct token *cur, *tmp;

	LL_FOREACH_SAFE(tlist, cur, tmp) {
		LL_DELETE(tlist, cur);
		free(cur->tok);
		free(cur);
	}
}

static struct token* encode_token_list(struct dfs_context *ctx, struct token *tlist)
{
	struct token *ret = NULL, *cur;

	LL_FOREACH(tlist, cur) {
		size_t etok_len;
		struct token *etok = calloc(1, sizeof(struct token));

		etok_len = dfs_nenc_calc_enclen(ctx, cur->tok);

		etok->tok = malloc(etok_len);
		dfs_nenc_encode(ctx, cur->tok, etok->tok, etok_len);

		LL_APPEND(ret, etok);
	}

	return ret;
}

static struct token* decode_token_list(struct dfs_context *ctx, struct token *tlist)
{
	struct token *ret = NULL, *cur;

	LL_FOREACH(tlist, cur) {
		size_t dtok_len;
		struct token *dtok = calloc(1, sizeof(struct token));

		dtok_len = dfs_nenc_calc_enclen(ctx, cur->tok);

		dtok->tok = malloc(dtok_len);
		dfs_nenc_decode(ctx, cur->tok, dtok->tok, dtok_len);

		LL_APPEND(ret, dtok);
	}

	return ret;
}

static size_t calc_ppath_len(struct dfs_context *ctx, const char *vpath)
{

	size_t len = 0;

	struct token *tlist, *cur;

	tlist = tokenize(vpath, "/");

	LL_FOREACH(tlist, cur) {
		// '/' + encoded name - '\0'
		len += 1 + dfs_nenc_calc_enclen(ctx, cur->tok)-1;
	}

	if (len == 0)
		len = 1;


	free_token_list(tlist);
	return len + 1;
}

static size_t calc_vpath_len(struct dfs_context *ctx, const char *ppath)
{
	size_t len = 0;

	struct token *tlist, *cur;

	tlist = tokenize(ppath, "/");

	LL_FOREACH(tlist, cur) {
		// '/' + decoded name - '\0'
		len += 1 + (dfs_nenc_calc_declen(ctx, cur->tok)-1);
	}

	if (len == 0)
		len = 1;


	free_token_list(tlist);
	return len + 1;

}

char* dfs_path_ptov_dup(struct dfs_context *ctx, const char *ppath)
{
	struct token *tlist = tokenize(ppath, "/"), *cur, *dtlist;
	int vpath_len = calc_vpath_len(ctx, ppath);
	char *ret = malloc(vpath_len);
	int ntok = 0;

	ret[0] = '\0';

	dtlist = decode_token_list(ctx, tlist);

	LL_FOREACH(dtlist, cur) {
		ntok++;
		strcat(ret, "/");
		strcat(ret, cur->tok);
	}

	if (ntok == 0)
		strcat(ret, "/");

	free_token_list(tlist);
	return ret;
}

char* dfs_path_vtop_dup(struct dfs_context *ctx, const char *vpath)
{
	struct token *tlist = tokenize(vpath, "/"), *cur, *etlist;
	int ppath_len = calc_ppath_len(ctx, vpath);
	char *ret = malloc(ppath_len);
	int ntok = 0;

	ret[0] = '\0';

	etlist = encode_token_list(ctx, tlist);

	LL_FOREACH(etlist, cur) {
		ntok++;
		strcat(ret, "/");
		strcat(ret, cur->tok);
	}

	if (ntok == 0)
		strcat(ret, "/");

	free_token_list(tlist);
	free_token_list(etlist);

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
