#ifndef SRC_UTILS_FILE_H_
#define SRC_UTILS_FILE_H_


ssize_t dfs_readfile(const char *path, unsigned char *data, size_t len);
ssize_t dfs_writefile(const char *path, const unsigned char *data, size_t len);



#endif /* SRC_UTILS_FILE_H_ */
