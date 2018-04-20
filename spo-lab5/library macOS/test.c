//
//  test.c
//  test
//
//  Created by Yan Khamutouski on 4/20/18.
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

const int SIZE_OF_BUFFER = 1024;

struct asyncOperations {
    //descriptor of file
    int descOfFile;
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

int read_async(struct asyncOperations *information) {
    information->asyncIO.aio_offset = information->pointerR;
    information->asyncIO.aio_fildes = information->descOfFile;
    information->asyncIO.aio_nbytes = information->bytes;
    information->asyncIO.aio_buf = information->buffer;
    aio_read(&information->asyncIO);
    
    while(aio_error(&information->asyncIO) == EINPROGRESS);
    information->bytesMoved = aio_return(&information->asyncIO);
    
    if(information->bytesMoved)
        information->pointerR = information->pointerR + information->bytesMoved;
    
    return information->bytesMoved;
}

int write_async(struct asyncOperations *information) {
    information->asyncIO.aio_offset = information->pointerW;
    information->asyncIO.aio_fildes = information->descOfFile;
    information->asyncIO.aio_nbytes = information->bytesMoved;
    information->asyncIO.aio_buf = information->buffer;
    aio_write(&information->asyncIO);
    
    int writeResult;
    while((writeResult = aio_error(&information->asyncIO)) == EINPROGRESS);
    
    information->pointerW = information->pointerW + aio_return(&information->asyncIO);
    
    return writeResult;
}

