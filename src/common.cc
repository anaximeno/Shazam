#include "../include/shazam/common.hh"

#include <string>
#include <algorithm>
#include <memory>
#include <iostream>

int shazam::hexaToInt(std::string hexadecimalString)
{
    return std::stoi(hexadecimalString, 0, 16);
}

std::string shazam::toUpperCase(std::string str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
}

std::string shazam::toLowerCase(std::string str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

void shazam::printErrMessage(const std::string& message)
{
    std::cerr << "Shazam: Err: " << message << std::endl;
    std::exit(1);
}
