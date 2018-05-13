//
// Created by Yan Khamutouski on 5/1/18.
//

#include "FileManager.h"

FileManager::FileManager(string nameOfFile) {
    this->nameOfFileSystem = nameOfFile;
    ///we will create it
    if (!this->createFileSystem())
        terminate();
}

void FileManager::fillAllFilesTable() {
    ///reading quantity of records
    this->moveReadPointer(0);
    this->moveWritePointer(0);
//    int z = 0;
//    filesystem.write(reinterpret_cast<char*>(&z), sizeof(z));
//    this->filler(0);
    filesystem.read(reinterpret_cast<char*>(&countOfRecords), sizeof(int));
    while(this->countOfRecords){
        FileRecord record = this->readRecord();
        if (!record.getBusyFlag()) {
            this->freeCells.insert(pair<int, int>(record.getSizeOfFile(), allFilesTable.size()));
        }
        allFilesTable.push_back(record);
        countOfRecords--;
    }
}

bool FileManager::createFileSystem() {
    ///creating FS file
    filesystem.open("virtual_file_system", ios::in | ios::out | ios::ate | ios::binary);
    if (filesystem.is_open()) return true;
    else return false;
}

bool FileManager::ls() {

    ///cout fileName and its size
    if (allFilesTable.size() == 0) {
        return false;
    }
    map<string, int> listing;
    for (auto it = allFilesTable.begin(); it != allFilesTable.end(); it++)
        if ((*it).isFirstPart() && (*it).getBusyFlag())
            listing.insert(pair((*it).getFileName(), (*it).getSizeOfFile()));
    for (auto it = allFilesTable.begin(); it != allFilesTable.end(); it++) {
        if ((*it).getAnotherPartExistence()) {
            //поиск по map и модификация размера
            auto mapIter = listing.find((*it).getFileName());
            (*mapIter).second += (*it).getSizeOfFile();
        }
    }
    cout << setw(25) << "Name of file:" <<
         setw(10) << "Size:" << endl;
//    for (auto it = allFilesTable.begin();
//            it != allFilesTable.end(); it++) {
//        if ((*it).isFirstPart() && (*it).getBusyFlag()) {
//            cout << setw(25) << (*it).getFileName()
//                 << setw(10) << (*it).getSizeOfFile() << endl;
//        }
//}
    for (auto it = listing.begin(); it != listing.end(); it++) {
        cout << setw(25) << (*it).first
             << setw(15) << (*it).second << endl;
    }
    listing.clear();
}

bool FileManager::create() {
    ///creating a file in our FS
    if (allFilesTable.size() >= 20) {
        cout << "File system has no possibility to append another file." << endl;
        return false;
    }
    string dataToWrite;
    string name;
    while(true) {
        cout << "Input name of file (25 symbols max):";
        cin >> name;
        if (name.length() <= 24) break;
    }
    cout << "Input data to write:";
    cin >> dataToWrite;
    int sizeOfInputedData = dataToWrite.length() + 1;


    int numberOfFreeCell = -1;
    for (auto it = freeCells.begin(); it != freeCells.end(); it++) {
        if (it->first >= sizeOfInputedData)
            numberOfFreeCell = it->second;
            freeCells.erase(it);
            break;
    }
    long int currOffset;
    if (numberOfFreeCell >= 0) {
        allFilesTable[numberOfFreeCell].setBusyFlag(true);
        currOffset = allFilesTable[numberOfFreeCell].getOffset();
        allFilesTable[numberOfFreeCell].setNewFileName(name.c_str());
        allFilesTable[numberOfFreeCell].setRecordNumber(true);
        allFilesTable[numberOfFreeCell].setFileSize(sizeOfInputedData);
        this->moveWritePointer(currOffset);
        int size = sizeOfInputedData;
        char buffer[sizeOfInputedData];
        strcpy(buffer, dataToWrite.c_str());
        this->filesystem.write(buffer, sizeOfInputedData);
        return true;
    }
    if (allFilesTable.empty()) {
        currOffset = offsetBorderForFilesTable;
    } else {
        currOffset = allFilesTable.back().getOffset()
                     + allFilesTable.back().getSizeOfFile();
    }
    FileRecord newRec(name.c_str(), sizeOfInputedData, currOffset);

    this->moveWritePointer(currOffset);
    if (!filesystem.good())
        filesystem.clear();

    char buffer[sizeOfInputedData];
    strcpy(buffer, dataToWrite.c_str());

    filesystem.write(buffer, sizeOfInputedData);

    allFilesTable.push_back(newRec);
    this->refresh();
    return true;
}

bool FileManager::del(string file) {
    ///searching in our collection and modify flag BUSY to FALSE
    if (allFilesTable.empty())
        return false;

    for (int i = 0; i < allFilesTable.size(); i++){
        if (this->allFilesTable[i].getFileName() == file) {
            this->allFilesTable[i].setBusyFlag(false);
        }
    }

}

bool FileManager::open(string file) {
    ///opening a file in our FS by searching in our FS
    /// (1st record, if it have filled row HAS_ANOTHER_PART searching
    /// for all)
    if (allFilesTable.empty()) return false;
    bool flag = false;
    int numberOfRecordInTable;
    cout << endl << "Content of file: " << file << ":"<< endl;
    for (auto it = allFilesTable.begin(); it != allFilesTable.end(); it++) {
        if ((*it).getFileName() == file && (*it).getBusyFlag()){
            if ((*it).isFirstPart()) {
                flag = (*it).getAnotherPartExistence();
                int size = (*it).getSizeOfFile();
                char content[size];
                this->moveReadPointer((*it).getOffset());
                this->filesystem.read(content, size);
                this->moveReadPointer(0);
                this->moveWritePointer(0);
                string contentStr(content);
                cout << contentStr;
                if (flag)
                    numberOfRecordInTable = (*it).getNumberOfNextPart();
                break;
            }
        }
    }
    while(flag) {
        if (numberOfRecordInTable > allFilesTable.size())
            return false;
        long int offset;
        offset = allFilesTable[numberOfRecordInTable].getOffset();
        this->moveReadPointer(offset);
        int size = allFilesTable[numberOfRecordInTable].getSizeOfFile();
        char content[size];
        this->filesystem.read(reinterpret_cast<char*>(&content), size);
        string contentString(content);
        fflush(stdin);
        cout << " " << contentString;
        if (allFilesTable[numberOfRecordInTable].getAnotherPartExistence()) {
            flag = true;
            int temp = allFilesTable[numberOfRecordInTable].getNumberOfNextPart();
            numberOfRecordInTable = temp;
        }
        else flag = false;
    }
    return true;
}

bool FileManager::rename(string oldName, string newName) {
    ///searching in collection and renaming
    if (newName.length() >= 25) {
        cout << "Too long name. Max - 24 sym" << endl;
        return false;
    }
    for (auto it = allFilesTable.begin(); it != allFilesTable.end(); it++) {
        if ((*it).getFileName() == oldName)
            (*it).setNewFileName(newName.c_str());
    }
    return true;
}

FileRecord FileManager::readRecord() {
    FileRecord record;
    filesystem.read(reinterpret_cast<char*>(&record), sizeof(FileRecord));
    return record;
}

void FileManager::writeRecord(FileRecord record) {
    filesystem.write(reinterpret_cast<char*>(&record), sizeof(FileRecord));
}

int FileManager::append(string toFile) {
    cout << "Input information to append:" << endl;
    string information;
    fflush(stdin);
    cin >> information;
    int length = information.length() + 1;
    int numberInTable = -1;
    for (auto it = freeCells.begin(); it != freeCells.end(); it++) {
        if ((*it).first >= information.length()) {
            numberInTable = (*it).second;
            freeCells.erase(it);
        }
    }
    if (numberInTable != -1) {
        char buffer[length];
        strcpy(buffer, information.c_str());
        allFilesTable[numberInTable].setNewFileName(toFile.c_str());
        allFilesTable[numberInTable].setBusyFlag(true);
        allFilesTable[numberInTable].setFileSize(information.length());
        allFilesTable[numberInTable].setRecordNumber(false);
        this->moveWritePointer(allFilesTable[numberInTable].getOffset());
        this->filesystem.write(buffer, length);
        this->refresh();
    }
    else {
        char buffer[length];
        strcpy(buffer, information.c_str());
        long offset = allFilesTable.back().getOffset() + allFilesTable.back().getSizeOfFile();
        this->moveWritePointer(offset);
        this->filesystem.write(buffer, length);
        this->refresh();
    }
    FileRecord newRecord(toFile.c_str(), length,
                         allFilesTable.back().getOffset()
                         + allFilesTable.back().getSizeOfFile()
    );
    newRecord.setRecordNumber(false);
    allFilesTable.push_back(newRecord);
    int num = allFilesTable.size();
    num -= 1;
    return num;
}

void FileManager::moveReadPointer(long int offset) {
    ///moving a read pointer
    this->filesystem.seekg(offset, ios::beg);
}

void FileManager::moveWritePointer(long int offset) {
    ///moving a write pointer
    this->filesystem.seekp(offset, ios::beg);
}

void FileManager::writeCountOfElements(int count) {
    this->filesystem.seekp(ios::beg);
    this->filesystem.seekg(ios::beg);
    this->filesystem.write(reinterpret_cast<char*>(&count), sizeof(count));
}

long int FileManager::getCurrentWritePosInFileSystem() {
    return this->filesystem.tellp();
}

long int FileManager::getCurrentReadPosInFileSystem() {
    return this->filesystem.tellg();
}

string FileManager::readFrom(FileRecord record) {
    string information;
    this->moveReadPointer(record.getOffset());
    this->filesystem.read(reinterpret_cast<char*>(&information), record.getSizeOfFile());
    return information;
}

void FileManager::quit() {
    int count = this->allFilesTable.size();
    this->writeCountOfElements(count);
    for (auto it = allFilesTable.begin();
            it != allFilesTable.end(); it++) {
        ///writing our
        this->writeRecord((*it));
    }
}

bool FileManager::appendInfo(string toFile) {
    if (allFilesTable.empty()) return false;
    if (allFilesTable.size() >= 20) return false;
    for (auto it = allFilesTable.begin(); it != allFilesTable.end(); it++) {
        if ((*it).getFileName() == toFile && !(*it).getAnotherPartExistence()) {
            (*it).setHasAnotherPart(true);
            int index = it - allFilesTable.begin();
            int num = append(toFile);
            allFilesTable[index].setAnotherPart(num);
            break;
        }
    }
    return true;
}

void FileManager::filler(int howMuchWeHaveFilled) {
    int count = 20;
    long int offset = sizeof(int);    ///start pos for write
    count -= howMuchWeHaveFilled;
    FileRecord emptyRecord("empty", 0, 0);
    while(count--)
        filesystem.write(reinterpret_cast<char*>(&emptyRecord), sizeof(FileRecord));
}

void FileManager::refresh() {
    fpos put = filesystem.tellp();
    fpos get = filesystem.tellg();
    filesystem.close();
    filesystem.open("virtual_file_system", ios::in | ios::out | ios::ate | ios::binary);
    this->moveWritePointer(put);
    this->moveReadPointer(get);
}


