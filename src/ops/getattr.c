#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>


#include "../deadfs.h"
#include "../path.h"
#include "../err.h"

int dfs_getattr(struct dfs_context *ctx, const char *vpath, struct stat *st)
{
	int r = DFS_ERR_GENERIC, rr;
	struct dfs_file *file = NULL;

	memset(st, 0, sizeof(*st));

	rr = dfs_open_file(ctx, vpath, &file);
	if (rr < 0) {
		r = rr;
		goto fail_open;
	}

	*st = file->st;
	st->st_uid = getuid();
	st->st_gid = getgid();
	// TODO: Hide metadata (like times)!
	st->st_size = file->size;


	r = 0;
	dfs_close_file(file);
fail_open:
	return r;
}
