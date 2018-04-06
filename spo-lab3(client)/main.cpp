#include <iostream>
#include <cstring>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <cstdlib>
#include <zconf.h>

const int size = 256;

const char message[] = "received message: ";

void waitForSemaphore(int, int);

void releaseSemaphore(int, int);


int main() {
    //pointer of shared memory
    char *sharedMemory;

    //IPC descriptor of memory
    int shmid;

    //IPC descriptor of semaphores
    int semIDServer;

    //Unique name used to generate unique key (for memory)
    char pathname[] = "/Users/greadvx/System-"
                      "Programming/spo-lab3(server)/main.cpp";

    //Unique name used to generate unique key (for semaphores)
    char pathname2[] = "/Users/greadvx/System-"
                       "Programming/spo-lab3(client)/main.cpp";
    //Buffer for reading- writing
    char buffer[size];

    //IPC keys
    key_t memoryKey, semKey;

    //Generating unique key
    if((memoryKey = ftok(pathname, 0)) < 0) {
        printf("Can\'t generate memoryKey\n");
        exit(-1);
    }

    //Generating unique key
    if((semKey = ftok(pathname2, 0)) < 0){
        printf("Can\'t generate memoryKey\n");
        exit(-1);
    }

    //Trying to get access to shared memory
    if((shmid = shmget(memoryKey, size * sizeof(char), 0)) < 0) {
        printf("Can\'t find shared memory\n");
        exit(-1);
    }

    //Getting set of semaphores
    if((semIDServer = semget(semKey, 1, 0666 )) < 0){
        printf("Can\'t get semID\n");
        exit(-1);
    }

    //Synchronization of client
    releaseSemaphore(semIDServer, 0);      // Ожидание, пока счётчик семафора не станет равным 1
    waitForSemaphore(semIDServer, 1);

    printf("Connected to server\n");

    //Attaching shared memory
    if((sharedMemory = (char *)shmat(shmid, NULL, 0)) == (char *)(-1)) {
        printf("Can't attach shared memory\n");
        exit(-1);
    }
    sleep(1);

    while(true) {

        //synchronizing
        waitForSemaphore(semIDServer, 0);

        //copying data to buffer
        strcpy(buffer, sharedMemory);

        //quitting
        if (!strcmp(buffer, "q")) break;

        printf(message);
        printf("%s\n", buffer);
        releaseSemaphore(semIDServer, 1);
    }

    //closing shared source
    if(shmdt(sharedMemory) < 0){
        printf("Can't detach shared memory\n");
        exit(-1);
    }
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

