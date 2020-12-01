//
// Created by osboxes on 11/1/20.
//
#include "socket.h"

int socket_sd;
struct sockaddr_in address;


int SocketInit(char *host, char *port) {
    struct hostent* he;

    if ((socket_sd = socket(AF_INET, SOCK_STREAM, 0)) == ERROR)
    {
        perror("socket failed");
        return ERROR;
    }

    /* resolve hostname */
    if ( (he = gethostbyname(host) ) == NULL ) {
        perror("can not translate HOST to IP");
        return ERROR;
    }

    address.sin_family = AF_INET;
    memcpy(&address.sin_addr, he->h_addr, he->h_length);
    address.sin_port = htons(atoi(port));
    bzero(&(address.sin_zero),8);

    return SUCCESS;
}


int SocketConnect(void) {
    if(connect(socket_sd, (struct sockaddr *)&address ,sizeof(struct sockaddr)) != SUCCESS) {
        perror("Failed to connect the socket");
        return ERROR;
    }
    return SUCCESS;
}


int SocketWrite(unsigned char *payload, unsigned int len) {
    return send(socket_sd, payload, len, 0);
}


int SocketRead(unsigned char *buf, unsigned int max_len, unsigned int timeout_ms) {
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = timeout_ms * 1000;
    setsockopt(socket_sd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
    return recv(socket_sd, buf, max_len, 0);
}


int SocketClose() {
    if(close(socket_sd) != SUCCESS) {
        perror("Failed to connect the socket");
        return ERROR;
    }
    return SUCCESS;
}
