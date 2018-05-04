#include <iostream>

#include "fileManager/FileManager.h"
#include "handlers.h"

using namespace std;

int main() {
    ///creating an object of file manager
    FileManager manager("virtual_file_system");
    manager.fillAllFilesTable();
    manager.create();
    manager.ls();
    manager.quit();
    return 0;
}