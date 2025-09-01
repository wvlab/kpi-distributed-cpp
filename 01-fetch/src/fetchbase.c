#ifdef _WIN32
    #include "src/fetchbase_win.c"
#elif __unix__
    #include "src/fetchbase_unix.c"
#endif
