//
// Created by osboxes on 11/24/20.
//
#include "cellular.h"
#include "serial_io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PORT_LEN 16
char *modemPort;

int CellularInit(char *port) {
    int portLen = strlen(port);
    if (portLen > MAX_PORT_LEN - 1) {
        perror("Error in CellularInit: port length too big");
        return ERROR;
    }
    modemPort = malloc(portLen);
    strcpy(modemPort, port);

    return SerialInit(modemPort, BAUD);
}


void CellularDisable(void) {

}


int CellularCheckModem(void) {

}


int CellularGetRegistrationStatus(int *status) {

}


int CellularGetOperators(OPERATOR_INFO *opList, int maxops, int *numOpsFound) {

}


int CellularSetOperator(int mode, char *operatorCode) {

}


int CellularGetSignalQuality(int *csq) {

}


int CellularGetICCID(char *iccid, int maxlen) {

}


int CellularGetSignalInfo(SIGNAL_INFO *sigInfo) {

}

