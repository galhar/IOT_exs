//
// Created by osboxes on 11/12/20.
//

#ifndef EX1_MQTTCLIENT_H
#define EX1_MQTTCLIENT_H
#include "wolfmqtt/mqtt_types.h"
#include "wolfmqtt/mqtt_socket.h"
#include "socket.h"
#include "wolfmqtt/mqtt_packet.h"
#include "wolfmqtt/mqtt_client.h"
#include <stdio.h>
#include <stdlib.h>

/* MQTT Client state */
typedef enum _MQTTCtxState {
    WMQ_BEGIN = 0,
    WMQ_NET_INIT,
    WMQ_INIT,
    WMQ_TCP_CONN,
    WMQ_MQTT_CONN,
    WMQ_SUB,
    WMQ_PUB,
    WMQ_WAIT_MSG,
    WMQ_UNSUB,
    WMQ_DISCONNECT,
    WMQ_NET_DISCONNECT,
    WMQ_DONE
} MQTTCtxState;


/* MQTT Client context */
/* This is used for the examples as reference */
/* Use of this structure allow non-blocking context */
typedef struct _MQTTCtx {
    MQTTCtxState stat;

    void* app_ctx; /* For storing application specific data */

    /* client and net containers */
    MqttClient client;
    MqttNet net;

    /* temp mqtt containers */
    MqttConnect connect;
    MqttMessage lwt_msg;
    MqttSubscribe subscribe;
    MqttUnsubscribe unsubscribe;
    MqttTopic topics[1];
    MqttPublish publish;
    MqttDisconnect disconnect;
    MqttPing ping;
#ifdef WOLFMQTT_SN
    SN_Publish publishSN;
#endif

    /* configuration */
    MqttQoS qos;
    const char* app_name;
    const char* host;
    const char* username;
    const char* password;
    const char* topic_name;
    const char* message;
    const char* pub_file;
    const char* client_id;
    byte *tx_buf, *rx_buf;
    int return_code;
    int use_tls;
    int retain;
    int enable_lwt;
#ifdef WOLFMQTT_V5
    int      max_packet_size;
#endif
    word32 cmd_timeout_ms;
#if defined(WOLFMQTT_NONBLOCK)
    word32  start_sec; /* used for keep-alive */
#endif
    word16 keep_alive_sec;
    word16 port;
#ifdef WOLFMQTT_V5
    word16  topic_alias;
    word16  topic_alias_max; /* Server property */
#endif
    byte    clean_session;
    byte    test_mode;
#ifdef WOLFMQTT_V5
    byte    subId_not_avail; /* Server property */
    byte    enable_eauth; /* Enhanced authentication */
#endif
    unsigned int dynamicTopic:1;
    unsigned int dynamicClientId:1;
#ifdef WOLFMQTT_NONBLOCK
    unsigned int useNonBlockMode:1; /* set to use non-blocking mode.
        network callbacks can return MQTT_CODE_CONTINUE to indicate "would block" */
#endif
} MQTTCtx;

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
