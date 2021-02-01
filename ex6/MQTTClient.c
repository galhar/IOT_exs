//
// Created by osboxes on 11/12/20.
//

#include "MQTTClient.h"

/**
 * Connects the socket to the broker, to the given host and port.
 * @param context
 * @param host defines host to connect
 * @param port defines port to connect
 * @param timeout_ms defines the timeout in milliseconds.
 * @return 0 on success, and a negative number otherwise (one of MqttPacketResponseCodes,
 * defined in wolfMQTT/wolfmqtt/mqtt_types.h).
 */
static int NetConnect(void *context, const char* host, word16 port, int timeout_ms) {
    char bufferPort[16];
    int rc;
    snprintf(bufferPort, 16, "%d", port);
    if( SocketInit(host, bufferPort) == ERROR){
    	return ERROR;
    }
    SocketConnect();
}

/**
 * Performs a network (socket) read from the connected broker, to the given buffer buf, and reads buf_len bytes.
 * @param context
 * @param buf buffer to write the data
 * @param buf_len number of bytes to read
 * @param timeout_ms defines the timeout in milliseconds.
 * @return number of read bytes on success, and a negative number otherwise (one of MqttPacketResponseCodes,
 * defined in wolfMQTT/wolfmqtt/mqtt_types.h).
 */
static int NetRead(void *context, byte* buf, int buf_len, int timeout_ms)
{
    SocketRead(buf, buf_len, timeout_ms);
}

/**
 * Performs a network (socket) write to the connected broker, from the given buffer buf, with size of buf_len.
 * @param context
 * @param buf buffer to write from
 * @param buf_len size in bytes to write
 * @param timeout_ms defines the timeout in milliseconds
 * @return the number of sent bytes on success, and a negative number otherwise
 * (one of MqttPacketResponseCodes, defined in wolfMQTT/wolfmqtt/mqtt_types.h).
 */
static int NetWrite(void *context, const byte* buf, int buf_len, int timeout_ms) {
    SocketWrite(buf, buf_len);
}

/**
 * Closes the network (socket) connection to the connected broker
 * @param context
 * @return number of sent bytes on success, and a negative number otherwise (one of MqttPacketResponseCodes,
 * defined in wolfMQTT/wolfmqtt/mqtt_types.h).
 */
static int NetDisconnect(void *context)
{
    SocketClose();
}

/**
 * Initializes the MqttNet struct with the previously defined callbacks (NetConnect, NetRead, NetWrite, NetDisconnect)
 * and allocates (if needed) structs, buffers, static variables etc.
 * Usage of MQTTCtx (defined in wolfMQTT/examples/mqttexample.h) is optional and can be replaced with any other struct or set of variables
 * @param net the struct to init
 * @param mqttCtx helper struct
 * @return 0 on success, -1 on failure
 */
int MqttClientNet_Init(MqttNet* net, MQTTCtx* mqttCtx) {
    net->connect = NetConnect;
    net->disconnect = NetDisconnect;
    net->read = NetRead;
    net->write = NetWrite;
    return SUCCESS;
}

/**
 * De-Initializes the MqttNet struct and all previously allocated structs, buffers, static variables etc.
 * @param net the struct to de-init
 * @return 0 on success, -1 on failure
 */
int MqttClientNet_DeInit(MqttNet* net) {
    return SUCCESS;
}
