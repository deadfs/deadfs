#include <unistd.h>
#include <fcntl.h>

#include "ops.h"
#include "../err.h"
#include "../path.h"

int dfs_open(struct dfs_context *ctx, const char *vpath, unsigned int flags, struct dfs_file **retfile)
{
	int r = DFS_ERR_GENERIC;
	char *appath = NULL;
	const char *mode = NULL;
	FILE *fp = NULL;
	unsigned int accmode = 0;
	struct dfs_file *file = NULL;

	appath = dfs_path_vtoap_dup(ctx, vpath);

	accmode = flags & O_ACCMODE;

	if (accmode == O_RDONLY)
		mode = "rb";
	else if (accmode == O_WRONLY || accmode == O_RDWR)
		mode = "rb+";
	else
		mode = "rb+";

	fp = fopen(appath, mode);
	if (!fp)
		goto fail_fopen;

	file = dfs_add_file_fast(ctx, fp, vpath, 0, 0, 0);

	if (retfile)
		*retfile = file;

	r = 0;

fail_fopen:
	free(appath);
	return r;
}
