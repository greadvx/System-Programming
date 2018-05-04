cmake_minimum_required(VERSION 3.10)
project(spo_lab7)

set(CMAKE_CXX_STANDARD 17)

add_executable(spo_lab7 main.cpp tableStructure/FileRecord.cpp tableStructure/FileRecord.h handlers.h fileManager/FileManager.cpp fileManager/FileManager.h)