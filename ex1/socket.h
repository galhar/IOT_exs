//
// Created by osboxes on 11/1/20.
//

#ifndef IOT_EXS_SOCKET_H
#define IOT_EXS_SOCKET_H
/**
 * Initializes the socket
 * Host is the destination address as DNS: en8wtnrvtnkt5.x.pipedream.net, or as IPv4: 35.169.0.97
 * Port the communication endpoint, as string, e.g.: “80”
 * Returns 0 on success, -1 on failure
 * @param host
 * @param port
 * @return
 */
int SocketInit(char *host, char *port);


int SocketConnect(void);


int SocketWrite(unsigned char *payload, unsigned int len);



int SocketRead(unsigned char *buf, unsigned int max_len, unsigned int
timeout_ms);


int SocketClose();

#endif //IOT_EXS_SOCKET_H
