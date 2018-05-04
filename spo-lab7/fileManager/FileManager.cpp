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
    //TODO: append info in free cells
    //this->moveReadPointer(0);
//    int z = 10;
//    filesystem.write(reinterpret_cast<char*>(&z), sizeof(z));
    filesystem.seekp(ios::beg);
    filesystem.seekg(ios::beg);
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
    filesystem.open("virtual_file_system", ios::in | ios::out | ios::app | ios::binary);
    if (filesystem.is_open()) return true;
    else return false;
}

bool FileManager::ls() {
    ///cout fileName and its size
    if (allFilesTable.size() == 0) {
        return false;
    }

    for (auto it = allFilesTable.begin();
            it != allFilesTable.end(); it++) {
        if ((*it).isFirstPart()) {
            cout << setw(25) << (*it).getFileName()
                 << setw(10) << (*it).getSizeOfFile() << endl;
        }
    }

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
        if (name.length() <=24) break;
    }
    cout << "Input data to write:";
    cin >> dataToWrite;
    int sizeOfInputedData = dataToWrite.length() + 1;


    int numberOfFreeCell = -1;
    for (auto it = freeCells.begin(); it != freeCells.end(); it++) {
        if (it->first >= sizeOfInputedData)
            numberOfFreeCell = it->second;
            freeCells.erase(it);
    }
    long int currOffset;
    if (numberOfFreeCell >= 0) {
        allFilesTable[numberOfFreeCell].setBusyFlag(true);
        currOffset = allFilesTable[numberOfFreeCell].getOffset();
        this->moveWritePointer(currOffset);
        allFilesTable[numberOfFreeCell].setNewFileName(name.c_str());
        allFilesTable[numberOfFreeCell].setRecordNumber(true);
        allFilesTable[numberOfFreeCell].setFileSize(sizeOfInputedData);
        this->filesystem.write(dataToWrite.c_str(), sizeOfInputedData);
        return true;
    }
    if (allFilesTable.empty()) {
        currOffset = offsetBorderForFilesTable + 1;
    } else {
        ///Тут добавить вариант поиска для записи в уже существовавшую но удаленную ячейку
        currOffset = allFilesTable.back().getOffset()
                     + allFilesTable.back().getSizeOfFile() + 1;
    }
    FileRecord newRec(name.c_str(), sizeOfInputedData, currOffset);
    allFilesTable.push_back(newRec);
    return true;
}

bool FileManager::del(string file) {
    ///searching in our collection and modify flag BUSY to FALSE
    if (allFilesTable.empty())
        return false;

    for (int i = 0; i < allFilesTable.size(); i++){
        if (this->allFilesTable[i].getFileName() == file)
            this->allFilesTable[i].setBusyFlag(false);
    }

}

bool FileManager::open(string file) {
    ///opening a file in our FS by searching in our FS
    /// (1st record, if it have filled row HAS_ANOTHER_PART searching
    /// for all)
    if (allFilesTable.empty()) return false;
    string content;
    bool flag = false;
    int numberOfRecordInTable;
    cout << endl << "Content of file: " << file << ":"<< endl;
    for (auto it = allFilesTable.begin(); it != allFilesTable.end(); it++) {
        if ((*it).getFileName() == file){
            if ((*it).isFirstPart()) {
                flag = (*it).getAnotherPartExistence();
                this->moveReadPointer((*it).getOffset());
                this->filesystem.read(reinterpret_cast<char*>(&content), (*it).getSizeOfFile());
                cout << content;
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
        this->filesystem.read(reinterpret_cast<char*>(&content),
                              allFilesTable[numberOfRecordInTable].getSizeOfFile());
        cout << " " << content;
        if (allFilesTable[numberOfRecordInTable].getAnotherPartExistence()) {
            flag = true;
            int temp = allFilesTable[numberOfRecordInTable].getNumberOfNextPart();
            numberOfRecordInTable = temp;
        }
        else flag = false;
    }
    return true;
}

bool FileManager::rename(string newName, string oldName) {
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

    int numberInTable = -1;
    for (auto it = freeCells.begin(); it != freeCells.end(); it++) {
        if ((*it).first >= information.length() + 1) {
            numberInTable = (*it).second;
            freeCells.erase(it);
        }
    }
    if (numberInTable != -1) {
        allFilesTable[numberInTable].setNewFileName(toFile.c_str());
        allFilesTable[numberInTable].setBusyFlag(true);
        allFilesTable[numberInTable].setFileSize(information.length() + 1);
        allFilesTable[numberInTable].setRecordNumber(false);
        this->moveWritePointer(allFilesTable[numberInTable].getOffset());
        this->filesystem.write(information.c_str(), information.length() + 1);
    }

    FileRecord newRecord(toFile.c_str(), sizeof(information),
                         allFilesTable.back().getOffset()
                         + allFilesTable.back().getSizeOfFile()
                         + 1
    );
    newRecord.setRecordNumber(false);
    allFilesTable.push_back(newRecord);
    //посмотреть на свободные ячейки.
    return (allFilesTable.size() - 1);
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
    this->filesystem.tellp();
}

long int FileManager::getCurrentReadPosInFileSystem() {
    this->filesystem.tellg();
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
//    cout << filesystem.tellp();
//    cout << filesystem.tellg();
//    for (auto it = allFilesTable.begin();
//            it != allFilesTable.end(); it++) {
//        ///writing our
//        this->writeRecord((*it));
//    }
}

bool FileManager::appendInfo(string toFile) {
    if (allFilesTable.empty()) return false;
    if (allFilesTable.size() >= 20) return false;
    for (auto it = allFilesTable.begin(); it != allFilesTable.end(); it++) {
        if ((*it).getFileName() == toFile && !(*it).getAnotherPartExistence()) {
            (*it).setHasAnotherPart(true);
            (*it).setAnotherPart(this->append(toFile));
        }
    }
    return true;
}


