#include "blockfs.h"

#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

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

static int file_init(struct dfs_file *file)
{
	struct filectx *fc = calloc(1, sizeof(struct filectx));
	file->private_data = fc;
	return 0;
}


static void destroy(struct dfs_file *file)
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
	ssize_t cbsize;
	uint64_t cbi = curblocki(file);
	uint32_t cbpos = curblock_pos(file);
	size_t rsize = blocksize - cbpos;
	struct blfs_rawnode *rn = file->node->private_data;
	struct filectx *fc = file->private_data;
	unsigned char *blk = NULL;

	if (rsize > size)
		goto fail_size;

	cbsize = blfs_readblock(file->node->super, rn->blocks[cbi], NULL, 0);
	if (cbsize <= 0)
		goto fail_cbsize;

	blk = malloc(cbsize);

	if (blfs_readblock(file->node->super, rn->blocks[cbi], blk, cbsize) != cbsize)
		goto fail_read;

	memcpy(buf, blk+cbpos, rsize);

	r = rsize;

	fc->pos += rsize;

fail_read:
	free(blk);
fail_cbsize:
fail_size:
	return r;
}

static ssize_t file_read(struct dfs_file *file, unsigned char *buf, size_t size)
{
	ssize_t rsize;
	size_t rtsize = 0;

	while (rtsize < size && (rsize=read_blocksize(file, buf+rtsize, size-rtsize)) > 0) {
		rtsize += rsize;
	}

	return rtsize;
}

static ssize_t write_newblock(struct dfs_file *file, const unsigned char *buf, size_t size)
{
	uint32_t blocksize = file->ctx->config->blocksize;
	uint64_t newblkid = blfs_inc_idctr(file->node->super);
	size_t wsize = MIN(size, blocksize);

	if (blfs_writeblock(file->node->super, newblkid, (void*)buf, wsize) != wsize)
		return -1;

	// TODO: Check for return code
	blfs_add_nodeblock(file->node, newblkid);

	return wsize;
}

// Write update block
static ssize_t write_upblock(struct dfs_file *file, const unsigned char *buf, size_t size)
{
	ssize_t r = -1;
	uint32_t blocksize = file->ctx->config->blocksize;
	uint32_t cbpos = curblock_pos(file);
	size_t maxwrite = blocksize - cbpos;
	size_t wsize = MIN(maxwrite, size);
	size_t nbsize = cbpos + wsize;
	ssize_t cbsize;
	uint64_t cbi = curblocki(file);
	uint64_t blkid;
	struct filectx *fc = file->private_data;
	unsigned char *blk = NULL;


	blkid = blfs_get_nodeblock(file->node, cbi);
	if (!blkid)
		return -1;

	cbsize = blfs_readblock(file->node->super, blkid, NULL, 0);
	if (cbsize <= 0)
		return -1;

	nbsize = MAX(cbsize, cbpos+wsize);
	blk = malloc(nbsize);

	if (blfs_readblock(file->node->super, blkid, blk, cbsize) != cbsize)
		goto fail_read;

	memcpy(blk+cbpos, buf, wsize);

	if (blfs_writeblock(file->node->super, blkid, blk, nbsize) != nbsize)
		goto fail_write;

	if (nbsize > cbsize)
		file->node->size += nbsize - cbsize;

	fc->pos += wsize;

	r = wsize;

fail_write:
fail_read:
	free(blk);
	return r;
}

static ssize_t write_blocksize(struct dfs_file *file, const unsigned char *buf, size_t size)
{
	uint64_t cbi = curblocki(file);
	uint64_t nblocks = blfs_get_nblocks(file->node);

	// We need to create another block
	if (cbi == nblocks)
		return write_newblock(file, buf, size);
	else
		return write_upblock(file, buf, size);
}

static ssize_t file_write(struct dfs_file *file, const unsigned char *buf, size_t size)
{
	ssize_t wsize;
	size_t wtsize = 0;

	while (wtsize < size && (wsize=write_blocksize(file, buf+wtsize, size-wtsize)) > 0) {
		wtsize += wsize;
	}

	return wtsize;
}


const struct dfs_fileops blfs_fops = {
		.init = file_init,
		.destroy = destroy,
		.seek = seek,
		.read = file_read,
		.write = file_write
};
