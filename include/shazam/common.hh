#ifndef _SHAZAM_COMMON_HEADER
#define _SHAZAM_COMMON_HEADER

#include <string>

namespace shazam {
    /* Converts and hexadecimal value to integer. */
    int hexaToInt(std::string hexadecimalString);

    /* Returns the input str as an uppercase output. */
    std::string toUpperCase(std::string str);

    /* Returns the input str as an lowercase output. */
    std::string toLowerCase(std::string str);

    /* Prints an error message and exits. */
    void printErrMessage(const std::string& message);
};

#endif /* _SHAZAM_COMMON_HEADER */
