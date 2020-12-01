//
// Created by osboxes on 11/24/20.
//
#include "cellular.h"

#define AT_COMMAND "ATE1\r\n"
#define CREG_COMMAND "AT+CREG?\r\n"
#define COPS_COMMAND "AT+COPS=?\r\n"
#define CSQ_COMMAND "AT+CSQ\r\n"
#define CCID_COMMAND "AT+CCID\r\n"

#define COPS_FORMAT_COMMAND "AT+COPS=%d,%d,\"%s\"\r\n"

// Set the internet connection commands
// 0 is the connection ID
#define SICS_CONNTYPE_COMM "AT^SICS=0,conType,\"GPRS0\""
#define SICS_INACT_FORMAT_COMM "AT^SICS=0,inactto,\"%d\""
#define SICS_APN_COMM "AT^SICS=0,apn,\"internet.t-d1.de\""

// Set the service profile commands
// 1 is the service ID
#define SISS_SRVTYPE "AT^SISS=1,\"SrvType\",\"Socket\""
#define SISS_CONNID "AT^SISS=1,\"conId\",\"0\""
#define SISS_SOCKTCP_FORMAT "AT^SISS=1,\"address\",\"socktcp://%s:%d;etx;timer=%d\""

// Open the internet session defined by the service profile defined
#define SISO_COMM "AT^SISO=1"

// Change to transparent mode
#define SIST_COMM "AT^SIST=1"

// Closes the TCP/IP connection to the remote peer which was opened with AT^SISO
#define SISC_COMM "AT^SISC=1"

#define OK "K\r\n"
#define CONNECT "NNECT\r\n"

#define REGCOLON "REG:"
#define CREG_REGSTATUS_LOC 7
#define REGSCQ "Q: "
#define REGSCQ_LOC 3
#define NOCARRIER "\r\nNO CARRIER\r\n"



#define READ_BUF_SIZE 500

#define SHORT_TIMEOUT_MS 9000
#define LONG_TIMEOUT_MS 90000

#define SLEEP_TIME 30

#define SECOND_G "2G"
#define THIRD_G "3G"

char readBuf[READ_BUF_SIZE];

int sendAndRecv(const char *command, const char *toGet, int timeout) {
    SerialFlushInputBuff();
    if (command != NULL) {
        printf("%s", command);
        usleep(SLEEP_TIME);
        int a = SerialSend(command, strlen(command));
        usleep(SLEEP_TIME);
        if (a < SUCCESS) {
            perror("Error in sendAndRecv: SerialSend error\n");
            return ERROR;
        }
    }
    do {
        usleep(SLEEP_TIME);
        int a = SerialRecv(readBuf, READ_BUF_SIZE - 1, timeout);
        usleep(SLEEP_TIME);
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


int checkSendAndRecieve(int rc, const char *a) {
    if (rc == TIMEOUT) {
        perror(a);
        perror(": timeout from the modem\n");
        return ERROR;
    }

    if (rc == ERROR) {
        perror(a);
        perror("\n");
        return ERROR;
    }

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

    return checkSendAndRecieve(rc, "ERROR in CellularCheckModem");

}


int CellularGetRegistrationStatus(int *status) {
    int rc;
    rc = sendAndRecv(CREG_COMMAND, OK, LONG_TIMEOUT_MS);
    if( checkSendAndRecieve(rc, "ERROR in CellularGetRegistrationStatus") == ERROR){
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
    if( checkSendAndRecieve(rc, "ERROR in CellularGetOperator") == ERROR){
        return ERROR;
    }

    // Now parse out the available operators
    char *operatorData, *nextOpData, *curOpDataEnd;
    *numOpsFound = 0;
    operatorData = strchr(readBuf, '(');
    while (operatorData && *numOpsFound < maxops) {
        if (operatorData[1] == ')') {
            // No operator data here
            continue;
        }
        // Prepare for processing and get next op data
        nextOpData = strchr(operatorData + 1, '(');
        // To allow search from end with strrchr
        curOpDataEnd = strchr(operatorData + 1, ')');
        *curOpDataEnd = '\0';

        // Parse out the current operator
        OPERATOR_INFO *curOpInfo = opList + *numOpsFound;

        // Parse the name
        char *opNameStart = strchr(operatorData, '"') + 1;
        char *opNameEnd = strchr(opNameStart, '"');
        int nameLen = opNameEnd - opNameStart;
        if (nameLen > OPERATOR_NAME_BUF_SIZE - 1) {
            nameLen = OPERATOR_NAME_BUF_SIZE - 1;
        }
        strncpy((*curOpInfo).operatorName, opNameStart, nameLen);
        (*curOpInfo).operatorName[nameLen] = '\0';

        // Parse AcT
        char *opAcT = strrchr(operatorData, '\0') - 1;
        strncpy((*curOpInfo).accessTechnology, (*opAcT - 48) == 0 ? SECOND_G : THIRD_G, strlen(SECOND_G));

        // Parse the code
        char *opCodeEnd = strrchr(operatorData, '"');
        *opCodeEnd = '\0';
        char *opCodeStart = strrchr(operatorData, '"') + 1;
        //int codeLen = opCodeEnd - opCodeStart;
        (*curOpInfo).operatorCode = atoi(opCodeStart);

        // Jump to next operator
        operatorData = nextOpData;
        (*numOpsFound)++;
    }


    return SUCCESS;
}


int CellularSetOperator(int mode, char *operatorCode) {
    char *setCommand;
    asprintf(&setCommand, COPS_FORMAT_COMMAND, mode, 2, operatorCode);
    int rc = sendAndRecv(setCommand, OK, SHORT_TIMEOUT_MS);
    if( checkSendAndRecieve(rc, "ERROR in CellularSetOperators") == ERROR) {
        return ERROR;
    }
    free(setCommand);
    return SUCCESS;
}


int CellularGetSignalQuality(int *csq) {
    char csqChar[3];
    int rc;
    rc = sendAndRecv(CSQ_COMMAND, OK, SHORT_TIMEOUT_MS);

    if( checkSendAndRecieve(rc, "ERROR in CellularGetRegistrationStatus") == ERROR) {
        return ERROR;
    }


    char *regSCQPointer;
    regSCQPointer = strstr(readBuf, REGSCQ);
    if (regSCQPointer == NULL) {
        perror("ERROR in CellularGetSignalQuality: didn't recieve \"+CSQ: in the response\n");
        return ERROR;
    }
    if(regSCQPointer[5] == ',') {
        strncpy(csqChar, regSCQPointer + REGSCQ_LOC, 2);
    }
    else {
        strncpy(csqChar, regSCQPointer + REGSCQ_LOC, 1);
    }
    *csq = atoi(csqChar);
    if (*csq == 0) {
        perror("ERROR in CellularGetSignalQuality: Error parsing the csq\n");
        return ERROR;
    }
    return SUCCESS;
}


int CellularGetICCID(char *iccid, int maxlen) {
    int rc;
    rc = sendAndRecv(CCID_COMMAND, OK, SHORT_TIMEOUT_MS);

    if( checkSendAndRecieve(rc, "ERROR in CellularGetICCID") == ERROR) {
        return ERROR;
    }

    strncpy(iccid, readBuf + 7, maxlen);
    return SUCCESS;
}


int CellularGetSignalInfo(SIGNAL_INFO *sigInfo) {
    return SUCCESS;
}


//---------------------------- From here on the functions are new to ex5 ----------------------------------------------


int CellularSetupInternetConnectionProfile(int inact_time_sec){
    int rc;
    rc = sendAndRecv(SICS_CONNTYPE_COMM, OK, SHORT_TIMEOUT_MS);
    if( checkSendAndRecieve(rc, "ERROR in CellularSetupInternetConnectionProfile setting the connection type") == ERROR) {
        return ERROR;
    }

    char *sicsInactCommand;
    asprintf(&sicsInactCommand, SICS_INACT_FORMAT_COMM, inact_time_sec);
    rc = sendAndRecv(sicsInactCommand, OK, SHORT_TIMEOUT_MS);
    free(sicsInactCommand);
    if( checkSendAndRecieve(rc, "ERROR in CellularSetupInternetConnectionProfile setting the inActivation time") == ERROR) {
        return ERROR;
    }

    rc = sendAndRecv(SICS_APN_COMM, OK, SHORT_TIMEOUT_MS);
    if( checkSendAndRecieve(rc, "ERROR in CellularSetupInternetConnectionProfile setting the connection APN") == ERROR) {
        return ERROR;
    }

    return SUCCESS;
}


int CellularSetupInternetServiceSetupProfile(char *IP, int port, int keepintvl_sec){
    int rc;
    rc = sendAndRecv(SISS_SRVTYPE, OK, SHORT_TIMEOUT_MS);
    if( checkSendAndRecieve(rc, "ERROR in CellularSetupInternetServiceSetupProfile setting the service type") == ERROR) {
        return ERROR;
    }

    rc = sendAndRecv(SISS_CONNID, OK, SHORT_TIMEOUT_MS);
    if( checkSendAndRecieve(rc, "ERROR in CellularSetupInternetServiceSetupProfile setting the connection ID") == ERROR) {
        return ERROR;
    }

    char *sissSocktcpCommand;
    asprintf(&sissSocktcpCommand, SISS_SOCKTCP_FORMAT, IP, port, keepintvl_sec);
    rc = sendAndRecv(sissSocktcpCommand, OK, SHORT_TIMEOUT_MS);
    free(sissSocktcpCommand);
    if( checkSendAndRecieve(rc, "ERROR in CellularSetupInternetServiceSetupProfile setting the address") == ERROR) {
        return ERROR;
    }

    return SUCCESS;
}


int CellularConnect(void){
    int rc;
    rc = sendAndRecv(SISO_COMM, OK, SHORT_TIMEOUT_MS);
    if( checkSendAndRecieve(rc, "ERROR in CellularConnect open the internet session defined by the service profile") == ERROR) {
        return ERROR;
    }


    rc = sendAndRecv(SIST_COMM, CONNECT, SHORT_TIMEOUT_MS);
    if( checkSendAndRecieve(rc, "ERROR in CellularConnect setting the connection APN") == ERROR) {
        return ERROR;
    }

    return SUCCESS;
}


int CellularClose(){
    int rc;

    rc = sendAndRecv(SISC_COMM, OK, SHORT_TIMEOUT_MS);
    if( checkSendAndRecieve(rc, "ERROR in CellularClose closing the tcp") == ERROR) {
        return ERROR;
    }

    return SUCCESS;
}


int CellularWrite(unsigned char *payload, unsigned int len){
    return SerialSend(payload, len);
}


int CellularRead(unsigned char *buf, unsigned int max_len, unsigned int timeout_ms){
    unsigned int readCount = 0;
    while( readCount < max_len ){
        unsigned int iterReadCount = 0;
        unsigned int toRead = max_len - readCount;

        iterReadCount = SerialRecv(buf + readCount, toRead, SHORT_TIMEOUT_MS);
        if( checkSendAndRecieve(iterReadCount, "ERROR in CellularRead reading from the socket") == ERROR){
            return ERROR;
        }

        readCount += iterReadCount;

        char *endRecv;
        endRecv = strstr(buf, NOCARRIER);
        if( endRecv != NULL){
            (*endRecv) = '\0';
            return readCount - strlen(NOCARRIER);
        }
    }
    return readCount;
}

