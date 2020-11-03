//
// Created by osboxes on 11/1/20.
//
#include "socket.h"
#include "HTTP_client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *httpHost;
char *httpPort;

int HTTPClientInit(char *host, char *port) {
    // save the host and port for internal usage
    httpHost = malloc(strlen(host) + 1);
    strcpy(httpHost, host);

    httpPort = malloc(strlen(port) + 1);
    strcpy(httpPort, port);

    if (SocketInit(host, port) == ERROR) {
        perror("HTTP client init error initializing the socket");
        return ERROR;
    }
    return SUCCESS;
}


int HTTPClientSendHTTPGetDemoRequest(char *url, char *response, int response_max_len) {
    if (SocketConnect() == ERROR) {
        perror("HTTP send GET error connect the socket");
        return ERROR;
    }


}


int HTTPClientSendHTTPPostDemoRequest(char *url, char *message, unsigned int message_len, char *response,
                                      int response_max_len) {

}


void HTTPClientDeinit(void) {
    free(httpHost);
    free(httpPort);
}
