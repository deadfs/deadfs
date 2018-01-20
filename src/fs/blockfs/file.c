#include "blockfs.h"


struct filectx {
	off_t pos;
};

static off_t seekset(struct dfs_file *file, off_t offset)
{
	struct filectx *fc = file->private_data;

	if (offset > file->node->size)
		return -1;

	fc->pos = offset;

	return fc->pos;
}

static off_t seekcur(struct dfs_file *file, off_t offset)
{
	struct filectx *fc = file->private_data;

	if (fc->pos + offset > file->node->size)
		return -1;

	fc->pos += offset;

	return fc->pos;
}

static off_t seekend(struct dfs_file *file, off_t offset)
{
	struct filectx *fc = file->private_data;

	if (offset > file->node->size)
		return -1;

	fc->pos = file->node->size - offset;

	return fc->pos;
}

static off_t seek(struct dfs_file *file, off_t offset, int whence)
{
	switch (whence) {
	case SEEK_SET: return seekset(file, offset);
	case SEEK_CUR: return seekcur(file, offset);
	case SEEK_END: return seekend(file, offset);
	default: return -1;
	}
}

static int file_open(struct dfs_file *file)
{
	struct filectx *fc = calloc(1, sizeof(struct filectx));
	file->private_data = fc;
	return 0;
}


static void release(struct dfs_file *file)
{
	if (!file)
		return;

	free(file->private_data);
}

// Returns the current index of the block array in the rawnode structure
static uint64_t curblocki(struct dfs_file *file)
{
	struct filectx *fc = file->private_data;

	// TODO:			L O L
	return fc->pos / file->ctx->config->blocksize;
}

// Return the current position inside the current block
static uint32_t curblock_pos(struct dfs_file *file)
{
	struct filectx *fc = file->private_data;

	// TODO:			L O L
	return fc->pos % file->ctx->config->blocksize;
}

static ssize_t read_blocksize(struct dfs_file *file, unsigned char *buf, size_t size)
{
	ssize_t r = -1;
	uint32_t blocksize = file->ctx->config->blocksize;
	uint64_t cbi = curblocki(file);
	uint32_t cbpos = curblock_pos(file);
	size_t rsize = blocksize - cbpos;
	struct blfs_rawnode *rn = file->node->private_data;
	unsigned char *blk = NULL;

	if (rsize > size)
		goto fail_size;

	blk = malloc(blocksize);

	if (blfs_readblock(file->node->super, rn->blocks[cbi], blk, blocksize) != blocksize)
		goto fail_read;

	memcpy(buf, blk+cbpos, rsize);

	r = rsize;

fail_read:
	free(blk);
fail_size:
	return r;
}

static ssize_t file_read(struct dfs_file *file, unsigned char *buf, size_t size)
{
	ssize_t rsize;
	size_t rtsize = 0;

	if (size > file->node->size)
		return -1;

	while (rtsize < size && (rsize=read_blocksize(file, buf+rtsize, size-rtsize)) > 0) {
		rtsize += rsize;
	}

	return rtsize;
}


const struct dfs_fileops blfs_fops = {
		.open = file_open,
		.release = release,
		.seek = seek,
		.read = file_read
};
