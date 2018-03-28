#include <iostream>
#include <csignal>
#include <unistd.h>
#include <list>

const int WAITING_TIME = 1;

const std::string NAME_OF_PROCESS =
        "Main control process : ";

const std::string FORK_ERROR =
        "Error with fork operation.";

const std::string LOAD_ERROR =
        "Error with load operation,"
        " please check filePath";

const std::string DELETE_ERROR =
        "List is empty.";

const char* PATH = "/Users/greadvx/System-Programming/"
        "spo-lab2(child)/child.o";

int main()
{
    int status = 0;
    char symbol;
    pid_t tempProcess;
    std::list<pid_t> processesIDList;

    sigset_t waitSet;
    //init of sigset and clearing
    sigemptyset(&waitSet);
    //adding a signal into set
    sigaddset(&waitSet, SIGUSR2);
    //change set of blocking signals in this time
    sigprocmask(SIG_BLOCK, &waitSet, nullptr);

    std::cout << NAME_OF_PROCESS
              << getpid() << std::endl;

    while(true) {
        //getting symbol
        std::cin.get(symbol);

        switch(symbol) {
            //case to add process
            case '+': {
                //creating new process
                tempProcess = fork();

                switch (tempProcess) {

                    //case of error
                    case -1: {
                        std::cout << FORK_ERROR << std::endl;
                    } exit(EXIT_FAILURE);

                    //case of child
                    case 0: {
                        //running code
                        execv(PATH, nullptr);

                        //this code will be runned in case of error.
                        std::cout << LOAD_ERROR << std::endl;
                    } exit(EXIT_FAILURE);

                    //parent process
                    default: {
                        //adding process to list
                        processesIDList.push_back(tempProcess);
                        sleep(WAITING_TIME);
                    } break;

                }

            } break;

            //case to delete process
            case '-': {
                if (!processesIDList.empty()) {

                    //sending to process kill signal
                    kill(processesIDList.back(), SIGKILL);

                    //deleting from list
                    processesIDList.pop_back();
                } else {
                    std::cout << DELETE_ERROR << std::endl;
                }
            } break;

            //case quit
            case 'q':
                if(!processesIDList.empty())
                {
                    for(pid_t &childPid : processesIDList) {

                        //killing child processes
                        kill(childPid, SIGKILL);
                    }
                    //clearing list
                    processesIDList.clear();
                } exit(EXIT_SUCCESS);
            default:
                continue;
        }
        std::cin.ignore();

        for(pid_t &childPid: processesIDList) {
            //send signal to next in list
            kill(childPid, SIGUSR1);
            //wait for signal
            sigwait(&waitSet, &status);
        }
    }
}
