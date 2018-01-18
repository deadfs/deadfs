#include <stdarg.h>
#include <stdio.h>

#include "log.h"

#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_RESET   "\x1b[0m"


void dfs_log_status(struct dfs_context *ctx, const char *fn, const char *fmt, ...)
{
	char cfmt[1024];
	va_list args;

	snprintf(cfmt, sizeof(cfmt), "["COLOR_YELLOW"*"COLOR_RESET"] %s(): %s\n", fn, fmt);

	va_start(args, fmt);
	vfprintf(stdout, cfmt, args);
	va_end(args);
}

void dfs_log_error(struct dfs_context *ctx, const char *fn, const char *fmt, ...)
{
	char cfmt[1024];
	va_list args;

	snprintf(cfmt, sizeof(cfmt), "["COLOR_RED"-"COLOR_RESET"] %s(): %s\n", fn, fmt);

	va_start(args, fmt);
	vfprintf(stderr, cfmt, args);
	va_end(args);
}
