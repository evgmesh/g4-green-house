/*
 * mqtt_defines.h
 *
 *  Created on: 11 Dec 2022
 *      Author: evgenymeshcheryakov
 */

#ifndef MQTT_DEFINES_H_
#define MQTT_DEFINES_H_

#include <string>
extern "C" {
/* Standard includes. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

/* MQTT library includes. */
#include "core_mqtt.h"

/* Exponential backoff retry include. */
#include "backoff_algorithm.h"

/* Transport interface include. */
#include "mqtt_demo/using_plaintext.h"

}


/**
 * @brief The topic to subscribe and publish to in the example.
 *
 * The topic name starts with the client identifier to ensure that each demo
 * interacts with a unique topic name.
 */


#define mqttTOPIC  			"channels/1955513/publish"

/**
 * @brief The maximum number of retries for network operation with server.
 */
#define mqttexampleRETRY_MAX_ATTEMPTS (5U)

/**
 * @brief The maximum back-off delay (in milliseconds) for retrying failed
 * operation with server.
 */
#define mqttexampleRETRY_MAX_BACKOFF_DELAY_MS (5000U)

/**
 * @brief The base back-off delay (in milliseconds) to use for network
 * operation retry attempts.
 */
#define mqttexampleRETRY_BACKOFF_BASE_MS (500U)

/**
 * @brief Timeout for receiving CONNACK packet in milliseconds.
 */
#define mqttexampleCONNACK_RECV_TIMEOUT_MS (1000U)


/**
 * @brief The number of topic filters to subscribe.
 */
#define mqttexampleTOPIC_COUNT (1)



/**
 * @brief Dimensions a file scope buffer currently used to send and receive
 * MQTT data from a socket.
 */
#define mqttexampleSHARED_BUFFER_SIZE (500U)


/**
 * @brief Timeout for MQTT_ProcessLoop in milliseconds.
 */
#define mqttexamplePROCESS_LOOP_TIMEOUT_MS (500U)

/**
 * @brief Keep alive time reported to the broker while establishing an MQTT
 * connection.
 *
 * It is the responsibility of the Client to ensure that the interval between
 * Control Packets being sent does not exceed the this Keep Alive value. In the
 * absence of sending any other Control Packets, the Client MUST send a
 * PINGREQ Packet.
 */
#define mqttexampleKEEP_ALIVE_TIMEOUT_SECONDS (60U)

/**
 * @brief Delay between MQTT publishes. Note that the process loop also has a
 * timeout, so the total time between publishes is the sum of the two delays.
 */
#define DELAY_BETWEEN_PUBLISHES (pdMS_TO_TICKS (5000U))

/**
 * @brief Transport timeout in milliseconds for transport send and receive.
 */
#define mqttexampleTRANSPORT_SEND_RECV_TIMEOUT_MS (200U)

#define MILLISECONDS_PER_SECOND (1000U) /**< @brief Milliseconds per second.  \
                                         */
#define MILLISECONDS_PER_TICK                                                 \
  (MILLISECONDS_PER_SECOND                                                    \
   / configTICK_RATE_HZ) /**< Milliseconds per FreeRTOS tick. */

/*-----------------------------------------------------------*/

struct NetworkContext
{
  PlaintextTransportParams_t *pParams;
};

/*-----------------------------------------------------------*/

/**
 * @brief Publishes a message mqttexampleMESSAGE on mqttexampleTOPIC topic.
 *
 * @param[in] pxMQTTContext MQTT context pointer.
 */
static void prvMQTTPublishToTopic (MQTTContext_t *pxMQTTContext, std::string, std::string);

/**
 * @brief The timer query function provided to the MQTT context.
 *
 * @return Time in milliseconds.
 */
static uint32_t prvGetTimeMs (void);

/**
 * @brief Process a response or ack to an MQTT request (PING, SUBSCRIBE
 * or UNSUBSCRIBE). This function processes PINGRESP, SUBACK, and UNSUBACK.
 *
 * @param[in] pxIncomingPacket is a pointer to structure containing
 * deserialized MQTT response.
 * @param[in] usPacketId is the packet identifier from the ack received.
 */
static void prvMQTTProcessResponse (MQTTPacketInfo_t *pxIncomingPacket,
                                    uint16_t usPacketId);

/**
 * @brief Process incoming Publish message.
 *
 * @param[in] pxPublishInfo is a pointer to structure containing deserialized
 * Publish message.
 */
static void prvMQTTProcessIncomingPublish (MQTTPublishInfo_t *pxPublishInfo);

/**
 * @brief The application callback function for getting the incoming publish
 * and incoming acks reported from the MQTT library.
 *
 * @param[in] pxMQTTContext MQTT context pointer.
 * @param[in] pxPacketInfo Packet Info pointer for the incoming packet.
 * @param[in] pxDeserializedInfo Deserialized information from the incoming
 * packet.
 */
static void prvEventCallback (MQTTContext_t *pxMQTTContext,
                              MQTTPacketInfo_t *pxPacketInfo,
                              MQTTDeserializedInfo_t *pxDeserializedInfo);

/*-----------------------------------------------------------*/

/**
 * @brief Static buffer used to hold MQTT messages being sent and received.
 */
static uint8_t ucSharedBuffer[mqttexampleSHARED_BUFFER_SIZE];

/**
 * @brief Global entry time into the application to use as a reference
 * timestamp in the #prvGetTimeMs function. #prvGetTimeMs will always return
 * the difference between the current time and the global entry time. This will
 * reduce the chances of overflow for the 32 bit unsigned integer used for
 * holding the timestamp.
 */
static uint32_t ulGlobalEntryTimeMs;

/**
 * @brief A pair containing a topic filter and its SUBACK status.
 */
typedef struct topicFilterContext
{
  const char *pcTopicFilter;
  MQTTSubAckStatus_t xSubAckStatus;
} topicFilterContext_t;

/**
 * @brief An array containing the context of a SUBACK; the SUBACK status
 * of a filter is updated when the event callback processes a SUBACK.
 */
static topicFilterContext_t xTopicFilterContext[mqttexampleTOPIC_COUNT]
    = { { mqttTOPIC, MQTTSubAckFailure } };

/**
 * @brief Static buffer used to hold MQTT messages being sent and received.
 */
static MQTTFixedBuffer_t xBuffer
    = { .pBuffer = ucSharedBuffer, .size = mqttexampleSHARED_BUFFER_SIZE };



/*-----------------------------------------------------------*/

#endif /* MQTT_DEFINES_H_ */
