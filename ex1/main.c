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

    if(HTTPClientSendHTTPGetDemoRequest("/", buffer, MAX_BUFFER) == ERROR) {
        return ERROR;
    }
    printf("main.c level get request:\n%s", buffer);


    if(HTTPClientSendHTTPPostDemoRequest("/", POST_MESSAGE, MESSAGE_LEN, buffer, MAX_BUFFER) == ERROR) {
        return ERROR;
    }
    printf("main.c level post request:\n%s", buffer);

    HTTPClientDeinit();
    return SUCCESS;
}
