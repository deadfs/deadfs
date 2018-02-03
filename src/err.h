#ifndef SRC_ERR_H_
#define SRC_ERR_H_

#include <limits.h>

enum {
	DFS_ERR_GENERIC = INT_MIN,
	DFS_ERR_BADVALUE,
	DFS_ERR_DENIED,
	DFS_ERR_ALOPEN, // Already opened
	DFS_ERR_NOSPACE,
	DFS_ERR_NOENT,
	DFS_ERR_PARTIAL,
	DFS_ERR_CORRUPTED
};

#endif /* SRC_ERR_H_ */
