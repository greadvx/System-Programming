//
//  library.c
//  spo-lib
//
//  This library is created for macOS 10.13 High Sierra
//
//  Created by Yan Khamutouski on 4/21/18.
//  Copyright © 2018 Yan Khamutouski. All rights reserved.
//
#include <stdio.h>
#include <unistd.h>
#include <aio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <pthread.h>
#include <dirent.h>
#include <errno.h>
#include <dlfcn.h>

const int SIZE_OF_BUFFER = 1024;

struct asyncOperations {
    //flag read
    bool readInit;
    bool readFinished;
    bool iteration;
    //descriptor of file r
    int descOfFile;
    
    int descofFileW;
    //buffer for reading
    char buffer[SIZE_OF_BUFFER];
    //bytes read
    size_t bytes;
    size_t bytesMoved;
    //pointer to conduct moving operations
    off_t pointerR;
    off_t pointerW;
    //struct for async r/w
    struct aiocb asyncIO;
};

void read_async(struct asyncOperations *information) {
    information->asyncIO.aio_offset = information->pointerR;
    information->asyncIO.aio_fildes = information->descOfFile;
    information->asyncIO.aio_nbytes = information->bytes;
    information->asyncIO.aio_buf = information->buffer;
    aio_read(&information->asyncIO);
    
}
int write_async(struct asyncOperations *information) {
    information->asyncIO.aio_offset = information->pointerW;
    information->asyncIO.aio_fildes = information->descofFileW;
    information->asyncIO.aio_nbytes = information->bytesMoved;
    information->asyncIO.aio_buf = information->buffer;
    aio_write(&information->asyncIO);
    
    int writeResult;
    while((writeResult = aio_error(&information->asyncIO)) == EINPROGRESS);
    
    return writeResult;
}
