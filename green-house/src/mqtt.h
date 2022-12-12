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

class mqtt {
public:
	mqtt();
	virtual ~mqtt();
	void publish(std::string, std::string);
private:
	/* Members */
	NetworkContext_t  xNetworkContext = {0};
	PlaintextTransportParams_t xPlaintextTransportParams = {0};
	MQTTContext_t xMQTTContext;
	MQTTStatus_t xMQTTStatus;
	PlaintextTransportStatus_t xNetworkStatus;

	/* Functions */
	void connect();
	PlaintextTransportStatus_t prvConnectToServerWithBackoffRetries (NetworkContext_t *);
	void prvCreateMQTTConnectionWithBroker (MQTTContext_t *, NetworkContext_t *);
	void prvMQTTProcessIncomingPublish (MQTTPublishInfo_t *);
};

#endif /* MQTT_H_ */
