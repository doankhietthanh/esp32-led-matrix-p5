#include "FileSystem.h"

FileSytem::FileSytem()
{
}

FileSytem::~FileSytem()
{
}

void FileSytem::createFile(const char *fileName)
{
    File file = SPIFFS.open(fileName, FILE_WRITE);
    if (!file)
    {
        Serial.println("There was an error opening the file for writing");
        return;
    }
    file.close();
}

void FileSytem::deleteFile(const char *fileName)
{
    SPIFFS.remove(fileName);
}

void FileSytem::writeFile(const char *fileName, const char *data)
{
    File file = SPIFFS.open(fileName, FILE_WRITE);
    if (!file)
    {
        Serial.println("There was an error opening the file for writing");
        return;
    }
    if (file.print(data))
    {
        Serial.println("File was written");
    }
    else
    {
        Serial.println("File write failed");
    }
    file.close();
}

String FileSytem::readFile(const char *fileName)
{
    File file = SPIFFS.open(fileName);
    if (!file || file.isDirectory())
    {
        Serial.println("There was an error opening the file for reading");
        return "";
    }
    String fileContent;
    while (file.available())
    {
        fileContent = file.readStringUntil('\n');
        break;
    }
    file.close();
    return fileContent;
}