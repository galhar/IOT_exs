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
#define BAUD 115200
#define OPERATOR_NAME_BUF_SIZE 30


typedef struct __OPERATOR_INFO {
    char operatorName[OPERATOR_NAME_BUF_SIZE]; // Long name. See <format> under +COPS.
    int operatorCode; // Short code. See <format> under +COPS.
    char accessTechnology[4]; // "2G" or "3G"
} OPERATOR_INFO;

typedef struct __SIGNAL_INFO {
    int signal_power; // In 2G: dBm. In 3G: RSCP. See ^SMONI responses.
    int EC_n0; // In 3G only. See ^SMONI responses.
    char accessTechnology[4]; // "2G" or "3G"
} SIGNAL_INFO;


int resetModem();


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


//---------------------------- From here on the functions are new to ex5 ----------------------------------------------


/**
 * Initialize an internet connection profile (AT^SICS) with inactTO=inact_time_sec and
 * conType=GPRS0 and apn="postm2m.lu". Return 0 on success, and -1 on failure.
 * @param inact_time_sec
 * @return
 */
int CellularSetupInternetConnectionProfile(int inact_time_sec);


/**
 * Initialize an internal service profile (AT^SISS) with keepintvl=keepintvl_sec (the timer) and
 * SrvType=Socket, and conId=<CellularSetupInternetConnectionProfile_id> (if
 * cellularSetupInternetConnectionProfile is already initialized. Return error, -1, otherwise) and
 * Address=socktcp://IP:port;etx;timer=keepintvl_sec. Return 0 on success, and -1 on failure.
 * @param IP
 * @param port
 * @param keepintvl_sec
 * @return
 */
int CellularSetupInternetServiceSetupProfile(char *IP, int port, int keepintvl_sec);


/**
 * Connects to the socket (establishes TCP connection to the pre-defined host and port).
 *
 * NOT FROM THE EX DESC: this must come after setup connection proile and then setup service profile. In the service
 * profile we define the host and port
 * @return Returns 0 on success, -1 on failure.
 */
int CellularConnect(void);


/**
 * Closes the established connection.
 * @return Returns 0 on success, -1 on failure.
 */
int CellularClose();


/**
 * Writes len bytes from payload buffer to the established connection.
 * Returns the number of bytes written on success, -1 on failure.
 * @param payload
 * @param len
 * @return
 */
int CellularWrite(unsigned char *payload, unsigned int len);


/**
 * Reads up to max_len bytes from the established connection to the provided buf buffer, for up to timeout_ms (doesn’t
 * block longer than that, even if not all max_len bytes were received).
 * @param buf
 * @param max_len
 * @param timeout_ms
 * @return
 */
int CellularRead(unsigned char *buf, unsigned int max_len, unsigned int timeout_ms);


#endif //README_CELLULAR_H
