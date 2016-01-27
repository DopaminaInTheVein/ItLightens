#include "mcv_platform.h"
#include <windows.h>

#ifndef NDEBUG

void dbg(const char* format, ...) {
	va_list argptr;
	va_start(argptr, format);
	char dest[1024 * 16];
	_vsnprintf(dest, sizeof(dest), format, argptr);
	va_end(argptr);
  ::OutputDebugString(dest);
}
#endif

bool fatal(const char* format, ...) {
  va_list argptr;
  va_start(argptr, format);
  char dest[1024 * 16];
  _vsnprintf(dest, sizeof(dest), format, argptr);
  va_end(argptr);
  ::OutputDebugString("FATAL!!!\n");
  ::OutputDebugString(dest);
  return false;
}


