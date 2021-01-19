//
// Created by osboxes on 11/1/20.
//
#include "socket.h"
#include <stdio.h>
#include <arpa/inet.h>


int connectToOpp(){
    char iccid[MAX_ICCID];
    OPERATOR_INFO oplist[MAX_OPP_LIST];
    char charOppCode[MAX_OPP_CODE];
    int opsFound = 0;
    int status = 0;
    int sqt = 0;
    printf("-------------------------------\nStarting device init\n");
    if(CellularInit(PORT) == ERROR) {
        perror("Error - can not init device\n");
        return ERROR;
    }
    else{
        printf("Device init completed\n-------------------------------\n");
    }

    printf("-------------------------------\nStarting AT command check\n");
    if(CellularCheckModem() == ERROR) {
        perror("Error - can not send AT commands\n");
        return ERROR;
    }
    else{
        printf("AT command check completed\n-------------------------------\n");
    }

    printf("-------------------------------\nStarting serch for ICCID\n");
    if(CellularGetICCID(iccid, MAX_ICCID) == ERROR) {
        perror("Error - can not get ICCID\n");
        return ERROR;
    }
    else{
        printf("Found ICCID: %s\n-------------------------------\n", iccid);
    }

    printf("-------------------------------\nStarting serch for Operators\n");
    if(CellularGetOperators(oplist, MAX_OPP_LIST, &opsFound) == ERROR) {
        perror("Error - can not get operators\n");
        return ERROR;
    }
    else{
        printf("Getting all operators completed\n-------------------------------\n");
    }

    printf("-------------------------------\nTrying to connect to the oppeartors!\n");
    for(int i = 0; i < opsFound; i++) {
        printf("Trying to coonect to %s opperator\n", oplist[i].operatorName);
        sprintf(charOppCode, "%d", oplist[i].operatorCode);
        if(CellularSetOperator(1, charOppCode) == ERROR ) {
            perror("Error - set operator failed\n");
            continue;

        }
        if( CellularGetRegistrationStatus(&status) == ERROR ){
            perror("Error - getRegStatus failed");
            return ERROR;
        }
        else if(status == 1 || status == 5) {
            printf("Connected!\n-------------------------------\n");
            break;
        }
        else {
            printf("Failed to connect to specific operator\n");
        }
        if(i == opsFound - 1) {
            perror("Can not connect to any operator!\n");
            return ERROR;
        }
    }

    if(CellularGetSignalQuality(&sqt) == ERROR) {
        perror("Error - can not get signal squality\n");
        return ERROR;
    }
    else{
        sqt = 113 - 2* sqt;
        printf("Signal quality is -%d DB\n",  sqt);
    }
    return SUCCESS;
}


int SocketInit(char *host, char *port) {
    struct hostent* he;
    struct in_addr a;
    char hostIP[100];
    /* resolve hostname */
    if ( (he = gethostbyname(host) ) == NULL ) {
        perror("can not translate HOST to IP");
        return ERROR;
    }
    bcopy(*he->h_addr_list++, (char *) &a, sizeof(a));
    printf("address: %s\n", inet_ntoa(a));
    strcpy(hostIP, inet_ntoa(a));

    if(connectToOpp() == ERROR) {
        perror("Error - can not connect to the oppeartor\n");
        return ERROR;
    }

    if (CellularSetupInternetConnectionProfile(20) == ERROR)
    {
        perror("Cellular setup connection profile failed");
        return ERROR;
    }

    if (CellularSetupInternetServiceSetupProfile(hostIP, (atoi(port)),200) == ERROR)
    {
        perror("Cellular setup service profile failed");
        return ERROR;
    }
    return SUCCESS;
}


int SocketConnect(void) {
    if(CellularConnect() != SUCCESS) {
        perror("Failed to connect the socket");
        return ERROR;
    }
    return SUCCESS;
}


int SocketWrite(unsigned char *payload, unsigned int len) {
    return CellularWrite(payload, len);
}


int SocketRead(unsigned char *buf, unsigned int max_len, unsigned int timeout_ms) {
    return CellularRead(buf, max_len, timeout_ms);
}


int SocketClose() {
    if(CellularClose() != SUCCESS) {
        perror("Failed to connect the socket");
        return ERROR;
    }
    CellularDisable();
    return SUCCESS;
}
