#if defined(_WIN32)
#include "src/fetchbase_win.cpp"
#elif defined(__linux__)
#include "src/fetchbase_linux.cpp"
#else
#error "incompatible target"
#endif
