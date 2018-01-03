
#include <stdlib.h>

#include "../deadfs.h"
#include "../nenc.h"
#include "../utils/log.h"

#include "../utils/base32.h"


struct context {
	struct dfs_context *dfs_ctx;
};


static void* init(struct dfs_context *dfs_ctx)
{
	struct context *ctx = calloc(1, sizeof(struct context));

	ctx->dfs_ctx = dfs_ctx;

	return ctx;
}

static void destroy(void *ctx)
{
	free(ctx);
}

static int calc_enclen(const char *in, void *ctx)
{
	return BASE32_LEN(strlen(in))+1;
}

static int calc_declen(const char *in, void *ctx)
{
	return UNBASE32_LEN(strlen(in))+1;
}

static void encode(const char *in, char *out, int len, void *ctx)
{
	memset(out, 0, len);
	base32_encode((const unsigned char*)in, strlen(in), (unsigned char*)out);
}

static void decode(const char *in, char *out, int len, void *ctx)
{
	memset(out, 0, len);
	base32_decode((const unsigned char*)in, (unsigned char*)out);
}

static struct dfs_nenc_ops opz = {
	.init = init,
	.destroy = destroy,
	.calc_enclen = calc_enclen,
	.calc_declen = calc_declen,
	.encode = encode,
	.decode = decode
};

void dfs_nenc_base32_init(struct dfs_context *dfs_ctx)
{
	DFS_LOG_STATUS(dfs_ctx, "Adding base32 nenc");

	dfs_add_nenc_fast(dfs_ctx, &opz, DFS_NENC_BASE32);
}
