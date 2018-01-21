#include "blockfs.h"


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
	ssize_t cbsize;
	uint64_t cbi = curblocki(file);
	uint32_t cbpos = curblock_pos(file);
	size_t rsize = blocksize - cbpos;
	struct blfs_rawnode *rn = file->node->private_data;
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
	struct filectx *fc = file->private_data;

	while (rtsize < size && (rsize=read_blocksize(file, buf+rtsize, size-rtsize)) > 0) {
		rtsize += rsize;
		fc->pos += rtsize;
	}

	return rtsize;
}

static ssize_t write_newblock(struct dfs_file *file, const unsigned char *buf, size_t size)
{
	uint64_t newblkid = blfs_inc_idctr(file->node->super);

	if (blfs_writeblock(file->node->super, newblkid, (void*)buf, size) != size)
		return -1;

	// TODO: Check for return code
	blfs_add_nodeblock(file->node, newblkid);

	return size;
}

static ssize_t write_blocksize(struct dfs_file *file, const unsigned char *buf, size_t size)
{
	ssize_t r = -1;
	uint32_t blocksize = file->ctx->config->blocksize;
	uint64_t cbi = curblocki(file);
	uint32_t cbpos = curblock_pos(file);
	ssize_t cbsize;
	size_t wsize = MIN(size, blocksize)-cbpos;
	uint64_t blkid;
	uint64_t nblocks = blfs_get_nblocks(file->node);
	unsigned char *blk = NULL;

	// We need to create another block
	if (cbi == nblocks)
		return write_newblock(file, buf, wsize);

	blkid = blfs_get_nodeblock(file->node, cbi);
	if (!blkid)
		return -1;

	cbsize = blfs_readblock(file->node->super, blkid, NULL, 0);
	if (cbsize <= 0)
		goto fail_cbsize;

	blk = malloc(blocksize);

	if (blfs_readblock(file->node->super, blkid, blk, cbsize) != cbsize)
		goto fail_read;

	// TODO: Check if wsize is greater than cbsize and eventually increase file->node->size
	memcpy(blk+cbpos, buf, wsize);

	if (blfs_writeblock(file->node->super, blkid, blk, cbpos+wsize) != cbpos+wsize)
		goto fail_write;

	r = wsize;

fail_write:
fail_read:
	free(blk);
fail_cbsize:
	return r;
}

static ssize_t file_write(struct dfs_file *file, const unsigned char *buf, size_t size)
{
	ssize_t wsize;
	size_t wtsize = 0;
	struct filectx *fc = file->private_data;

	while (wtsize < size && (wsize=write_blocksize(file, buf+wtsize, size-wtsize)) > 0) {
		wtsize += wsize;
		fc->pos += wtsize;
	}

	return wtsize;
}


const struct dfs_fileops blfs_fops = {
		.open = file_open,
		.release = release,
		.seek = seek,
		.read = file_read,
		.write = file_write
};
