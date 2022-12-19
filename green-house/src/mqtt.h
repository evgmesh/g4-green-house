/*
 * mqtt.h
 *
 *  Created on: 11 Dec 2022
 *      Author: evgenymeshcheryakov
 */

#ifndef MQTT_H_
#define MQTT_H_

#include "mqtt_defines.h"
#include "mqtt_config.h"
#include "common_values.h"
//#include "mqtt_demo/logging_stack.h"
//#include "mqtt_demo/logging_levels.h"

class mqtt {
public:
	mqtt(char *, char *);
	virtual ~mqtt();
	void publish(std::string, std::string);
private:
	/* Members */
	NetworkContext_t  xNetworkContext = {0};
	PlaintextTransportParams_t xPlaintextTransportParams = {0};
	MQTTContext_t xMQTTContext;
	MQTTStatus_t xMQTTStatus;
	PlaintextTransportStatus_t xNetworkStatus;
	char _ssid[16];
	char _password[16];
	/* Functions */
	void connect();
	PlaintextTransportStatus_t prvConnectToServerWithBackoffRetries (NetworkContext_t *);
	void prvCreateMQTTConnectionWithBroker (MQTTContext_t *, NetworkContext_t *);
	void prvMQTTProcessIncomingPublish (MQTTPublishInfo_t *);
	PlaintextTransportStatus_t Plaintext_FreeRTOS_Connect( NetworkContext_t *,
	                                                       const char *,
	                                                       uint16_t, uint32_t, uint32_t);
	void set_ssid_and_password(char *, char *);
};

#endif /* MQTT_H_ */
