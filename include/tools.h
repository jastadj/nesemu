#ifndef TOOLS_H
#define TOOLS_H

#include <vector>
#include <string>

namespace Tools
{
    // String Tools
    std::vector<std::string> split(std::string str, char delimiter = ' ', bool allow_empty = true, bool encapsulate_quotes = true);
    std::string toLower(std::string str);
    std::string toUpper(std::string str);
    
    // Data Types
    int toInt(std::string tstr);
    unsigned long long toUnsignedLongLong(std::string str);

    // File Tools
    bool fileExists(const char* filepath);
}

#endif