//
// Created by osboxes on 11/1/20.
//
#include "socket.h"

int server_fd;
struct sockaddr_in address;


int SocketInit(char *host, char *port) {
    struct hostent* he;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
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
    memcpy(&address.sin_addr, he->h_addr_list[0], he->h_length);
    address.sin_port = atoi(port);

    return SUCCESS;
}


int SocketConnect(void) {
    int addrlen = sizeof(address);
    if(connect(server_fd, &address, addrlen) != SUCCESS) {
        perror("Failed to connect the socket");
        return ERROR;
    }
    return SUCCESS;
}


int SocketWrite(unsigned char *payload, unsigned int len) {
    return send(server_fd, payload, len, 0);
}


int SocketRead(unsigned char *buf, unsigned int max_len, unsigned int timeout_ms) {
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = timeout_ms * 1000;
    setsockopt(server_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
    return recv(server_fd, buf, max_len, 0);
}


int SocketClose() {
    if(close(server_fd) != SUCCESS) {
        perror("Failed to connect the socket");
        return ERROR;
    }
    return SUCCESS;
}
