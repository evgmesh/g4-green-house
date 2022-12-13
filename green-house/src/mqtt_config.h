/*
 * mqtt_config.h
 *
 *  Created on: 11 Dec 2022
 *      Author: evgenymeshcheryakov
 */

#ifndef MQTT_CONFIG_H_
#define MQTT_CONFIG_H_

#define SMARTIOTMQTT 1

#define CLIENT_IDENTIFIER			"GH-MQTT-G4"

/**
 * @brief The port to use for the demo.
 */
#define MQTT_BROKER_PORT		  1883

/*
 * @brief MQTT broker end point to connect to.
 */
#if SMARTIOTMQTT
#define MQTT_BROKER_ENDPOINT	 "192.168.1.136"
#define WIFI_SSID	    		 "SmartIotMQTT"
#define WIFI_PASS       		 "SmartIot"

#else
#define MQTT_BROKER_ENDPOINT	 "192.168.43.1"
#define WIFI_SSID	    		 "test"
#define WIFI_PASS       		 "12345678"

#endif

#endif /* MQTT_CONFIG_H_ */
