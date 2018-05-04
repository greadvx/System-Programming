//
// Created by Yan Khamutouski on 5/1/18.
//

#ifndef SPO_LAB7_FILEMANAGER_H
#define SPO_LAB7_FILEMANAGER_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <map>


#include "../tableStructure/FileRecord.h"

///maximal count of files in FS
const int MAX_COUNT_OF_FILES = 20;

const long int offsetBorderForFilesTable = sizeof(FileRecord)
        * MAX_COUNT_OF_FILES + sizeof(int);
const long int offsetStartForFilesTable = sizeof(int) + 1;

using namespace std;

class FileManager {
    ///stream of file I/O
    fstream filesystem;
    ///name of file where stored our filesystem
    string nameOfFileSystem;
    ///current count of records (how much need to read)
    int countOfRecords;
    ///count of cells that we can reuse
    ///key - size, value - number in vector.
    map<int, int > freeCells;
    ///collection of files
    vector<FileRecord> allFilesTable;
    /// open/create FS
    bool createFileSystem();
    ///appending info in file return number of record in vector (model 1-2-1(added part))
    int append(string toFile);
    void moveReadPointer(long int offset);
    void moveWritePointer(long int offset);
    void writeCountOfElements(int count);
    string readFrom(FileRecord record);

public:
    ///constructor of fileManager
    FileManager(string nameOfFile);
    ///filling the collection of
    void fillAllFilesTable();
    ///printing our vector(only names of file and its size)
    bool ls();
    ///creating a file in our FS
    bool create();
    ///deleting a file in our FS
    bool del(string file);
    ///opening a file in our FS
    bool open(string file);
    ///renaming a file
    bool rename(string newName, string oldName);
    bool appendInfo(string toFile);
    FileRecord readRecord();
    void writeRecord(FileRecord record);
    long int getCurrentWritePosInFileSystem();
    long int getCurrentReadPosInFileSystem();
    void quit();
};

#endif //SPO_LAB7_FILEMANAGER_H
