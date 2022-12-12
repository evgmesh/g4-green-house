/*
 * mqtt.cpp
 *
 *  Created on: 11 Dec 2022
 *      Author: evgenymeshcheryakov
 */

#include "mqtt.h"
#include "mqtt_demo/logging_stack.h"
#include "mqtt_demo/logging_levels.h"

mqtt::mqtt() {

		std::string topic("HotTopic");
		std::string message("Please, print something!");
	/* Set the pParams member of the network context with desired transport. */
	xNetworkContext.pParams = &xPlaintextTransportParams;
	ulGlobalEntryTimeMs = prvGetTimeMs ();
	connect();
}

/* Attempt to connect to the MQTT broker. If connection fails, retry
       * after a timeout. The timeout value will exponentially increase until
       * the maximum number of attempts are reached or the maximum timeout
       * value is reached. The function below returns a failure status if the
       * TCP connection cannot be established to the broker after the
       * configured number of attempts. */
void mqtt::connect() {
      xNetworkStatus = prvConnectToServerWithBackoffRetries (&xNetworkContext);
      configASSERT (xNetworkStatus == PLAINTEXT_TRANSPORT_SUCCESS);

      /* Sends an MQTT Connect packet over the already connected TCP socket,
       * and waits for a connection acknowledgment (CONNACK) packet. */
      LogInfo (("Creating an MQTT connection to %s.",
                democonfigMQTT_BROKER_ENDPOINT));
      prvCreateMQTTConnectionWithBroker (&xMQTTContext, &xNetworkContext);
}

    /* If server rejected the subscription request, attempt to resubscribe to
     * the topic. Attempts are made according to the exponential backoff
     * retry strategy declared in backoff_algorithm.h. */
void mqtt::subscribe() {
    prvMQTTSubscribeWithBackoffRetries (&xMQTTContext);
}

/* Publish messages with QoS0, then send and process Keep Alive messages.
 */
void mqtt::publish(std::string mqtt_topic, std::string mqtt_message) {
	const char * mqtt_mess  = new char[mqtt_message.length()+1];
	mqtt_mess = mqtt_message.c_str();
	uint32_t ulPublishCount = 0U, ulTopicCount = 0U;
	const uint32_t ulMaxPublishCount = 5UL;
	for (ulPublishCount = 0; ulPublishCount < ulMaxPublishCount;
			   ulPublishCount++)
			{
			  LogInfo (("Publish to the MQTT topic %s.", mqttexampleTOPIC));
			  prvMQTTPublishToTopic (&xMQTTContext, mqtt_topic, mqtt_message);
			  vTaskDelay (mqttexamplePROCESS_LOOP_TIMEOUT_MS);
			  /* Process the incoming publish echo. Since the application
			   * subscribed to the same topic, the broker will send the same
			   * publish message back to the application. */
			  LogInfo (("Attempt to receive publish message from broker."));
			  xMQTTStatus = MQTT_ProcessLoop (&xMQTTContext,
											  mqttexamplePROCESS_LOOP_TIMEOUT_MS);
			  configASSERT (xMQTTStatus == MQTTSuccess);

			  /* Leave the connection idle for some time. */
			  LogInfo (("Keeping Connection Idle...\r\n"));
			  vTaskDelay (mqttexampleDELAY_BETWEEN_PUBLISHES);
			}
	delete[] mqtt_mess;
}


/**
 * @brief Connect to MQTT broker with reconnection retries.
 *
 * If connection fails, retry is attempted after a timeout.
 * Timeout value will exponentially increase until maximum
 * timeout value is reached or the number of attempts are exhausted.
 *
 * @param[out] pxNetworkContext The parameter to return the created network
 * context.
 *
 * @return The status of the final connection attempt.
 */

uint32_t uxRand() {
	return rand();
}
PlaintextTransportStatus_t mqtt::prvConnectToServerWithBackoffRetries (NetworkContext_t *pxNetworkContext)
{
  PlaintextTransportStatus_t xNetworkStatus;
  BackoffAlgorithmStatus_t xBackoffAlgStatus = BackoffAlgorithmSuccess;
  BackoffAlgorithmContext_t xReconnectParams;
  uint16_t usNextRetryBackOff = 0U;

  /* Initialize reconnect attempts and interval.*/
  BackoffAlgorithm_InitializeParams (
      &xReconnectParams, mqttexampleRETRY_BACKOFF_BASE_MS,
      mqttexampleRETRY_MAX_BACKOFF_DELAY_MS, mqttexampleRETRY_MAX_ATTEMPTS);

  /* Attempt to connect to MQTT broker. If connection fails, retry after
   * a timeout. Timeout value will exponentially increase till maximum
   * attempts are reached.
   */
  do
    {
      /* Establish a TCP connection with the MQTT broker. This example connects
       * to the MQTT broker as specified in democonfigMQTT_BROKER_ENDPOINT and
       * democonfigMQTT_BROKER_PORT at the top of this file. */
      LogInfo (("Create a TCP connection to %s:%d.",
                MQTT_BROKER_ENDPOINT, MQTT_BROKER_PORT));
      xNetworkStatus = Plaintext_FreeRTOS_Connect (
          pxNetworkContext, MQTT_BROKER_ENDPOINT,
          MQTT_BROKER_PORT,
          mqttexampleTRANSPORT_SEND_RECV_TIMEOUT_MS,
          mqttexampleTRANSPORT_SEND_RECV_TIMEOUT_MS);

      if (xNetworkStatus != PLAINTEXT_TRANSPORT_SUCCESS)
        {
          /* Generate a random number and calculate backoff value (in
           * milliseconds) for the next connection retry. Note: It is
           * recommended to seed the random number generator with a
           * device-specific entropy source so that possibility of multiple
           * devices retrying failed network operations at similar intervals
           * can be avoided. */
          xBackoffAlgStatus = BackoffAlgorithm_GetNextBackoff (
              &xReconnectParams, uxRand (), &usNextRetryBackOff);

          if (xBackoffAlgStatus == BackoffAlgorithmRetriesExhausted)
            {
              LogError ((
                  "Connection to the broker failed, all attempts exhausted."));
            }
          else if (xBackoffAlgStatus == BackoffAlgorithmSuccess)
            {
              LogWarn (("Connection to the broker failed. "
                        "Retrying connection with backoff and jitter."));
              vTaskDelay (pdMS_TO_TICKS (usNextRetryBackOff));
            }
        }
    }
  while ((xNetworkStatus != PLAINTEXT_TRANSPORT_SUCCESS)
         && (xBackoffAlgStatus == BackoffAlgorithmSuccess));

  return xNetworkStatus;
}


static uint32_t prvGetTimeMs (void)
{
 TickType_t xTickCount = 0;
 uint32_t ulTimeMs = 0UL;

 /* Get the current tick count. */
 xTickCount = xTaskGetTickCount ();

 /* Convert the ticks to milliseconds. */
 ulTimeMs = (uint32_t)xTickCount * MILLISECONDS_PER_TICK;

 /* Reduce ulGlobalEntryTimeMs from obtained time so as to always return the
  * elapsed time in the application. */
 ulTimeMs = (uint32_t)(ulTimeMs - ulGlobalEntryTimeMs);

 return ulTimeMs;
}


static void prvMQTTProcessIncomingPublish (MQTTPublishInfo_t *pxPublishInfo)
{
  configASSERT (pxPublishInfo != NULL);

  /* Process incoming Publish. */
  LogInfo (("Incoming QoS : %d\n", pxPublishInfo->qos));

  /* Verify the received publish is for the we have subscribed to. */
  if ((pxPublishInfo->topicNameLength == strlen (mqttexampleTOPIC))
      && (0
          == strncmp (mqttexampleTOPIC, pxPublishInfo->pTopicName,
                      pxPublishInfo->topicNameLength)))
    {
      LogInfo (
          ("Incoming Publish Topic Name: %.*s matches subscribed topic.\r\n"
           "Incoming Publish Message : %.*s",
           pxPublishInfo->topicNameLength,
           (const char *)pxPublishInfo->pTopicName,
           pxPublishInfo->payloadLength,
           (const char *)pxPublishInfo->pPayload));
    }
  else
    {
      LogInfo ((
          "Incoming Publish Topic Name: %.*s does not match subscribed topic.",
          pxPublishInfo->topicNameLength, pxPublishInfo->pTopicName));
    }
}

static void
prvUpdateSubAckStatus (MQTTPacketInfo_t *pxPacketInfo)
{
  MQTTStatus_t xResult = MQTTSuccess;
  uint8_t *pucPayload = NULL;
  size_t ulSize = 0;
  uint32_t ulTopicCount = 0U;

  xResult = MQTT_GetSubAckStatusCodes (pxPacketInfo, &pucPayload, &ulSize);

  /* MQTT_GetSubAckStatusCodes always returns success if called with packet
   * info from the event callback and non-NULL parameters. */
  configASSERT (xResult == MQTTSuccess);

  for (ulTopicCount = 0; ulTopicCount < ulSize; ulTopicCount++)
    {
      xTopicFilterContext[ulTopicCount].xSubAckStatus
          = (MQTTSubAckStatus) pucPayload[ulTopicCount];
    }
  /* getting rid of warning: warning: variable 'xResult' set but not used [-Wunused-but-set-variable] */
  (void) xResult;
}

static void
prvMQTTProcessResponse (MQTTPacketInfo_t *pxIncomingPacket,
                        uint16_t usPacketId)
{
  uint32_t ulTopicCount = 0U;

  switch (pxIncomingPacket->type)
    {
    case MQTT_PACKET_TYPE_SUBACK:

      /* A SUBACK from the broker, containing the server response to our
       * subscription request, has been received. It contains the status code
       * indicating server approval/rejection for the subscription to the
       * single topic requested. The SUBACK will be parsed to obtain the status
       * code, and this status code will be stored in global variable
       * #xTopicFilterContext. */
      prvUpdateSubAckStatus (pxIncomingPacket);

      for (ulTopicCount = 0; ulTopicCount < mqttexampleTOPIC_COUNT;
           ulTopicCount++)
        {
          if (xTopicFilterContext[ulTopicCount].xSubAckStatus
              != MQTTSubAckFailure)
            {
              LogInfo (("Subscribed to the topic %s with maximum QoS %u.",
                        xTopicFilterContext[ulTopicCount].pcTopicFilter,
                        xTopicFilterContext[ulTopicCount].xSubAckStatus));
            }
        }

      /* Make sure ACK packet identifier matches with Request packet
       * identifier. */
      configASSERT (usSubscribePacketIdentifier == usPacketId);
      break;

    case MQTT_PACKET_TYPE_UNSUBACK:
      LogInfo (("Unsubscribed from the topic %s.", mqttexampleTOPIC));
      /* Make sure ACK packet identifier matches with Request packet
       * identifier. */
      configASSERT (usUnsubscribePacketIdentifier == usPacketId);
      break;

    case MQTT_PACKET_TYPE_PINGRESP:

      /* Nothing to be done from application as library handles
       * PINGRESP with the use of MQTT_ProcessLoop API function. */
      LogWarn (("PINGRESP should not be handled by the application "
                "callback when using MQTT_ProcessLoop.\n"));
      break;

    /* Any other packet type is invalid. */
    default:
      LogWarn (
          ("prvMQTTProcessResponse() called with unknown packet type:(%02X).",
           pxIncomingPacket->type));
    }
}



static void prvEventCallback (MQTTContext_t *pxMQTTContext, MQTTPacketInfo_t *pxPacketInfo,
                  MQTTDeserializedInfo_t *pxDeserializedInfo)
{
  /* The MQTT context is not used in this project. */
  (void)pxMQTTContext;

  if ((pxPacketInfo->type & 0xF0U) == MQTT_PACKET_TYPE_PUBLISH)
    {
      prvMQTTProcessIncomingPublish (pxDeserializedInfo->pPublishInfo);
    }
  else
    {
      prvMQTTProcessResponse (pxPacketInfo,
                              pxDeserializedInfo->packetIdentifier);
    }
}


/**
 * @brief Sends an MQTT Connect packet over the already connected TCP socket.
 *
 * @param[in, out] pxMQTTContext MQTT context pointer.
 * @param[in] pxNetworkContext Network context.
 *
 */

void mqtt::prvCreateMQTTConnectionWithBroker (MQTTContext_t *pxMQTTContext,
                                   NetworkContext_t *pxNetworkContext)
{
  MQTTStatus_t xResult;
  MQTTConnectInfo_t xConnectInfo;
  bool xSessionPresent;
  TransportInterface_t xTransport;

  /***
   * For readability, error handling in this function is restricted to the use
   *of asserts().
   ***/

  /* Fill in Transport Interface send and receive function pointers. */
  xTransport.pNetworkContext = pxNetworkContext;
  xTransport.send = Plaintext_FreeRTOS_send;
  xTransport.recv = Plaintext_FreeRTOS_recv;

  /* Initialize MQTT library. */
  xResult = MQTT_Init (pxMQTTContext, &xTransport, prvGetTimeMs,
                       prvEventCallback, &xBuffer);
  configASSERT (xResult == MQTTSuccess);

  /* Many fields not used in this demo so start with everything at 0. */
  (void)memset ((void *)&xConnectInfo, 0x00, sizeof (xConnectInfo));

  /* Start with a clean session i.e. direct the MQTT broker to discard any
   * previous session data. Also, establishing a connection with clean session
   * will ensure that the broker does not store any data when this client
   * gets disconnected. */
  xConnectInfo.cleanSession = true;

  /* The client identifier is used to uniquely identify this MQTT client to
   * the MQTT broker. In a production device the identifier can be something
   * unique, such as a device serial number. */
  xConnectInfo.pClientIdentifier = CLIENT_IDENTIFIER;
  xConnectInfo.clientIdentifierLength
      = (uint16_t)strlen (CLIENT_IDENTIFIER);

  /* Set MQTT keep-alive period. It is the responsibility of the application to
   * ensure that the interval between Control Packets being sent does not
   * exceed the Keep Alive value. In the absence of sending any other Control
   * Packets, the Client MUST send a PINGREQ Packet. */
  xConnectInfo.keepAliveSeconds = mqttexampleKEEP_ALIVE_TIMEOUT_SECONDS;

  /* Send MQTT CONNECT packet to broker. LWT is not used in this demo, so it
   * is passed as NULL. */
  xResult = MQTT_Connect (pxMQTTContext, &xConnectInfo, NULL,
                      mqttexampleCONNACK_RECV_TIMEOUT_MS, &xSessionPresent);
  configASSERT (xResult == MQTTSuccess);
  /* getting rid of warning: warning: variable 'xResult' set but not used [-Wunused-but-set-variable] */
  (void) xResult;
}

static void prvMQTTPublishToTopic (MQTTContext_t *pxMQTTContext, std::string topic, std::string message)
{
  MQTTStatus_t xResult;
  MQTTPublishInfo_t xMQTTPublishInfo;
  const char *msg = message.c_str();
  int len = strlen (msg);
  len = strlen (msg);

  /***
   * For readability, error handling in this function is restricted to the use
   *of asserts().
   ***/

  /* Some fields are not used by this demo so start with everything at 0. */
  (void)memset ((void *)&xMQTTPublishInfo, 0x00, sizeof (xMQTTPublishInfo));

  /* This demo uses QoS0. */
  xMQTTPublishInfo.qos = MQTTQoS0;
  xMQTTPublishInfo.retain = false;
  xMQTTPublishInfo.pTopicName = topic.c_str();
  xMQTTPublishInfo.topicNameLength = (uint16_t)strlen (topic.c_str());
#if 0
    xMQTTPublishInfo.pPayload = mqttexampleMESSAGE;
    xMQTTPublishInfo.payloadLength = strlen( mqttexampleMESSAGE );
#else
  xMQTTPublishInfo.pPayload = msg;
  xMQTTPublishInfo.payloadLength = len;
#endif

  /* Send PUBLISH packet. Packet ID is not used for a QoS0 publish. */
  xResult = MQTT_Publish (pxMQTTContext, &xMQTTPublishInfo, 0U);

  configASSERT (xResult == MQTTSuccess);
  /* getting rid of warning: warning: variable 'xResult' set but not used [-Wunused-but-set-variable] */
  (void) xResult;
}


mqtt::~mqtt() {
	// TODO Auto-generated destructor stub
}

