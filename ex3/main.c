//
// Created by osboxes on 11/16/20.
//
#include "serial_io.h"

#define READ_TIMEOUT_MS 3000
#define READ_BUF_SIZE 1000

int main(int argc, char *argv[]) {
    char* port = "dev/tty0/";
    unsigned int baud = 15200;
    char readBuf[READ_BUF_SIZE];

    if( SerialInit(port, baud) == ERROR){
        perror("Error in main: Initializing the serial");
        return ERROR;
    }

    SerialRecv(readBuf, READ_BUF_SIZE + 1, READ_TIMEOUT_MS);



}
