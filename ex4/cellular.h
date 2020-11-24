//
// Created by osboxes on 11/24/20.
//

#ifndef README_CELLULAR_H
#define README_CELLULAR_H

#include "serial_io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PORT_LEN 16
#define ERROR -1
#define SUCCESS 0
#define BAUD B115200

typedef struct __OPERATOR_INFO {
    char operatorName[10]; // Long name. See <format> under +COPS.
    int operatorCode; // Short code. See <format> under +COPS.
    char accessTechnology[4]; // "2G" or "3G"
} OPERATOR_INFO;

typedef struct __SIGNAL_INFO {
    int signal_power; // In 2G: dBm. In 3G: RSCP. See ^SMONI responses.
    int EC_n0; // In 3G only. See ^SMONI responses.
    char accessTechnology[4]; // "2G" or "3G"
} SIGNAL_INFO;


/**
 * Initialize whatever is needed to start working with the cellular modem (e.g. the serial
 * port). Returns 0 on success, and -1 on failure.
 * @param port
 * @return
 */
int CellularInit(char *port);


/**
 * Deallocate / close whatever resources CellularInit() allocated.
 */
void CellularDisable(void);


/**
 * Checks if the modem is responding to AT commands. Return 0 if it does, returns -1
 * otherwise.
 * @return
 */
int CellularCheckModem(void);


/**
 * Returns -1 if the modem did not respond or respond with an error.
 * Returns 0 if the command was successful and the registration status was obtained from
 * the modem. In that case, the status parameter will be populated with the numeric value
 * of the <regStatus> field of the “+CREG” AT command.
 * @param status
 * @return
 */
int CellularGetRegistrationStatus(int *status);


/**
 * Forces the modem to search for available operators (see “+COPS=?” command). Returns -1
 * if an error occurred or no operators found. Returns 0 and populates opList and opsFound if
 * the command succeeded
 * @param opList opList is a pointer to the first item of an array of type OPERATOR_INFO, which is allocated
 * by the caller of this function. The array contains a total of maxops items. numOpsFound is
 * allocated by the caller and set by the function. numOpsFound will contain the number of
 * operators found and populated into the opList.
 * @param maxops
 * @param numOpsFound
 * @return
 */
int CellularGetOperators(OPERATOR_INFO *opList, int maxops, int *numOpsFound);


/**
 * Forces the modem to register/deregister with a network. Returns 0 if the
 * command was successful, returns -1 otherwise.
 * @param mode If mode=0, sets the modem to automatically register with an operator (ignores the
 * operatorCode parameter).
 * If mode=1, forces the modem to work with a specific operator, given in
 * operatorCode.
 * If mode=2, deregisters from the network (ignores the operatorCode parameter).
 * See the “+COPS=<mode>,…” command for more details.
 * @param operatorCode
 * @return
 */
int CellularSetOperator(int mode, char *operatorCode);


/**
 * Returns -1 if the modem did not respond or respond with an error (note, CSQ=99
 * is also an error!)
 * Returns 0 if the command was successful and the signal quality was obtained from
 * the modem. In that case, the csq parameter will be populated with the numeric
 * value between -113dBm and -51dBm.
 * @param csq
 * @return
 */
int CellularGetSignalQuality(int *csq);


/**
 * Returns -1 if the modem did not respond or respond with an error.
 * Returns 0 if the command was successful and the ICCID was obtained from the modem
 * @param iccid iccid is a pointer to a char buffer, which is allocated by the caller
 * of this function. The buffer size is maxlen chars. The obtained ICCID will be placed
 * into the iccid buffer as a null-terminated string.
 * @param maxlen
 * @return
 */
int CellularGetICCID(char *iccid, int maxlen);


/**
 * Returns -1 if the modem did not respond, respond with an error, respond with SEARCH or NOCONN.
 * Returns 0 if the command was successful and the signal info was obtained from the modem.
 * @param sigInfo sigInfo is a pointer to a struct, which is allocated by the caller of this function.
 * The obtained info will be placedinto the sigInfo struct.
 * @return
 */
int CellularGetSignalInfo(SIGNAL_INFO *sigInfo);

#endif //README_CELLULAR_H
