/*
 * mqtt.cpp
 *
 *  Created on: 11 Dec 2022
 *      Author: evgenymeshcheryakov
 */

#include "mqtt.h"

mqtt::mqtt(char *ssid, char *password, char *brokerIP) {
	/* Set the pParams member of the network context with desired transport. */
  set_ssid_and_password(ssid, password);
  memcpy(_brokerIP, brokerIP, 16);
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
      LogInfo (("Creating an MQTT connection to %s.", MQTT_BROKER_ENDPOINT));
      prvCreateMQTTConnectionWithBroker (&xMQTTContext, &xNetworkContext);
}

/* Publish messages with QoS0, then send and process Keep Alive messages.
 */
void mqtt::publish(std::string mqtt_topic, std::string mqtt_message) {
	  LogInfo (("Publish to the MQTT topic %s.", mqttTOPIC));
	  prvMQTTPublishToTopic (&xMQTTContext, mqtt_topic.c_str(), mqtt_message.c_str());
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
  if ((pxPublishInfo->topicNameLength == strlen (mqttTOPIC))
      && (0
          == strncmp (mqttTOPIC, pxPublishInfo->pTopicName,
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
  xConnectInfo.pUserName = BROKER_USER_NAME;
  xConnectInfo.userNameLength = (uint16_t)strlen (BROKER_USER_NAME);
  xConnectInfo.pPassword = BROKER_PASSWORD;
  xConnectInfo.passwordLength = (uint16_t)strlen (BROKER_PASSWORD);

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

void
mqtt::set_ssid_and_password(char *ssid, char *password)
{
  //Force terminate strings in the end.
  ssid[16] = '\0';
  password[16] = '\0';
  int i = 1;
  int q = 0;
  while (i <= 16)
  {
    if (ssid[i] == '\"')
      ssid[i] = '\0';
      
    if (password[i] == '\"')
      password[i] = '\0';

    _ssid[q] = ssid[i];
    _password[q] = password[i];
    i++;
    q++;
  }
  _ssid[16] = '\0';
  _password[16] = '\0';
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
  xMQTTPublishInfo.pPayload = msg;
  xMQTTPublishInfo.payloadLength = len;

  /* Send PUBLISH packet. Packet ID is not used for a QoS0 publish. */
  xResult = MQTT_Publish (pxMQTTContext, &xMQTTPublishInfo, 0U);

  configASSERT (xResult == MQTTSuccess);
  /* getting rid of warning: warning: variable 'xResult' set but not used [-Wunused-but-set-variable] */
  (void) xResult;
}



PlaintextTransportStatus_t mqtt::Plaintext_FreeRTOS_Connect( NetworkContext_t * pNetworkContext,
                                                       const char * pHostName,
                                                       uint16_t port,
                                                       uint32_t receiveTimeoutMs,
                                                       uint32_t sendTimeoutMs)
{
    PlaintextTransportParams_t * pPlaintextTransportParams = NULL;
    PlaintextTransportStatus_t plaintextStatus = PLAINTEXT_TRANSPORT_SUCCESS;
    BaseType_t socketStatus = 0;

    if( ( pNetworkContext == NULL ) || ( pNetworkContext->pParams == NULL ) || ( pHostName == NULL ) )
    {
        LogError( ( "Invalid input parameter(s): Arguments cannot be NULL. pNetworkContext=%p, "
                    "pHostName=%p.",
                    pNetworkContext,
                    pHostName ) );
        plaintextStatus = PLAINTEXT_TRANSPORT_INVALID_PARAMETER;
    }
    else
    {
        pPlaintextTransportParams = pNetworkContext->pParams;
        /* Establish a TCP connection with the server. */
#if 0
        socketStatus = Sockets_Connect( &( pPlaintextTransportParams->tcpSocket ),
                                        pHostName,
                                        port,
                                        receiveTimeoutMs,
                                        sendTimeoutMs );
#else
        pPlaintextTransportParams->tcpSocket = esp_socket(_ssid, _password) ;
        socketStatus = esp_connect(pPlaintextTransportParams->tcpSocket, _brokerIP, port);
#endif

        /* A non zero status is an error. */
        if( socketStatus != 0 )
        {
            LogError( ( "Failed to connect to %s with error %d.",
                        pHostName,
                        (int)socketStatus ) );
            plaintextStatus = PLAINTEXT_TRANSPORT_CONNECT_FAILURE;
        }
    }

    return plaintextStatus;
}


int32_t Plaintext_FreeRTOS_recv( NetworkContext_t * pNetworkContext,
                                 void * pBuffer,
                                 size_t bytesToRecv )
{
    PlaintextTransportParams_t * pPlaintextTransportParams = NULL;
    int32_t socketStatus = 1;

    configASSERT( ( pNetworkContext != NULL ) && ( pNetworkContext->pParams != NULL ) );

    pPlaintextTransportParams = pNetworkContext->pParams;

    /* The TCP socket may have a receive block time.  If bytesToRecv is greater
     * than 1 then a frame is likely already part way through reception and
     * blocking to wait for the desired number of bytes to be available is the
     * most efficient thing to do.  If bytesToRecv is 1 then this may be a
     * speculative call to read to find the start of a new frame, in which case
     * blocking is not desirable as it could block an entire protocol agent
     * task for the duration of the read block time and therefore negatively
     * impact performance.  So if bytesToRecv is 1 then don't call recv unless
     * it is known that bytes are already available. */
#if 0
    if( bytesToRecv == 1 )
    {
        socketStatus = ( int32_t ) FreeRTOS_recvcount( pPlaintextTransportParams->tcpSocket );
    }

    if( socketStatus > 0 )
    {
        socketStatus = FreeRTOS_recv( pPlaintextTransportParams->tcpSocket,
                                      pBuffer,
                                      bytesToRecv,
                                      0 );
    }
#else
#if 0
    /* this is essentially polling which makes no sense because our read
     * has a character count based timeout. Timeout per byte is very short
     * and we don't have an agent task so a little wait does not impact performance.
     * Actually AT-based ESP seems to be a bit sluggish sometimes and
     * the mqtt stack expects longer reads to complete without timeout for
     * messages to be handled properly so a longer wait is justified.
     */
    if( bytesToRecv == 1 )
    {
       socketStatus = esp_peek( pPlaintextTransportParams->tcpSocket );
    }
#endif
    if( socketStatus > 0 )
    {
        socketStatus = esp_read( pPlaintextTransportParams->tcpSocket,
                                      pBuffer,
                                      bytesToRecv );
    }
#endif

    return socketStatus;
}

int32_t Plaintext_FreeRTOS_send( NetworkContext_t * pNetworkContext,
                                 const void * pBuffer,
                                 size_t bytesToSend )
{
    PlaintextTransportParams_t * pPlaintextTransportParams = NULL;
    int32_t socketStatus = 0;

    configASSERT( ( pNetworkContext != NULL ) && ( pNetworkContext->pParams != NULL ) );

    pPlaintextTransportParams = pNetworkContext->pParams;
#if 0
    socketStatus = FreeRTOS_send( pPlaintextTransportParams->tcpSocket,
                                  pBuffer,
                                  bytesToSend,
                                  0 );
#else
    socketStatus = esp_write(pPlaintextTransportParams->tcpSocket, pBuffer, bytesToSend);
#endif

    if( socketStatus == -pdFREERTOS_ERRNO_ENOSPC )
    {
        /* The TCP buffers could not accept any more bytes so zero bytes were sent.
         * This is not necessarily an error that should cause a disconnect
         * unless it persists. */
        socketStatus = 0;
    }

    #if ( configUSE_PREEMPTION == 0 )
        {
            /* FreeRTOS_send adds the packet to be sent to the IP task's queue for later processing.
             * The packet is sent later by the IP task. When FreeRTOS is used in collaborative
             * mode (i.e. configUSE_PREEMPTION is 0), call taskYIELD to give IP task a chance to run
             * so that the packet is actually sent before this function returns. */
            taskYIELD();
        }
    #endif

    return socketStatus;
}



mqtt::~mqtt() {
	// TODO Auto-generated destructor stub
}

