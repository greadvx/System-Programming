#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <time.h>

/**
 *
 * This program create 2 process and find
 * maximal and minimal element in integer
 * array that fields in a random form.
 *
 * @author Yan Khamutouski, Kaleka Mikita
 * @version 1.0
 * @date 2018-02-22
 *
 * */

using namespace std;

int main() {

    pid_t process;
    time_t currentTime;
    int *array;
    int arraySize;
    int max, min, status;

    cout << "Original program: Process ID = " << getpid() << endl;

    //random initializing of size
    arraySize = 5 + rand() % 123;
    cout << endl << "Size of array is " << arraySize << endl;

    //memory allocation for the array
    array = new int[arraySize];
   
    for (int i = 0; i < arraySize; i++) {
        array[i] = rand() % 15;
        cout << setw(5) << array[i];
    }
    cout << endl;
    max = min = array[0];

    //creating a child process
    process = fork();

    switch(process){
        //child
        case 0 : {
            time(&currentTime);
            cout << endl << "In child: process ID = " << getpid() << " and ppid = " << getppid() << endl
                 << "started at " << ctime(&currentTime);
            for (int i = 0; i < arraySize; i++){
                if (max < array[i])
                    max = array[i];
            }
            for (int i = 0; i < arraySize; i++)
                cout << "C";
            cout << endl << "Maximum is the array is " << max << endl;
            time(&currentTime);
            cout << "Child exiting at " << ctime(&currentTime) << endl;
        } break;

        //exception
        case -1 : {
            perror("fork() exception detected.");
            exit(1);
        }

        //parent
        default: {
            time(&currentTime);
            //wait(&status);          //equivalent to waitpid(-1, &status, 0);
            cout << endl << "In parent: process ID = " << getpid() << ", fork returned = " << process << endl
                 << "is done waiting at " << ctime(&currentTime);
            for (int i = 0; i < arraySize; i++){
                if (min > array[i])
                    min = array[i];
            }
            for (int i = 0; i < arraySize; i++)
                cout << "P";
            cout << endl << "Minimal in the array is " << min << endl;
        } break;
    }
    delete array;

    return 0;
}