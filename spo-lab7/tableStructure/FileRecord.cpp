//
// Created by Yan Khamutouski on 4/30/18.
//

#include <ostream>
#include "FileRecord.h"

FileRecord::FileRecord(const char *name, int size, long int currentOffset){

    strcpy(this->nameOfFile,name);
    this->sizeOfCurrentPart = size;
    this->offset = currentOffset;
    this->hasAnotherPart = false;
    this->numberOfNextPart = -1;
    this->setBusyFlag(true);
    this->recordNumber = true;
}
string FileRecord::getFileName() {
    string temp(this->nameOfFile);
    return temp;
}
int FileRecord::getSizeOfFile() {
    return this->sizeOfCurrentPart;
}
void FileRecord::setAnotherPart(int numberOfAnotherPart) {
    this->hasAnotherPart = true;
    this->numberOfNextPart = numberOfAnotherPart;
}

void FileRecord::setBusyFlag(bool value) {
    this->busy = value;
}

void FileRecord::setNewFileName(const char *newFileName) {
    strcpy(this->nameOfFile, newFileName);
}

FileRecord::FileRecord() {
    this->busy = false;
    strcpy(this->nameOfFile, "empty");
    this->sizeOfCurrentPart = 0;
    this->offset = 0;
    this->hasAnotherPart = false;
    this->numberOfNextPart = 0;
    this->recordNumber = false;
}

FileRecord &FileRecord::operator=(FileRecord &rec) {
    strcpy(this->nameOfFile,rec.nameOfFile);
    this->numberOfNextPart = rec.numberOfNextPart;
    this->hasAnotherPart = rec.hasAnotherPart;
    this->offset = rec.offset;
    this->sizeOfCurrentPart = rec.sizeOfCurrentPart;
    this->busy = rec.busy;
    this->recordNumber = rec.recordNumber;
}

FileRecord::FileRecord(const FileRecord &rec) {
    strcpy(this->nameOfFile,rec.nameOfFile);
    this->numberOfNextPart = rec.numberOfNextPart;
    this->hasAnotherPart = rec.hasAnotherPart;
    this->offset = rec.offset;
    this->sizeOfCurrentPart = rec.sizeOfCurrentPart;
    this->busy = rec.busy;
    this->recordNumber = rec.recordNumber;
}

void FileRecord::setRecordNumber(bool isFirst) {
    this->recordNumber = isFirst;
}

bool FileRecord::isFirstPart() {
    return this->recordNumber;
}

long FileRecord::getOffset() {
    return this->offset;
}

bool FileRecord::getAnotherPartExistence() {
    return this->hasAnotherPart;
}

int FileRecord::getNumberOfNextPart() {
    return this->numberOfNextPart;
}

bool FileRecord::getBusyFlag() {
    return this->busy;
}

void FileRecord::setFileSize(int size) {
    this->sizeOfCurrentPart = size;
}

void FileRecord::setHasAnotherPart(bool flag) {
    this->hasAnotherPart = flag;
}
