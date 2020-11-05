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
                     "Content-type: text/plain\r\n"
                     "Cache-Control: no-cache\r\n"
                     "Content-length: %d\r\n\r\n"
                     "%s\r\n";
char *httpResponseHeaderSuffix = "\r\n\r\n";

int connectHttpSocket() {
    if (SocketConnect() == ERROR) {
        perror("HTTP send GET error: connect the socket");
        return ERROR;
    }
}


int parseHttpResponse(char* response, int response_max_len){
    int httpResponseBufLen = MAX_HTTP_RESPONSE_HEADER_LENGTH + response_max_len;
    char *httpResponseBuf = malloc(httpResponseBufLen);
    int readBytes = SocketRead(httpResponseBuf, httpResponseBufLen, HTTP_RESPONSE_TIMEOUT);
    if (readBytes == ERROR){
        perror("HTTP get error: while reading response from the socket");
        return ERROR;
    }
    // print for debugging
    printf("HTTP RESPONSE got:\n%s\n", httpResponseBuf);

    // get the beginning of the contetn
    char *responseSuffix = strstr(httpResponseBuf, httpResponseHeaderSuffix);
    if (responseSuffix == NULL) {
        perror("HTTP get error: the response in bad format");
        return ERROR;
    }

    // return min length between the string got and the defined max length
    int responseLen = strlen(responseSuffix) - strlen(httpResponseHeaderSuffix);
    if (response_max_len < responseLen) {
        responseLen = response_max_len;
    }
    strncpy(response, responseSuffix + strlen(responseSuffix), responseLen);

    free(httpResponseBuf);

    return responseLen;

}


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
    if (connectHttpSocket() == ERROR) {
        return ERROR;
    }

    // enough space to contain the message, without the "%s"x2 and with the host and url
    char *tmpGETMsg = malloc(strlen(httpGETBody) + +strlen(httpHost) + strlen(url) - 4);
    sprintf(tmpGETMsg, httpGETBody, url, httpHost);

    SocketWrite(tmpGETMsg, strlen(tmpGETMsg));

    free(tmpGETMsg);

    // Now Read the RESPONSE and parse out the content
    return parseHttpResponse(response, response_max_len);

}


int HTTPClientSendHTTPPostDemoRequest(char *url, char *message, unsigned int message_len, char *response,
                                      int response_max_len) {
    if (connectHttpSocket() == ERROR) {
        return ERROR;
    }

    // Write the POST message
    // enough space to contain the message, without the "%s"x4 and with the host, url, message and message_len
    char *tmpPOSTMsg = malloc(strlen(httpGETBody) + +strlen(httpHost) + strlen(url) - 4);
    sprintf(tmpPOSTMsg, httpPOSTBody, url, httpHost, message_len, message);

    SocketWrite(tmpPOSTMsg, strlen(tmpPOSTMsg));

    free(tmpPOSTMsg);

    // Get the RESPONSE
    return parseHttpResponse(response, response_max_len);
}


void HTTPClientDeinit(void) {
    free(httpHost);
    free(httpPort);
    if (SocketClose() == ERROR) {
        perror("HTTP deinitalization error: while closing the socket");
    }
}
