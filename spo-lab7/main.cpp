#include <iostream>

#include "fileManager/FileManager.h"
#include "handlers.h"

using namespace std;

void help();
pair<string, string> searchForCommandAndName(string toPharse);


int main() {
    ///creating an object of file manager
    FileManager manager("virtual_file_system");
    char buffer[80];
    while(true){
        cout << "V:/";
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
            manager.del(parsed.second);
        }
        if (parsed.first == "rename") {
            //парсим еще раз second
            //иначе ошибка
            manager.rename("", "");
        }
        if (parsed.first == "open") {
            //проверка на наличие пробела find должна вернуть -1
            manager.open(parsed.second);
        }
        if (parsed.first == "create") {
            manager.create();
        }
        if (parsed.first == "append") {
            //проверка на наличие пробела find должна вернуть -1
            manager.appendInfo("");
        }
        if (parsed.first == "help") {
            help();
        }
    }
    return 0;
}

pair<string, string> searchForCommandAndName(string toPharse) {
    string::size_type posOfSpace;
    posOfSpace = toPharse.find(' ');
    if (posOfSpace == -1) {
        return (toPharse, "-");
    }
    //TODO: Разбить на подстроки и вернуть их
    else {

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