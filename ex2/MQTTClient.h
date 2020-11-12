//
// Created by osboxes on 11/12/20.
//

#ifndef EX1_MQTTCLIENT_H
#define EX1_MQTTCLIENT_H
#define "wolfmqtt/mqttexample.h"
/**
 * Initializes the MqttNet struct with the previously defined callbacks (NetConnect, NetRead, NetWrite, NetDisconnect)
 * and allocates (if needed) structs, buffers, static variables etc.
 * Usage of MQTTCtx (defined in wolfMQTT/examples/mqttexample.h) is optional and can be replaced with any other struct or set of variables
 * @param net the struct to init
 * @param mqttCtx helper struct
 * @return 0 on success, -1 on failure
 */
int MqttClientNet_Init(MqttNet* net, MQTTCtx* mqttCtx);

/**
 * De-Initializes the MqttNet struct and all previously allocated structs, buffers, static variables etc.
 * @param net the struct to de-init
 * @return 0 on success, -1 on failure
 */
int MqttClientNet_DeInit(MqttNet* net);
#endif //EX1_MQTTCLIENT_H
