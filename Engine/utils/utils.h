#ifndef INC_UTILS_H_
#define INC_UTILS_H_

#ifdef NDEBUG

#define dbg(...)  

#else

void dbg(const char* fmt, ...);

#endif

bool fatal(const char* fmt, ...);

#endif

