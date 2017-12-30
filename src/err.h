#ifndef SRC_ERR_H_
#define SRC_ERR_H_

#include <limits.h>

enum {
	DFS_ERR_GENERIC = INT_MIN,
	DFS_ERR_DENIED,
	DFS_ERR_NOSPACE,
	DFS_ERR_NOENT
};

#endif /* SRC_ERR_H_ */
