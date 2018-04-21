#include <iostream>
#include <unistd.h>
#include <aio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <dirent.h>
#include <list>
#include <regex>
#include <cstring>
#include <dlfcn.h>


const char isEmptyFolderMessage[] = "\n There is no files in directory. \nQuitting...";
const char paramsIsNotSet[] = "\n Command line params isn't set yet. \nQuitting...";
const char errorWithInit[] = "\n Error with initialization of mutex. \nQuitting...";
const char errorWithCreating[] = "\n Error with creating of thread. \n";
const char errorWithFounding[] = "\n Method or function is NOT FOUND. \nQuitting...";

const int SIZE_OF_BUFFER = 1024;

const int ERROR_CMD = -1;
const int ERROR_EMPTY = -2;
const int ERROR_INIT = -3;
const int ERROR_LIB = -4;
const int ERROR_FIND = -5;

struct asyncOperations {
    //flag read
    bool readInit;
    bool readFinished;

    bool iteration = false;

    //descriptor of file r
    int descOfFile;

    int descofFileW;
    //buffer for reading
    char buffer[SIZE_OF_BUFFER];
    //bytes read
    size_t bytes;
    size_t bytesMoved;
    //pointer to conduct moving operations
    off_t pointerR;
    off_t pointerW;
    //struct for async r/w
    struct aiocb asyncIO;
};

using namespace std;

//function to control folder
bool isEmptyFolder(const char *folder);

pthread_t createRThread(const char* folderPath);
pthread_t createWThread(const char* folderPath);

//prototype of function of reading thread
void* readingThread(void* arg);

//prototype of function of writing thread
void* writingThread(void* arg);

void (*read_async) (struct asyncOperations *);
int (*write_async) (struct asyncOperations *);

pthread_mutex_t mutexS;

list <asyncOperations*> readyToWrite;

int main(int argc, char *argv[]) {

    if (argc > 1) {
        if (isEmptyFolder(argv[1])) {
            cout << isEmptyFolderMessage << endl;
            getchar();
            return ERROR_EMPTY;
        }
    } else {
        cout << paramsIsNotSet << endl;
        getchar();
        return ERROR_CMD;
    }
    if (pthread_mutex_init(&mutexS, NULL)) {
        cout << errorWithInit << endl;
        return ERROR_INIT;
    }
    //opening the library
    void *lib = dlopen("./libspo-lib.dylib", RTLD_NOW);
    if(lib == NULL) {
        printf("Library ERROR");
        return ERROR_LIB;
    }
    //searching for such function
    read_async = (void(*)(struct asyncOperations*))dlsym(lib,"read_async");
    if (!read_async) {
        cout << errorWithFounding << endl;
        cout << dlerror() << endl;
        return ERROR_FIND;

    }
    //searching for such function
    write_async = (int(*)(struct asyncOperations*))dlsym(lib,"write_async");
    if (!write_async) {
        cout << errorWithFounding << endl;
        cout << dlerror() << endl;
        return ERROR_FIND;

    }
    //id of reading thread
    pthread_t readingThread;
    readingThread = createRThread(argv[1]);
    //id of writing thread
    pthread_t writingThread;
    writingThread = createWThread(argv[1]);

    pthread_join(readingThread, NULL);
    pthread_join(writingThread, NULL);

    //unplug dynamic library
    dlclose(lib);

    cout << "\nConcatenate fineshed. \n Quitting..." << endl;
    return 0;
}

bool isEmptyFolder(const char *folder) {
    //describing an open directory
    DIR *dirp;
    //struct to describe directory
    struct dirent *directory;

    dirp = opendir(folder);

    int countOfFiles = 0;

    if (dirp) {
        while(directory = readdir(dirp)) {

            if (strcmp(directory->d_name, ".") && strcmp(directory->d_name, ".."))
                countOfFiles += 1;
        }
        closedir(dirp);
    }
    return countOfFiles <= 0;
}

//creating a reading thread
pthread_t createRThread(const char* folderPath) {

    pthread_t thread;

    if (pthread_create(&thread, NULL, readingThread, (void*)folderPath) != 0) {
        cout << errorWithCreating;
        return NULL;
    }
    return thread;
}

//creating a writing thread
pthread_t createWThread(const char* folderPath) {

    pthread_t thread;

    if (pthread_create(&thread, NULL, writingThread, (void*)folderPath) != 0) {
        cout << errorWithCreating;
        return NULL;
    }
    return thread;
}


void* readingThread(void* arg) {
    pthread_mutex_lock(&mutexS);
    string path;
    path.append((char*)arg);

    //list of files that are stored in folder
    list<string> filesList;
    list<asyncOperations> buffers;

    DIR *dirp;
    struct dirent *directory;

    dirp = opendir(path.c_str());

    if (dirp) {
        while(directory = readdir(dirp)) {
            string file(directory->d_name);
            regex regularExpression(".+\\.txt");
            smatch matcher;
            regex_search(file, matcher, regularExpression);
            for (auto match: matcher) {

                string fullPath;
                fullPath.append(path);
                fullPath.append("/");
                fullPath.append(match);
                filesList.push_back(fullPath);
            }
        }
        closedir(dirp);
    }
    int counter = 0;
    do{
        struct asyncOperations *information = new asyncOperations();
        information->pointerR = 0;
        information->bytes = SIZE_OF_BUFFER;

        int rDescriptor;
        rDescriptor = open(filesList.front().c_str(), O_RDONLY);
        filesList.pop_front();

        information->descOfFile = rDescriptor;
        information->readInit = true;
        readyToWrite.push_back(information);
        counter++;
        read_async(information);
    }while(filesList.empty() == false);
    bool flag = false;
    while(!readyToWrite.empty()) {
        if(flag) {
            pthread_mutex_lock(&mutexS);
            flag = false;
        }

        if (readyToWrite.empty()) break;

        if (readyToWrite.front()->iteration) {
            pthread_mutex_lock(&mutexS);
            read_async(readyToWrite.front());
            readyToWrite.front()->iteration = false;
        }
        //reading is ended
        if (aio_error(&readyToWrite.front()->asyncIO) == 0) {
            //returned quantity of bytes is 0
            int readResult = aio_return(&readyToWrite.front()->asyncIO);
            if (readResult == 0) {
                readyToWrite.front()->bytesMoved = readResult;
                readyToWrite.front()->readFinished = true;
                readyToWrite.front()->readInit = false;
                readyToWrite.front()->iteration = false;
                pthread_mutex_unlock(&mutexS);
                flag = true;
            }
            //returned quantity of bytes is not 0
            else {
                flag = false;
                readyToWrite.front()->bytesMoved = readResult;
                readyToWrite.front()->pointerR += readyToWrite.front()->bytesMoved;
                readyToWrite.front()->iteration = true;
                pthread_mutex_unlock(&mutexS);
                }
            }
    }
    return NULL;
}

void* writingThread(void* arg) {

    pthread_mutex_lock(&mutexS);
    int numberOfFile = 0;
    string pathToResultFile;
    pathToResultFile.append((char*)arg);
    pathToResultFile.append("/result.txt");
    off_t offset = 0;
    off_t current = 0;
    bool flag;
    int wDescriptor = open(pathToResultFile.c_str(), O_WRONLY | O_CREAT, S_IRWXU);

    while(!readyToWrite.empty()) {
        if (flag)
            pthread_mutex_lock(&mutexS);
        if (readyToWrite.front()->readFinished || readyToWrite.front()->iteration) {
            readyToWrite.front()->descofFileW = wDescriptor;
            readyToWrite.front()->pointerW = offset;
            write_async(readyToWrite.front());
            current = aio_return(&readyToWrite.front()->asyncIO);
            offset += current;
            if (readyToWrite.front()->readFinished) {
                numberOfFile += 1;
                cout << "Content of file #" << numberOfFile
                     << " has been written to result file successful!" << endl;
                close(readyToWrite.front()->descOfFile);
                readyToWrite.pop_front();
            }
            flag = true;
            pthread_mutex_unlock(&mutexS);
        }
    }
    close(wDescriptor);
    return NULL;
}