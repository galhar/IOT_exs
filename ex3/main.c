// Created by osboxes on 11/16/20.
//
#include <string.h>
#include "serial_io.h"

#define READ_TIMEOUT_MS 3000
#define READ_BUF_SIZE 1000
#define OK "\nOK"

void sendAndRecv(char *readBuf, const char* command, const char* toGet) {
    if(command != NULL) {
        printf("%s", command);
        SerialSend(command, strlen(command));
    }
    do{
        unsigned int a = SerialRecv(readBuf, READ_BUF_SIZE - 1, READ_TIMEOUT_MS);
        readBuf[a] = 0;
        printf("%s", readBuf);
    }while (strstr(readBuf, toGet) == NULL);
    SerialFlushInputBuff();
}

int main(int argc, char *argv[]) {
    char* port = "/dev/ttyS0";
    unsigned int baud = B115200;
    char readBuf[READ_BUF_SIZE];
    unsigned char* atCommand = "AT\r\n";
    unsigned char* atccidCommand = "AT+CCID\r\n";
    unsigned char* atcopseqCommand = "AT+COPS=?\r\n";
    unsigned char* atcregCommand = "AT+CREG?\r\n";
    unsigned char* atcopsCommand = "AT+COPS?\r\n";


    if( SerialInit(port, baud) == ERROR){
        perror("Error in main: Initializing the serial");
        return ERROR;
    }

    sendAndRecv(readBuf, NULL, "+PBREADY");
    sendAndRecv(readBuf, atCommand, OK);
    sendAndRecv(readBuf, atccidCommand, OK);
    sendAndRecv(readBuf, atcopseqCommand, OK);
    sendAndRecv(readBuf, atcregCommand, OK);
    sendAndRecv(readBuf, atcopsCommand, OK);

    SerialDisable();



}

