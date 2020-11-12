//
// Created by osboxes on 11/12/20.
//
#include "MQTTClient.h"
#include <time.h>

#define DEFAULT_MQTT_HOST "broker.mqttdashboard.com"
#define DEFAULT_MQTT_PORT 1883
#define DEFAULT_MQTT_QOS 1
#define DEFAULT_KEEP_ALIVE_SEC 60
#define DEFAULT_CMD_TIMEOUT_MS 30000
#define DEFAULT_CON_TIMEOUT_MS  5000
#define DEFAULT_TOPIC_NAME "huji_iot_class/2020_2021"
#define DEFAULT_CLIENT_ID "galDov"
#define DEFAULT_MESSAGE "hi there I'm gal and dovner"

#define MAX_BUFFER_SIZE 100
#define PRINT_BUFFER_SIZE 100

#define DEFAULT_PAYLOAD "{"
"“Student1ID\":\"208296822\""
"“Student1ID\":\"<student-1-id>\""
"“Student1Name\":\"<student-1-name>\","
"“Student1Name\":\"<student-1-name>\","
"“CurrentTimeUTC\":<seconds-since-epoch>}"

static const char* kDefTopicName = DEFAULT_TOPIC_NAME;
static const char* kDefClientId =  DEFAULT_CLIENT_ID;

static volatile word16 mPacketIdLast;

int mqtt_tls_cb(MqttClient* client)
{
    (void)client;
    return 0;
}

word16 mqtt_get_packetid(void)
{
    /* Check rollover */
    if (mPacketIdLast >= MAX_PACKET_ID) {
        mPacketIdLast = 0;
    }

    return ++mPacketIdLast;
}

void mqtt_init_ctx(MQTTCtx* mqttCtx)
{
    XMEMSET(mqttCtx, 0, sizeof(MQTTCtx));
    mqttCtx->host = DEFAULT_MQTT_HOST;
    mqttCtx->port = DEFAULT_MQTT_PORT;
    mqttCtx->qos = DEFAULT_MQTT_QOS;
    mqttCtx->clean_session = 1;
    mqttCtx->keep_alive_sec = DEFAULT_KEEP_ALIVE_SEC;
    mqttCtx->client_id = kDefClientId;
    mqttCtx->topic_name = kDefTopicName;
    mqttCtx->cmd_timeout_ms = DEFAULT_CMD_TIMEOUT_MS;
    mqttCtx->message = DEFAULT_PAYLOAD
}

static int mqtt_message_cb(MqttClient *client, MqttMessage *msg,
                           byte msg_new, byte msg_done)
{
    byte buf[PRINT_BUFFER_SIZE+1];
    word32 len;
    MQTTCtx* mqttCtx = (MQTTCtx*)client->ctx;

    (void)mqttCtx;

    if (msg_new) {
        /* Determine min size to dump */
        len = msg->topic_name_len;
        if (len > PRINT_BUFFER_SIZE) {
            len = PRINT_BUFFER_SIZE;
        }
        XMEMCPY(buf, msg->topic_name, len);
        buf[len] = '\0'; /* Make sure its null terminated */

        /* Print incoming message */
        PRINTF("MQTT Message: Topic %s, Qos %d, Len %u",
               buf, msg->qos, msg->total_len);

    }

    /* Print message payload */
    len = msg->buffer_len;
    if (len > PRINT_BUFFER_SIZE) {
        len = PRINT_BUFFER_SIZE;
    }
    XMEMCPY(buf, msg->buffer, len);
    buf[len] = '\0'; /* Make sure its null terminated */
    PRINTF("Payload (%d - %d): %s",
           msg->buffer_pos, msg->buffer_pos + len, buf);

    if (msg_done) {
        PRINTF("MQTT Message: Done");
    }

    /* Return negative to terminate publish processing */
    return MQTT_CODE_SUCCESS;
}

int mqttclient_test(MQTTCtx *mqttCtx)
{
    int rc = MQTT_CODE_SUCCESS, i;

    PRINTF("MQTT Client: QoS %d, Use TLS %d", mqttCtx->qos,
           mqttCtx->use_tls);

    /* Initialize Network */
    rc = MqttClientNet_Init(&mqttCtx->net, mqttCtx);
    PRINTF("MQTT Net Init: %s (%d)",
           MqttClient_ReturnCodeToString(rc), rc);
    if (rc != MQTT_CODE_SUCCESS) {
        goto exit;
    }

    /* setup tx/rx buffers */
    mqttCtx->tx_buf = (byte*)WOLFMQTT_MALLOC(MAX_BUFFER_SIZE);
    mqttCtx->rx_buf = (byte*)WOLFMQTT_MALLOC(MAX_BUFFER_SIZE);

    /* Initialize MqttClient structure */
    rc = MqttClient_Init(&mqttCtx->client, &mqttCtx->net,
                         mqtt_message_cb,
                         mqttCtx->tx_buf, MAX_BUFFER_SIZE,
                         mqttCtx->rx_buf, MAX_BUFFER_SIZE,
                         mqttCtx->cmd_timeout_ms);

    PRINTF("MQTT Init: %s (%d)",
           MqttClient_ReturnCodeToString(rc), rc);
    if (rc != MQTT_CODE_SUCCESS) {
        goto exit;
    }
    /* The client.ctx will be stored in the cert callback ctx during
       MqttSocket_Connect for use by mqtt_tls_verify_cb */
    mqttCtx->client.ctx = mqttCtx;


    /* Connect to broker */
    rc = MqttClient_NetConnect(&mqttCtx->client, mqttCtx->host,
                               mqttCtx->port,
                               DEFAULT_CON_TIMEOUT_MS, mqttCtx->use_tls, mqtt_tls_cb);

    PRINTF("MQTT Socket Connect: %s (%d)",
           MqttClient_ReturnCodeToString(rc), rc);
    if (rc != MQTT_CODE_SUCCESS) {
        goto exit;
    }

    /* Build connect packet */
    XMEMSET(&mqttCtx->connect, 0, sizeof(MqttConnect));
    mqttCtx->connect.keep_alive_sec = mqttCtx->keep_alive_sec;
    mqttCtx->connect.clean_session = mqttCtx->clean_session;
    mqttCtx->connect.client_id = mqttCtx->client_id;

    /* Last will and testament sent by broker to subscribers
        of topic when broker connection is lost */
    XMEMSET(&mqttCtx->lwt_msg, 0, sizeof(mqttCtx->lwt_msg));
    mqttCtx->connect.lwt_msg = &mqttCtx->lwt_msg;
    mqttCtx->connect.enable_lwt = mqttCtx->enable_lwt;
    if (mqttCtx->enable_lwt) {
        /* Send client id in LWT payload */
        mqttCtx->lwt_msg.qos = mqttCtx->qos;
        mqttCtx->lwt_msg.retain = 0;
        mqttCtx->lwt_msg.topic_name = "lwttopic";
        mqttCtx->lwt_msg.buffer = (byte*)mqttCtx->client_id;
        mqttCtx->lwt_msg.total_len = (word16)XSTRLEN(mqttCtx->client_id);

    }

    /* Send Connect and wait for Connect Ack */
    rc = MqttClient_Connect(&mqttCtx->client, &mqttCtx->connect);

    PRINTF("MQTT Connect: Proto (%s), %s (%d)",
           MqttClient_GetProtocolVersionString(&mqttCtx->client),
           MqttClient_ReturnCodeToString(rc), rc);
    if (rc != MQTT_CODE_SUCCESS) {
        goto disconn;
    }

    /* Validate Connect Ack info */
    PRINTF("MQTT Connect Ack: Return Code %u, Session Present %d",
           mqttCtx->connect.ack.return_code,
           (mqttCtx->connect.ack.flags &
            MQTT_CONNECT_ACK_FLAG_SESSION_PRESENT) ?
           1 : 0
    );

    /* Print the acquired client ID */
        PRINTF("MQTT Connect Ack: Assigned Client ID: %s",
                mqttCtx->client_id);

    /* Build list of topics */
    XMEMSET(&mqttCtx->subscribe, 0, sizeof(MqttSubscribe));

    i = 0;
    mqttCtx->topics[i].topic_filter = mqttCtx->topic_name;
    mqttCtx->topics[i].qos = mqttCtx->qos;


    /* Publish Topic */
    XMEMSET(&mqttCtx->publish, 0, sizeof(MqttPublish));
    mqttCtx->publish.retain = 0;
    mqttCtx->publish.qos = mqttCtx->qos;
    mqttCtx->publish.duplicate = 0;
    mqttCtx->publish.topic_name = mqttCtx->topic_name;
    mqttCtx->publish.packet_id = mqtt_get_packetid();
    mqttCtx->publish.buffer = (byte*)mqttCtx->message;
    mqttCtx->publish.total_len = (word16)XSTRLEN(mqttCtx->message);


    rc = MqttClient_Publish(&mqttCtx->client, &mqttCtx->publish);

    PRINTF("MQTT Publish: Topic %s, %s (%d)",
           mqttCtx->publish.topic_name,
           MqttClient_ReturnCodeToString(rc), rc);
    if (rc != MQTT_CODE_SUCCESS) {
        goto disconn;
    }


    /* Check for error */
    if (rc != MQTT_CODE_SUCCESS) {
        goto disconn;
    }

    disconn:
    /* Disconnect */
    rc = MqttClient_Disconnect_ex(&mqttCtx->client,
                                  &mqttCtx->disconnect);

    PRINTF("MQTT Disconnect: %s (%d)",
           MqttClient_ReturnCodeToString(rc), rc);
    if (rc != MQTT_CODE_SUCCESS) {
        goto disconn;
    }

    rc = MqttClient_NetDisconnect(&mqttCtx->client);

    PRINTF("MQTT Socket Disconnect: %s (%d)",
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


int main(int argc, char *argv[]) {
    MqttNet mqttNet;
    int rc;
    MQTTCtx mqttCtx;

    /* init defaults */
    mqtt_init_ctx(&mqttCtx);
    mqttCtx.app_name = "mqttclient";
    mqttCtx.message = DEFAULT_MESSAGE;

    rc = mqttclient_test(&mqttCtx);

    return (rc == 0) ? 0 : EXIT_FAILURE;
}