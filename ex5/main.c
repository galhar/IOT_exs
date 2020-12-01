//
// Created by osboxes on 11/3/20.
//
#include "HTTP_client.h"
#define MAX_BUFFER 100
#define POST_MESSAGE "hello cellular world!"
#define MESSAGE_LEN 21

int main(int argc, char *argv[]){
    char buffer[MAX_BUFFER];
    if(HTTPClientInit("en8wtnrvtnkt5.x.pipedream.net", "80") == ERROR) {
        return ERROR;
    }

    if(HTTPClientSendHTTPPostDemoRequest("/", POST_MESSAGE, MESSAGE_LEN, buffer, MAX_BUFFER) == ERROR) {
        return ERROR;
    }
    printf("main.c level post 1 request:\n%s\n", buffer);

    if(HTTPClientSendHTTPPostDemoRequest("/", POST_MESSAGE, MESSAGE_LEN, buffer, MAX_BUFFER) == ERROR) {
        return ERROR;
    }
    printf("main.c level post 2 request:\n%s\n", buffer);

    if(HTTPClientSendHTTPGetDemoRequest("/", buffer, MAX_BUFFER) == ERROR) {
        return ERROR;
    }
    printf("main.c level get 1 request:\n%s\n", buffer);

    if(HTTPClientSendHTTPGetDemoRequest("/", buffer, MAX_BUFFER) == ERROR) {
        return ERROR;
    }
    printf("main.c level get 2 request:\n%s\n", buffer);


    if(HTTPClientSendHTTPPostDemoRequest("/", POST_MESSAGE, MESSAGE_LEN, buffer, MAX_BUFFER) == ERROR) {
        return ERROR;
    }
    printf("main.c level post 3 request:\n%s\n", buffer);

    HTTPClientDeinit();
    return SUCCESS;
}
