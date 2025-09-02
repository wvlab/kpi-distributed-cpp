#ifdef _WIN32
#include "src/fetchbase_win.c"
#else
#ifdef __linux__
#include "src/fetchbase_linux.c"
#else
#error "incompatible target"
#endif // #ifdef __linux__
#endif // #ifdef _WIN32
