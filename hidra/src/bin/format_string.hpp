#pragma once

#ifdef _WIN32
    #define NEWLINE "\r\n"
#elif defined macintosh
    #define NEWLINE "\r"
#else
    #define NEWLINE "\n"
#endif