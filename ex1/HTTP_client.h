//
// Created by osboxes on 11/1/20.
//

#ifndef IOT_EXS_HTTP_CLIENT_H
#define IOT_EXS_HTTP_CLIENT_H

/**
Initializes the socket.
 * Host is the destination address as DNS: en8wtnrvtnkt5.x.pipedream.net, or as IPv4: 35.169.0.97.
 * Port the communication endpoint, as string, e.g.: “80”.
 * Returns 0 on success, -1 on failure. * @param host
 * @param port
 * @return
 */
int HTTPClientInit(char *host, char *port);

/**
 * Writes a simple HTTP GET request to the given URL (usually “/”), and pre-defined host
 * (appears in HTTP body) and port. Reads
 * up to response_max_len bytes from the received response to the provided response buffer.
 * Returns the number of bytes read on success, -1 on failure.
 * @param url
 * @param response
 * @param response_max_len
 * @return
 */
int HTTPClientSendHTTPGetDemoRequest(char *url, char *response, int
response_max_len);

/**
 * Writes a simple HTTP POST request to the given URL (usually “/”), and pre-defined host (appears in HTTP body)
 * and port. In the POST request, send the provided message_len from the message buffer.
 * Reads up to response_max_len bytes from the received response to the provided response buffer.
 * Returns the number of bytes read on success, -1 on failure.
 * @param url
 * @param message
 * @param message_len
 * @param response
 * @param response_max_len
 * @return
 */
int HTTPClientSendHTTPPostDemoRequest(char *url, char *message, unsigned int
message_len, char *response, int response_max_len);


/**
 * Closes any open connections and cleans all the defined variables.
 */
void HTTPClientDeinit(void);

#endif //IOT_EXS_HTTP_CLIENT_H
