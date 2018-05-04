//
// Created by Yan Khamutouski on 4/30/18.
//

#ifndef SPO_LAB7_HANDLERS_H

#define SPO_LAB7_HANDLERS_H

#include <string>
#include <map>

const int MAX = 20;

using namespace std;

///map of operations that we can provide on our filesystem

map<string, int> operations = { { "ls",     1},
                                { "create", 2},
                                { "del",    3},
                                { "open",   4},
                                { "append", 5},
                                { "rename", 6}
};


#endif //SPO_LAB7_HANDLERS_H
