#include <iostream>

#include "fileManager/FileManager.h"
#include "handlers.h"

using namespace std;

void help();
pair<string, string> searchForCommandAndName(string toParse);


int main() {
    ///creating an object of file manager
    FileManager manager("virtual_file_system");
    manager.fillAllFilesTable();
    char buffer[80];
    while(true){
        cout << endl <<"V:/";
        fflush(stdin);
        cin.getline(buffer, 80);
        string toParse(buffer);
        auto parsed = searchForCommandAndName(toParse);

        if (parsed.first == "quit") {
            manager.quit();
            break;
        }
        if (parsed.first == "ls") {
            manager.ls();
        }
        if (parsed.first == "del") {
            //проверка на наличие пробела find должна вернуть -1
            if (parsed.second.find(" ") == -1)
                manager.del(parsed.second);
            else cout << endl <<"Error in command! Use help" << endl;
        }
        if (parsed.first == "rename") {

            pair<string, string> names = searchForCommandAndName(parsed.second);
            if (names.first.find(" ") == -1 && names.second.find(" ") == -1)
                manager.rename(names.first, names.second);
            else cout << endl << "Error in command! Use help" << endl;
        }
        if (parsed.first == "open") {
            //проверка на наличие пробела find должна вернуть -1
            if (parsed.second.find(" ") == -1)
                manager.open(parsed.second);
            else cout << endl << "Error in command! Use help" << endl;
        }
        if (parsed.first == "create") {
            if (parsed.second == "-")
                manager.create();
            else cout << endl << "Error in command! Use help" << endl;
        }
        if (parsed.first == "append") {
            //проверка на наличие пробела find должна вернуть -1
            if (parsed.second.find(" ") == -1)
                manager.appendInfo(parsed.second);
            else cout << endl << "Error in command! Use help" << endl;
        }
        if (parsed.first == "help") {
            if (parsed.second == "-")
                help();
            else cout << endl << "Error in command! Use help" << endl;
        }
    }
    return 0;
}

pair<string, string> searchForCommandAndName(string toPharse) {
    string::size_type posOfSpace;
    posOfSpace = toPharse.find(' ');
    string empty = "-";
    if (posOfSpace == -1) {
        pair<string, string> toReturn(toPharse, empty);
        return toReturn;
    }
    //TODO: Разбить на подстроки и вернуть их
    else {
        string secondPart = toPharse.substr(posOfSpace + 1, toPharse.length());
        string firstPart = toPharse.substr(0, posOfSpace);
        pair<string, string> toReturn(firstPart, secondPart);
        return toReturn;
    }
}
void help() {
    cout << "Commands of filesystem:" << endl;
    cout << setw(25) << "help" << setw (25) << "– show all user commands." << endl;
    cout << setw(25) << "ls" << setw (25) << "– list all files in filesystem." << endl;
    cout << setw(25) << "quit" << setw (25) << "– quitting from filesystem." << endl;
    cout << setw(25) << "create" << setw (25) << "– creating a file." << endl;
    cout << setw(25) << "open nameOfFile" << setw (25) << "– opening a file by name." << endl;
    cout << setw(25) << "del nameOfFile" << setw (25) << "– deleting file by name." << endl;
    cout << setw(25) << "rename oldName newName" << setw (25) << "– renaming of file." << endl;
    cout << setw(25) << "append nameOfFile" << setw (25) << "– appending info to file." << endl;
}