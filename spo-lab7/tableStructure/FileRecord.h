//
// Created by Yan Khamutouski on 4/30/18.
//

#ifndef SPO_LAB7_FILERECORD_H
#define SPO_LAB7_FILERECORD_H

#include <string>

using namespace std;

class FileRecord {
    bool busy;
    ///name of file
    char nameOfFile[25];
    ///size of current part of file (may have many parts)
    int sizeOfCurrentPart;
    ///offset in our file system
    long int offset;
    ///it is a first part? is yes - true else false
    bool recordNumber;
    ///flag that inform us if we have another part of file
    bool hasAnotherPart;
    ///number of structure in list (used if flag is set)
    int numberOfNextPart;
public:
    FileRecord();
    FileRecord(const FileRecord &rec);
    ///constructor
    FileRecord(const char *name, int size, long int currentOffset);
    ///getter for name of file
    string getFileName();
    ///getter of file size
    int getSizeOfFile();
    ///setter for another part of file
    void setAnotherPart(int numberOfAnotherPart);
    void setBusyFlag(bool value);
    void setNewFileName(const char *newFileName);
    void setRecordNumber(bool isFirst);
    void setHasAnotherPart(bool flag);
    bool isFirstPart();
    FileRecord &operator=(FileRecord& rec);
    long int getOffset();
    bool getAnotherPartExistence();
    int getNumberOfNextPart();
    bool getBusyFlag();
    void setFileSize(int size);
};

#endif //SPO_LAB7_FILERECORD_H
