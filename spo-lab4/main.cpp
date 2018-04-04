#include <iostream>
#include <cstring>
#include <vector>
#include <stack>
#include <unistd.h>
#include <termios.h>
#include <ncurses.h>
#include <pthread.h>

using namespace std;

//Maximal count of new threads
const int COUNT = 10;

//Exit error code
const int ERROR_CODE = 255;

//Prototype of function in new thread.
void* printString(void* arg);

//function to close last thread
void closeLastThread();

//function to sync threads
void synchronizeThreads();

//function to add new thread
void addNewThread();

//array of strings with unique info.
const string uniqueStrings[COUNT] = {
        "First thread",
        "Second thread",
        "Third thread",
        "Fourth thread",
        "Fifth thread",
        "Sixth thread",
        "Seventh thread",
        "Eighth thread",
        "Ninth thread",
        "Tenth thread"
};

//Error message
const static string errorWithInit = "Error! "
       "Detected a problem with initializing";

//object of mutex
pthread_mutex_t pthreadMutex;

//stack of threads
stack<pthread_t> threads;

//
stack<pthread_t> closingThreads;

//
vector<bool*> quitFlags;

struct threadArg
{
    bool* quitFlag;
    int num;
};


int main() {

    //initializing of mutex
    if(pthread_mutex_init(&pthreadMutex, NULL) != 0) {
        cout << errorWithInit << endl;
        return ERROR_CODE;
    }
    while(true) {
        sleep(1);
        char symbol;

        fflush(stdin);
        cin >> symbol;

        switch(symbol) {
            case '+':
                if(threads.size() < COUNT) {
                    addNewThread();
                } break;

            case '-':
                if(threads.size() > 0) {
                    closeLastThread();
                } break;

            case 'q':
                while(threads.size() > 0) {
                    closeLastThread();
                }
                synchronizeThreads();

                //deleting mutex
                pthread_mutex_destroy(&pthreadMutex);

                return 0;

            default:
                break;
        }
    }
}


void closeLastThread()
{
    closingThreads.push(threads.top()); // Добавляем id последнего потока в стек закрывающихся потоков

    *(quitFlags.back()) = true;   // Устанавливаем флаг выхода для последнего потока

    quitFlags.pop_back();         // Удаляем указатель на флаг закрытия последнего потока из массива

    threads.pop();				  // Удаляем id последнего потока
}

void synchronizeThreads()
{
    while(closingThreads.size() > 0) {
        pthread_join(closingThreads.top(), NULL); // Ожидаем завершения последнего потока
        closingThreads.pop();
    }
}

void addNewThread() {
    quitFlags.push_back(new bool(false));

    threadArg* arg = new threadArg();
    arg->num = threads.size();              // Номер добавляемого потока
    arg->quitFlag = quitFlags.back();		// Указатель на флаг закрытия для данного потока

    pthread_t thread;

    if(pthread_create(&thread, NULL, printString, arg) != 0) {
        cout << "Creating thread error" << endl;
        return;
    }
    threads.push(thread);
}

void* printString(void* arg) {
    //	usleep(1000000); // для проверки на ошибки

    bool *qFlag = (*(threadArg*)arg).quitFlag;   // Указатель на флаг выхода для данного потока
    int threadNumber = (*(threadArg*)arg).num;   // Номер данного потока
    delete (threadArg*)arg;

    while(true) {

        if(*qFlag) break;

        pthread_mutex_lock(&pthreadMutex);
        if(*qFlag) break;
        cout << uniqueStrings[threadNumber] << endl;
        sleep(1);

        pthread_mutex_unlock(&pthreadMutex);
        sleep(2);
    }
    delete qFlag;
    return NULL;
}