//
// Created by osboxes on 11/24/20.
//
#include "cellular.h"

#define AT_COMMAND "AT\r\n"
#define CREG_COMMAND "AT+CREG=?\r\n"
#define COPS_COMMAND "AT+COPS=?\r\n"
#define COPS_FORMAT_COMMAND "AT+COPS=%d, %d, \"%s\"\r\n"

#define OK "K\r\n"
#define REGCOLON "REG:"
#define CREG_REGSTATUS_LOC 8

#define READ_BUF_SIZE 500

#define SHORT_TIMEOUT_MS 9000
#define LONG_TIMEOUT_MS 90000


char readBuf[READ_BUF_SIZE];

int sendAndRecv(const char *command, const char *toGet, int timeout) {
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

int CellularInit(char *port) {
    int portLen = strlen(port);
    if (portLen > MAX_PORT_LEN - 1) {
        perror("Error in CellularInit: port length too big\n");
        return ERROR;
    }

    if (SerialInit(port, BAUD) == ERROR) {
        perror("Error in CellularInit: SerialInit error\n");
        return ERROR;
    }

    if (sendAndRecv(NULL, "+PBREADY", SHORT_TIMEOUT_MS) != SUCCESS) {
        perror("Error in CellularInit: No +PBREADY recieved\n");
        return ERROR;
    }

    return SUCCESS;
}


void CellularDisable(void) {
    SerialDisable();
}


int CellularCheckModem(void) {
    int rc;
    rc = sendAndRecv(AT_COMMAND, OK, SHORT_TIMEOUT_MS);
    if (rc == TIMEOUT) {
        perror("ERROR in CellularCheckModem: timeout from the modem");
        return ERROR;
    }

    if (rc == ERROR) {
        perror("ERROR in CellularCheckModem");
        return ERROR;
    }

    return SUCCESS;

}


int CellularGetRegistrationStatus(int *status) {
    int rc;
    rc = sendAndRecv(CREG_COMMAND, OK, LONG_TIMEOUT_MS);
    if (rc == TIMEOUT) {
        perror("ERROR in CellularGetRegistrationStatus: got timeout from the modem\n");
        return ERROR;
    }
    if (rc == ERROR) {
        perror("ERROR in CellularGetRegistrationStatus\n");
        return ERROR;
    }

    // Now parse the regstatus out of; +CREG: <Mode>, <regStatus>
    char *regColonPointer;
    regColonPointer = strstr(readBuf, REGCOLON);
    if (regColonPointer == NULL) {
        perror("ERROR in CellularGetRegistrationStatus: didn't recieve \"reg: in the response\n");
        return ERROR;
    }
    *status = atoi(regColonPointer + CREG_REGSTATUS_LOC);
    if (*status == 0) {
        perror("ERROR in CellularGetRegistrationStatus: Error parsing the regStatus\n");
        return ERROR;
    }

    return SUCCESS;
}


int CellularGetOperators(OPERATOR_INFO *opList, int maxops, int *numOpsFound) {
    int rc = sendAndRecv(COPS_COMMAND, OK, LONG_TIMEOUT_MS);

    // Now parse out the available operators


    return SUCCESS;
}


int CellularSetOperator(int mode, char *operatorCode) {
    char *setCommand;
    asprintf(&setCommand, COPS_FORMAT_COMMAND, mode, 2, operatorCode);
    int rc = sendAndRecv(setCommand, OK, SHORT_TIMEOUT_MS);
    if (rc == TIMEOUT) {
        perror("ERROR in CellularSetOperators: got timeout from the modem\n");
        return ERROR;
    } else if (rc == ERROR) {
        perror("ERROR in CellularSetOperators\n");
        return ERROR;
    }

    free(setCommand);
    return SUCCESS;
}


int CellularGetSignalQuality(int *csq) {
    return SUCCESS;
}


int CellularGetICCID(char *iccid, int maxlen) {
    return SUCCESS;
}


int CellularGetSignalInfo(SIGNAL_INFO *sigInfo) {
    return SUCCESS;
}

