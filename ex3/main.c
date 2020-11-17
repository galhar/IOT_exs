// Created by osboxes on 11/16/20.
//
#include "serial_io.h"
#include <string.h>

#define READ_TIMEOUT_MS_SHORT 10000
#define READ_TIMEOUT_MS_LONG 90000
#define READ_BUF_SIZE 1000
#define OK "K\r\n"
int sendAndRecv(char *readBuf, const char *command, const char *toGet, int timeout) {
    if (command != NULL) {
        printf("%s", command);
        int a = SerialSend(command, strlen(command));

        if (a < SUCCESS) {
            perror("Error in sendAndRecv: SerialSend error\n");
            return ERROR;
        }
    }
    do {
        int a = SerialRecv(readBuf, READ_BUF_SIZE - 1, timeout);

        if (a == ERROR) {
            perror("Error in sendAndRecv: SerialSend error\n");
            return ERROR;
        }
        if (a == TIMEOUT) {
            perror("Error in sendAndRecv: SerialRecv TIMEOUT\n");
            return TIMEOUT;
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


    if (SerialInit(port, baud) == ERROR) {
        perror("Error in main: Initializing the serial\n");
        return ERROR;
    }

    if (sendAndRecv(readBuf, NULL, "+PBREADY", READ_TIMEOUT_MS_SHORT) == ERROR) {
        return ERROR;
    }
    if (sendAndRecv(readBuf, atCommand, OK, READ_TIMEOUT_MS_SHORT) == ERROR) {
        return ERROR;
    }
    if (sendAndRecv(readBuf, atccidCommand, OK, READ_TIMEOUT_MS_SHORT) == ERROR) {
        return ERROR;
    }
    if (sendAndRecv(readBuf, atcopseqCommand, OK, READ_TIMEOUT_MS_LONG) == ERROR) {
        return ERROR;
    }
    SerialFlushInputBuff();
    if (sendAndRecv(readBuf, atcregCommand, OK, READ_TIMEOUT_MS_SHORT) == ERROR) {
        return ERROR;
    }
    if (sendAndRecv(readBuf, atcopsCommand, OK, READ_TIMEOUT_MS_SHORT) == ERROR) {
        return ERROR;
    }

    SerialDisable();
}

