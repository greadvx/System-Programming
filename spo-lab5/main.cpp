#include <iostream>
#include <unistd.h>
#include <aio.h>
#include <sys/types.h>
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
    //descriptor of file
    int descOfFile;
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
}information;

using namespace std;

//function to control folder
bool isEmptyFolder(const char *folder);

pthread_t createRThread(const char* folderPath);
pthread_t createWThread(const char* folderPath);

//prototype of function of reading thread
void* readingThread(void* arg);

//prototype of function of writing thread
void* writingThread(void* arg);

int (*read_async) (struct asyncOperations *);
int (*write_async) (struct asyncOperations *);


//object of mutex for sync
pthread_mutex_t pthreadMutex;

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
    //opening the library
    void *lib = dlopen("./spo-lib.dylib",RTLD_NOW);
    if(lib == NULL) {
        printf("Library ERROR");
        return ERROR_LIB;
    }
    //searching for such function
    read_async = (int(*)(struct asyncOperations*))dlsym(lib,"read_async");
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
    //initializing of mutex
    if(pthread_mutex_init(&pthreadMutex, NULL) != 0) {
        cout << errorWithInit << endl;
        return ERROR_INIT;
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
    bool block = true;
    pthread_mutex_lock(&pthreadMutex);
    block = false;
    string path;
    path.append((char*)arg);

    //list of files that are stored in folder
    list<string> filesList;

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

                //adding files in list.
                filesList.push_back(fullPath);
            }
        }
        closedir(dirp);
    }
    int rDescriptor = 0;
    int readResult = 0;
    while(true) {
        if(block)
            pthread_mutex_lock(&pthreadMutex);
        if (readResult == 0) {
            information.pointerR = 0;
            information.bytes = SIZE_OF_BUFFER;
            rDescriptor = open(filesList.front().c_str(), O_RDONLY);
        }
        information.descOfFile = rDescriptor;

        readResult = read_async(&information);

        //if end of file
        if (readResult == 0) {
            cout << "added " << filesList.front() << endl;
            filesList.pop_front();
            close(rDescriptor);
            if (filesList.empty()) {
                strcpy(information.buffer, "\0");
                pthread_mutex_unlock(&pthreadMutex);
                break;
            }
            else {
                pthread_mutex_unlock(&pthreadMutex);
                block = true;
            }
        }
        else {
            block = true;
            pthread_mutex_unlock(&pthreadMutex);
        }
    }
    return NULL;
}

void* writingThread(void* arg) {
    bool flag;
    pthread_mutex_lock(&pthreadMutex);
    flag = true;

    string pathToResultFile;
    pathToResultFile.append((char*)arg);
    pathToResultFile.append("/result.txt");

    int wDescriptor = open(pathToResultFile.c_str(), O_WRONLY | O_CREAT, S_IRWXU);

    while(true) {

        //blocking
        if (!flag)
            pthread_mutex_lock(&pthreadMutex);

        information.descOfFile = wDescriptor;

        //condition for quit
        if (!strcmp(information.buffer, "\0")) {
            pthread_mutex_unlock(&pthreadMutex);
            break;
        }
        //async writing
        write_async(&information);

        //unblocking
        flag = false;
        pthread_mutex_unlock(&pthreadMutex);
    }
    return NULL;
}
