// Created by osboxes on 11/16/20.
//

#include "cellular.h"
#define PORT "/dev/ttyS0"
#define MAX_ICCID 30
#define MAX_OPP_LIST 30
#define MAX_OPP_CODE 10
int main(int argc, char *argv[]) {
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
            if(i == opsFound - 1) {
                perror("Can not connect to any operator!\n");
                return ERROR;
            }

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
    }

    if(CellularGetSignalQuality(&sqt) == ERROR) {
        perror("Error - can not get signal squality\n");
        return ERROR;
    }
    else{
        sqt = 113 - 2* sqt;
        printf("Signal quality is -%d DBm\n",  sqt);
    }
    return SUCCESS;
}
