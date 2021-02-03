//
// Created by osboxes on 11/12/20.
//
#include "MQTTClient.h"
#include <time.h>
#include <stdio.h>
#define DEFAULT_MQTT_HOST "35.156.182.231"
#define DEFAULT_MQTT_PORT 1883
#define DEFAULT_MQTT_QOS 1
#define DEFAULT_KEEP_ALIVE_SEC 60
#define DEFAULT_CMD_TIMEOUT_MS 30000
#define DEFAULT_CON_TIMEOUT_MS  5000
#define DEFAULT_TOPIC_NAME "huji_iot_class/2020_2021"
#define DEFAULT_CLIENT_ID "galDov"
#define ERROR_PREFIX_TEST_FUNC "mqttclient_test ERROR: "

#define MAX_BUFFER_SIZE 100
#define PRINT_BUFFER_SIZE 100

#define MAX_ICCID 30
#define MAX_OPP_CODE 4
#define MAX_OPP_NAME 30

char iccid[MAX_ICCID];
char charOppCode[MAX_OPP_CODE];
char charOppName[MAX_OPP_NAME];
char charOppTech[4];
int csq;

char *payloadTemplate = "{\n\"ICCID\":\"%s\",\n"
                        "\"OperatorName\":\"%s\",\n"
                        "\"OperatorCode\":\"%s\",\n"
                        "\"Technology\":\"%s\",\n"
                        "\"Signal\":%d,\n"
                        "\"Clicks\":%d\n}";

static const char *kDefTopicName = DEFAULT_TOPIC_NAME;
static const char *kDefClientId = DEFAULT_CLIENT_ID;

static volatile word16 mPacketIdLast;

int mqtt_tls_cb(MqttClient *client) {
    (void) client;
    return 0;
}

word16 mqtt_get_packetid(void) {
    /* Check rollover */
    if (mPacketIdLast >= MAX_PACKET_ID) {
        mPacketIdLast = 0;
    }

    return ++mPacketIdLast;
}

int mqtt_init_ctx(MQTTCtx *mqttCtx) {
    XMEMSET(mqttCtx, 0, sizeof(MQTTCtx));
    mqttCtx->host = DEFAULT_MQTT_HOST;
    mqttCtx->port = DEFAULT_MQTT_PORT;
    mqttCtx->qos = DEFAULT_MQTT_QOS;
    mqttCtx->clean_session = 0;
    mqttCtx->keep_alive_sec = DEFAULT_KEEP_ALIVE_SEC;
    mqttCtx->client_id = kDefClientId;
    mqttCtx->topic_name = kDefTopicName;
    mqttCtx->cmd_timeout_ms = DEFAULT_CMD_TIMEOUT_MS;

    return MQTT_CODE_SUCCESS;
}

int definePayload(MQTTCtx *mqttCtx){
    (*iccid) = 'i';
    (*charOppCode) = 'c';
    (*charOppName) = 'n';
    (*charOppTech) = 't';
    csq = 125;

    char* messagePayload;
    if(0 > asprintf(&messagePayload, payloadTemplate, iccid, charOppName, charOppCode, charOppTech, csq, 3)){
        printf("mqtt_init_ctx ERROR: during initializing of the payload message");
        return EXIT_FAILURE;
    }
    mqttCtx->message = messagePayload;
    return SUCCESS;
}

void mqtt_free_ctx(MQTTCtx* mqttCtx)
{
    if (mqttCtx == NULL) {
        return;
    }

    if (mqttCtx->message){
        free(mqttCtx->message);
    }
}

static int mqtt_message_cb(MqttClient *client, MqttMessage *msg,
                           byte msg_new, byte msg_done) {
    byte buf[PRINT_BUFFER_SIZE + 1];
    word32 len;
    MQTTCtx *mqttCtx = (MQTTCtx *) client->ctx;

    (void) mqttCtx;

    if (msg_new) {
        /* Determine min size to dump */
        len = msg->topic_name_len;
        if (len > PRINT_BUFFER_SIZE) {
            len = PRINT_BUFFER_SIZE;
        }
        XMEMCPY(buf, msg->topic_name, len);
        buf[len] = '\0'; /* Make sure its null terminated */

        /* Print incoming message */
        printf("MQTT Message: Topic %s, Qos %d, Len %u",
               buf, msg->qos, msg->total_len);

    }

    /* Print message payload */
    len = msg->buffer_len;
    if (len > PRINT_BUFFER_SIZE) {
        len = PRINT_BUFFER_SIZE;
    }
    XMEMCPY(buf, msg->buffer, len);
    buf[len] = '\0'; /* Make sure its null terminated */
    printf("Payload (%d - %d): %s",
           msg->buffer_pos, msg->buffer_pos + len, buf);

    if (msg_done) {
        printf("MQTT Message: Done");
    }

    /* Return negative to terminate publish processing */
    return MQTT_CODE_SUCCESS;
}

int mqttclient_test(MQTTCtx *mqttCtx) {
    int rc = MQTT_CODE_SUCCESS, i;

    printf("MQTT Client: QoS %d, Use TLS %d", mqttCtx->qos,
           mqttCtx->use_tls);

    /* Initialize Network */
    rc = MqttClientNet_Init(&mqttCtx->net, mqttCtx);
    printf("MQTT Net Init: %s (%d)",
           MqttClient_ReturnCodeToString(rc), rc);
    if (rc != MQTT_CODE_SUCCESS) {
        printf(ERROR_PREFIX_TEST_FUNC "MqttClientNet_Init Error");
        goto exit;
    }

    /* setup tx/rx buffers */
    mqttCtx->tx_buf = (byte *) WOLFMQTT_MALLOC(MAX_BUFFER_SIZE);
    mqttCtx->rx_buf = (byte *) WOLFMQTT_MALLOC(MAX_BUFFER_SIZE);

    /* Initialize MqttClient structure */
    rc = MqttClient_Init(&mqttCtx->client, &mqttCtx->net,
                         mqtt_message_cb,
                         mqttCtx->tx_buf, MAX_BUFFER_SIZE,
                         mqttCtx->rx_buf, MAX_BUFFER_SIZE,
                         mqttCtx->cmd_timeout_ms);

    printf("MQTT Init: %s (%d)",
           MqttClient_ReturnCodeToString(rc), rc);
    if (rc != MQTT_CODE_SUCCESS) {
        printf(ERROR_PREFIX_TEST_FUNC "MqttClient_Init Error");
        goto exit;
    }


    /* Connect to broker */
    rc = MqttClient_NetConnect(&mqttCtx->client, mqttCtx->host,
                               mqttCtx->port,
                               DEFAULT_CON_TIMEOUT_MS, mqttCtx->use_tls, mqtt_tls_cb);

    printf("MQTT Socket Connect: %s (%d)",
           MqttClient_ReturnCodeToString(rc), rc);
    if (rc != MQTT_CODE_SUCCESS) {
        printf(ERROR_PREFIX_TEST_FUNC "MqttClient_NetConnect Error");
        goto exit;
    }

    /* Build connect packet */
    XMEMSET(&mqttCtx->connect, 0, sizeof(MqttConnect));
    mqttCtx->connect.keep_alive_sec = mqttCtx->keep_alive_sec;
    mqttCtx->connect.clean_session = mqttCtx->clean_session;
    mqttCtx->connect.client_id = mqttCtx->client_id;


    /* Send Connect and wait for Connect Ack */
    rc = MqttClient_Connect(&mqttCtx->client, &mqttCtx->connect);

    printf("MQTT Connect: Proto (%s), %s (%d)",
           MqttClient_GetProtocolVersionString(&mqttCtx->client),
           MqttClient_ReturnCodeToString(rc), rc);
    if (rc != MQTT_CODE_SUCCESS) {
        printf(ERROR_PREFIX_TEST_FUNC "MqttClient_Connect Error");
        //goto disconn;
        goto exit;
    }

    /* Validate Connect Ack info */
    printf("MQTT Connect Ack: Return Code %u, Session Present %d",
           mqttCtx->connect.ack.return_code,
           (mqttCtx->connect.ack.flags &
            MQTT_CONNECT_ACK_FLAG_SESSION_PRESENT) ?
           1 : 0
    );

    /* Print the acquired client ID */
    printf("MQTT Connect Ack: Assigned Client ID: %s",
           mqttCtx->client_id);

    definePayload(mqttCtx);

    /* Publish Topic */
    XMEMSET(&mqttCtx->publish, 0, sizeof(MqttPublish));
    mqttCtx->publish.qos = mqttCtx->qos;
    mqttCtx->publish.topic_name = mqttCtx->topic_name;
    mqttCtx->publish.packet_id = mqtt_get_packetid();
    mqttCtx->publish.buffer = (byte *) mqttCtx->message;
    mqttCtx->publish.total_len = (word16) XSTRLEN(mqttCtx->message);


    rc = MqttClient_Publish(&mqttCtx->client, &mqttCtx->publish);

    printf("MQTT Publish: Topic %s, %s (%d)",
           mqttCtx->publish.topic_name,
           MqttClient_ReturnCodeToString(rc), rc);
    if (rc != MQTT_CODE_SUCCESS) {
        printf(ERROR_PREFIX_TEST_FUNC "MqttClient_Publish Error");
        //goto disconn;
        goto exit;
    }


    disconn:
    /* Disconnect */
    rc = MqttClient_Disconnect_ex(&mqttCtx->client,
                                  &mqttCtx->disconnect);

    printf("MQTT Disconnect: %s (%d)",
           MqttClient_ReturnCodeToString(rc), rc);
    if (rc != MQTT_CODE_SUCCESS) {
        printf(ERROR_PREFIX_TEST_FUNC "NQTTClient_Disconnect_ex Error");
        //goto disconn;
    }

    rc = MqttClient_NetDisconnect(&mqttCtx->client);

    printf("MQTT Socket Disconnect: %s (%d)",
           MqttClient_ReturnCodeToString(rc), rc);

    exit:

    /* Free resources */
    if (mqttCtx->tx_buf) WOLFMQTT_FREE(mqttCtx->tx_buf);
    if (mqttCtx->rx_buf) WOLFMQTT_FREE(mqttCtx->rx_buf);

    /* Cleanup network */
    MqttClientNet_DeInit(&mqttCtx->net);

    MqttClient_DeInit(&mqttCtx->client);

    return rc;
}

int publishMqtt(){
    int rc;
    MQTTCtx mqttCtx;

    // init defaults
    rc = mqtt_init_ctx(&mqttCtx);
    if (rc != MQTT_CODE_SUCCESS) {
        printf("Main ERROR: mqtt_init_ctx error");
        goto exit;
    }

    mqttCtx.app_name = "mqttclient";

    rc = mqttclient_test(&mqttCtx);

    exit:
    mqtt_free_ctx(&mqttCtx);

    return (rc == 0) ? 0 : EXIT_FAILURE;
}

int main(int argc, char *argv[]) {
    /*int rc;
    MQTTCtx mqttCtx;

    // init defaults
    rc = mqtt_init_ctx(&mqttCtx);
    if (rc != MQTT_CODE_SUCCESS) {
        perror("Main ERROR: mqtt_init_ctx error");
        goto exit;
    }

    mqttCtx.app_name = "mqttclient";

    rc = mqttclient_test(&mqttCtx);

    exit:
    mqtt_free_ctx(&mqttCtx);

    return (rc == 0) ? 0 : EXIT_FAILURE;*/
    return publishMqtt();
}