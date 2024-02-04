#ifndef __FILESYSTEM_H
#define __FILESYSTEM_H

#include "SPIFFS.h"

class FileSytem
{
public:
    FileSytem();
    ~FileSytem();
    void createFile(const char *fileName);
    void deleteFile(const char *fileName);
    String readFile(const char *fileName);
    void writeFile(const char *fileName, const char *data);
};

#endif