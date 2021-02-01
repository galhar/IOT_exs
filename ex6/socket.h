//
// Created by osboxes on 11/1/20.
//

#ifndef IOT_EXS_SOCKET_H
#define IOT_EXS_SOCKET_H
#define ERROR -1
#define SUCCESS 0
#define MAX_ICCID 30
#define MAX_OPP_LIST 30
#define PORT "/dev/ttyS1"
#define MAX_OPP_CODE 10

#include "cellular.h"
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


/**
 * Initializes the socket.
 * Host is the destination address as DNS: en8wtnrvtnkt5.x.pipedream.net, or as IPv4: 35.169.0.97
 * Port the communication endpoint, as string, e.g.: “80”
 * Returns 0 on success, -1 on failure
 * @param host
 * @param port
 * @return
 */
int SocketInit(char *host, char *port);


/**
 * Connects to the socket (establishes TCP connection to the pre-defined host and port)
 * Returns 0 on success, -1 on failure.
 * @return
 */
int SocketConnect(void);


/**
 * Writes len bytes from payload buffer to the established connection
 * Returns the number of bytes written on success, -1 on failure.
 * @param payload
 * @param len
 * @return
 */
int SocketWrite(unsigned char *payload, unsigned int len);


/**
 * Reads up to max_len bytes from the established connection to the provided buf buffer, for up to timeout_ms
 * (doesn’t block longer than that, even if not all max_len bytes were received)
 * Returns the number of bytes read on success, -1 on failure.
 * @param buf
 * @param max_len
 * @param timeout_ms
 * @return
 */
int SocketRead(unsigned char *buf, unsigned int max_len, unsigned int timeout_ms);


/**
 * Closes the established connection
 * Returns 0 on success, -1 on failure.
 * @return
 */
int SocketClose();

#endif //IOT_EXS_SOCKET_H
