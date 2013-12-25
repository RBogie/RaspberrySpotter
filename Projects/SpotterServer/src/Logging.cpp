#include <cstdio>
#include <cstdarg>

#include "GlobalDefines.hpp"

void logError(const char *format, ...) {
	va_list argptr;
	va_start(argptr, format);
	vfprintf(stderr, format, argptr);
	fprintf(stderr, "\n");
}

void logDebug(const char *format, ...) {
	if (!DEBUG)
		return;

	va_list argptr;
	va_start(argptr, format);
	vfprintf(stdout, format, argptr);
	printf("\n");
}
