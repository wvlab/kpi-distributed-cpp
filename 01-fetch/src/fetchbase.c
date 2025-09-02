#if defined(_WIN32)
#include "src/fetchbase_win.c"
#elif defined(__linux__)
#include "src/fetchbase_linux.c"
#else
#error "incompatible target"
#endif
