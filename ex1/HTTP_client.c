//
// Created by osboxes on 11/1/20.
//

#include "HTTP_client.h"


char *httpHost;
char *httpPort;
char *httpGETBody = "GET %s HTTP/1.1\r\n"
                    "Host: %s\r\n"
                    "\r\n";
char *httpPOSTBody = "POST %s HTTP/1.1\r\n"
                     "Host: %s\r\n"
                     "Content-type: application/x-www-form-urlencoded\r\n"
                     "Content-length: %d\r\n\r\n"
                     "%s\r\n";

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
        perror("HTTP send GET error: connect the socket");
        return ERROR;
    }

    // enough space to contain the message, without the "%s"x2 and with the host and url
    char *tmpGETMsg = malloc(strlen(httpGETBody) + +strlen(httpHost) + strlen(url) - 4);
    sprintf(tmpGETMsg, httpGETBody, url, httpHost);

    SocketWrite(tmpGETMsg, strlen(tmpGETMsg));

    free(tmpGETMsg);

    // now read the return message, and parse out the content
    int HTTPResponseBufLen = MAX_HTTP_RESPONSE_HEADER_LENGTH + response_max_len;
    char *HTTPResponseBuf = malloc(HTTPResponseBufLen);
    int readBytes = SocketRead(HTTPResponseBuf,HTTPResponseBufLen, HTTP_RESPONSE_TIMEOUT);
    if(strstr())

}


int HTTPClientSendHTTPPostDemoRequest(char *url, char *message, unsigned int message_len, char *response,
                                      int response_max_len) {

}


void HTTPClientDeinit(void) {
    free(httpHost);
    free(httpPort);
    if (SocketClose() == ERROR) {
        perror("HTTP deinitalization error: while closing the socket");
    }
}
