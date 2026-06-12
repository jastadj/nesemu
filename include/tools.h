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
    std::string getYesNo(bool y);
    
    // Data Types
    int toInt(std::string tstr);
    unsigned long long toUnsignedLongLong(std::string str);

    // File Tools
    std::vector<std::string> getFilesAtPath(std::string path);
    bool fileExists(std::string filepath);
    std::string getFilename(std::string filepath);
    std::string getFileExtension(std::string filepath);
    std::string getFileDir(std::string filepath);
}

#endif