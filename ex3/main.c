// Created by osboxes on 11/16/20.
//
#include "serial_io.h"
#include <string.h>
#include "serial_io.h"

#define READ_TIMEOUT_MS 3000
#define READ_BUF_SIZE 1000
#define OK "\nOK"

int sendAndRecv(char *readBuf, const char *command, const char *toGet) {
    if (command != NULL) {
        printf("%s", command);
        int a = SerialSend(command, strlen(command));

        if (a < SUCCESS) {
            perror("Error in sendAndRecv: SerialSend error");
            return ERROR;
        }
    }
    do {
        unsigned int a = SerialRecv(readBuf, READ_BUF_SIZE - 1, READ_TIMEOUT_MS);

        if (a < SUCCESS) {
            perror("Error in sendAndRecv: SerialRecv error");
            return ERROR;
        }

        readBuf[a] = 0;
        printf("%s", readBuf);
    } while (strstr(readBuf, toGet) == NULL);
    SerialFlushInputBuff();
    return SUCCESS;
}

int main(int argc, char *argv[]) {
    char *port = "/dev/ttyS1";
    unsigned int baud = B115200;
    char readBuf[READ_BUF_SIZE];
    unsigned char *atCommand = "AT\r\n";
    unsigned char *atccidCommand = "AT+CCID\r\n";
    unsigned char *atcopseqCommand = "AT+COPS=?\r\n";
    unsigned char *atcregCommand = "AT+CREG?\r\n";
    unsigned char *atcopsCommand = "AT+COPS?\r\n";
    int rc = SUCCESS;


    if (SerialInit(port, baud) == ERROR) {
        perror("Error in main: Initializing the serial");
        return ERROR;
    }

    if (sendAndRecv(readBuf, NULL, "+PBREADY") == ERROR) {
        return ERROR;
    }
    if (sendAndRecv(readBuf, atCommand, OK) == ERROR) {
        return ERROR;
    }
    if (sendAndRecv(readBuf, atccidCommand, OK) == ERROR) {
        return ERROR;
    }
    if (sendAndRecv(readBuf, atcopseqCommand, OK) == ERROR) {
        return ERROR;
    }
    if (sendAndRecv(readBuf, atcregCommand, OK) == ERROR) {
        return ERROR;
    }
    if (sendAndRecv(readBuf, atcopsCommand, OK) == ERROR) {
        return ERROR;
    }

    SerialDisable();


}

