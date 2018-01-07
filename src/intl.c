#include <unistd.h>

#include "intl.h"
#include "err.h"


static size_t getfilesize(const char* path)
{
	struct stat st;

	if(stat(path, &st) != 0)
		return 0;

	return st.st_size;
}

static int readblockids(FILE *fp, uint64_t nb, uint64_t *blockids)
{
	uint64_t i;

	for (i=0; i<nb; i++) {
		if (fread(&blockids[i], sizeof(uint64_t), 1, fp) != 1)
			return -1;
	}

	return 0;
}

int dfs_readfile(struct dfs_context *ctx, const char *path, struct dfs_filehdr *hdr, uint64_t **blockids)
{
	int r = DFS_ERR_GENERIC;
	FILE *fp = NULL;
	size_t filesize = 0;

	*blockids = NULL;

	if (access(path, F_OK) != 0) {
		r = DFS_ERR_NOENT;
		goto fail_access;
	}

	filesize = getfilesize(path);
	if (filesize < sizeof(struct dfs_filehdr)) {
		r = DFS_ERR_CORRUPTED;
		goto fail_size;
	}

	fp = fopen(path, "rb");
	if (!fp) {
		r = DFS_ERR_DENIED;
		goto fail_fopen;
	}

	if (fread(hdr, sizeof(struct dfs_filehdr), 1, fp) != 1) {
		r = DFS_ERR_CORRUPTED;
		goto fail_fread;
	}

	// TODO: Add check hdr->size ~ matching hdr->nb * blocksize

	if (hdr->nb * sizeof(uint64_t) != filesize - sizeof(struct dfs_filehdr)) {
		r = DFS_ERR_CORRUPTED;
		goto fail_nb;
	}

	*blockids = malloc(hdr->nb * sizeof(uint64_t));

	if (readblockids(fp, hdr->nb, *blockids) != 0) {
		free(*blockids);
		*blockids = NULL;
		r = DFS_ERR_CORRUPTED;

		goto fail_rbi;
	}

	r = 0;

fail_rbi:
fail_nb:
fail_fread:
	fclose(fp);
fail_size:
fail_fopen:
fail_access:
	return r;
}

static int writeblockids(FILE *fp, uint64_t nb, const uint64_t *blockids)
{
	uint64_t i;

	for (i=0; i<nb; i++) {
		if (fwrite(&blockids[i], sizeof(uint64_t), 1, fp) != 1)
			return -1;
	}
	return 0;
}

int dfs_writefile(struct dfs_context *ctx, const char *path, const struct dfs_filehdr *hdr, const uint64_t *blockids)
{
	int r = DFS_ERR_GENERIC;
	FILE *fp = NULL;

	fp = fopen(path, "wb");
	if (!fp) {
		r = DFS_ERR_DENIED;
		goto fail_fopen;
	}

	// TODO: Add check hdr->size ~ matching hdr->nb * blocksize

	if (fwrite(hdr, sizeof(struct dfs_filehdr), 1, fp) != 1) {
		r = DFS_ERR_CORRUPTED;
		goto fail_fwrite;
	}

	if (writeblockids(fp, hdr->nb, blockids) != 0) {
		r = DFS_ERR_CORRUPTED;
		goto fail_wbi;
	}

	r = 0;
fail_wbi:
fail_fwrite:
	fclose(fp);
fail_fopen:
	return r;
}
