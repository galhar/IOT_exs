// Created by osboxes on 11/16/20.
//

#include "cellular.h"
#define PORT "/dev/ttyS1"
#define MAX_ICCID 20
#define MAX_OPP_LIST 30
#define MAX_OPP_CODE 10
int main(int argc, char *argv[]) {
    char iccid[MAX_ICCID];
    OPERATOR_INFO oplist[MAX_OPP_LIST];
    char charOppCode[MAX_OPP_CODE];
    int opsFound = 0;
    int status = 0;
    if(CellularInit(PORT) == ERROR) {
        perror("Error - can not init device");
        return ERROR;
    }
    else{
        printf("Device init completed");
    }

    if(CellularCheckModem() == ERROR) {
        perror("Error - can not send AT commands");
        return ERROR;
    }
    else{
        printf("AT command check completed");
    }

    if(CellularGetICCID(iccid, MAX_ICCID) == ERROR) {
        perror("Error - can not get ICCID");
        return ERROR;
    }
    else{
        printf("Found ICCID: %s", iccid);
    }

    if(CellularGetOperators(oplist, MAX_OPP_LIST, &opsFound) == ERROR) {
        perror("Error - can not get operators");
        return ERROR;
    }
    else{
        printf("Getting all operators completed");
    }

    for(int i = 0; i < opsFound; i++) {
        printf("Trying to coonect to %s opperator", oplist[i].operatorName);
        sprintf(charOppCode, "%d", oplist[i].operatorCode);
        if(CellularSetOperator(1, charOppCode) == ERROR || CellularGetRegistrationStatus(&status) == ERROR) {
            perror("Error - set operator failed");
            return ERROR;

        }
        else if(status == 1 || status == 5) {
            printf("Connected!");
            break;
        }
        else {
            printf("Failed to connect to specific operator");
        }
        if(i == opsFound - 1) {
            perror("Can not connect to any operator!");
            return ERROR;
        }
    }






}
