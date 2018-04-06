#include <iostream>
#include <cstring>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <cstdlib>

using namespace std;

const int size = 256;

const char waitMessage[] = "Waiting for client connection. \n";

void waitForSemaphore(int, int);

void releaseSemaphore(int, int);

int main() {

    //pointer of shared memory
    char *sharedMemory;

    //IPC descriptor of memory
    int shmID;

    //IPC descriptor of semaphores
    int semIDServer;

    //Unique name used to generate unique key (for memory)
    char pathname[] = "/Users/greadvx/System-Programming/"
                      "spo-lab3(server)/main.cpp";

    //Unique name used to generate unique key (for semaphores)
    char pathname2[] = "/Users/greadvx/System-Programming/"
                      "spo-lab3(client)/main.cpp";
    //Buffer for reading- writing
    char buffer[size];
    string bufferString;

    //IPC keys
    key_t memoryKey, semKey;

    //Generating unique key
    if((memoryKey = ftok(pathname, 0)) < 0){
        printf("Can\'t generate memoryKey\n");
        exit(-1);
    }
    //Generating unique key
    if((semKey = ftok(pathname2, 0)) < 0){
        printf("Can\'t generate memoryKey\n");
        exit(-1);
    }



   //Trying to create/access shared memory (0666 - r/w for all users)
    if((shmID = shmget(memoryKey, size * sizeof(char),
                       0666|IPC_CREAT|IPC_EXCL)) < 0){
        //If doesn't exist
        if(errno != EEXIST){
            printf("Can\'t create shared memory\n");
            exit(-1);
        } else {
            //Accessing in case of existing
            if((shmID = shmget(memoryKey, size * sizeof(char), 0)) < 0){
                printf("Can\'t find shared memory\n");
                exit(-1);
            }
        }
    }
    //Creating/getting set of semaphores
    if ((semIDServer = semget(semKey, 1, 0666 | IPC_CREAT)) < 0){
        printf("Can\'t get semID\n");
        exit(-1);
    }
    //Init with 0
    semctl(semIDServer, 0, SETALL, 0);

    //Attaching shared memory in address space
    if ((sharedMemory = (char *)shmat(shmID, NULL, 0)) == (char *)(-1)){
        printf("Can't attach shared memory\n");
        exit(-1);
    }
    printf(waitMessage);
    //waiting for client connection
    waitForSemaphore(semIDServer, 0);
    releaseSemaphore(semIDServer, 1);
    cout << "q - to exit" << endl;
    cout << endl << "Input messages:" << endl;
    fflush(stdin);
    while(true) {


        cin.getline(buffer, size);
        if (cin.fail()) {
            cin.clear();
        }
        strcpy(sharedMemory, buffer);

        releaseSemaphore(semIDServer, 0);	// сообщаем о передаче сообщения
        waitForSemaphore(semIDServer, 1);    // Ожидаем получения сообщения

        //quitting
        if (!strcmp(buffer, "q")) break;
    }
    printf("quited\n");

    //closing shared source
    if(shmdt(sharedMemory) < 0){
        printf("Can't detach shared memory\n");
        exit(-1);
    }
    //deleting shared memory and set of semaphores
    shmctl(shmID, IPC_RMID, NULL);
    semctl(semIDServer, IPC_RMID, 0);

    return 0;
}


//Waiting for counter != 1 and resetting it to 0
void waitForSemaphore(int semID, int num) {
    struct sembuf buf;
    buf.sem_op = -1;
    buf.sem_flg = SEM_UNDO;
    buf.sem_num = num;
    semop(semID, &buf, 1);
}

//Setting semaphore count into 1
void releaseSemaphore(int semID, int num) {
    struct sembuf buf;
    buf.sem_op = 1;
    buf.sem_flg = SEM_UNDO;
    buf.sem_num = num;
    semop(semID, &buf, 1);
}