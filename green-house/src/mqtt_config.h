/*
 * mqtt_config.h
 *
 *  Created on: 11 Dec 2022
 *      Author: evgenymeshcheryakov
 */

#ifndef MQTT_CONFIG_H_
#define MQTT_CONFIG_H_

#define SMARTIOTMQTT 0
#define DANIIL 0
#define GREENHOUSEDEMO 1


/**
 * @brief The port to use for the demo.
 */
#define MQTT_BROKER_PORT		  1883
#define WIFI_SSID	    		 "SmartIotMQTT"
#define WIFI_PASS       		 "SmartIot"
/*
 * @brief MQTT broker end point to connect to.
 */
#if SMARTIOTMQTT
#define CLIENT_IDENTIFIER			"GH-MQTT-G4"
#define MQTT_BROKER_ENDPOINT	 "192.168.1.136"
#define BROKER_USER_NAME    	 "group4"
#define BROKER_PASSWORD			 "12345678"

#elif DANIIL
#define CLIENT_IDENTIFIER			"GH-MQTT-G4"
#define MQTT_BROKER_ENDPOINT	 "192.168.43.1"
#define WIFI_SSID	    		 "test"
#define WIFI_PASS       		 "12345678"
#define BROKER_USER_NAME    "group4"
#define BROKER_PASSWORD		"12345678"

#elif GREENHOUSEDEMO
#define BROKER_USER_NAME    	 "DCAmDzgFFhoKKy8kCBw3NQA"
#define BROKER_PASSWORD			 "qKxqzEzD+xMf2LMg0SU24WYk"
#define CLIENT_IDENTIFIER		 "DCAmDzgFFhoKKy8kCBw3NQA"
#define MQTT_BROKER_ENDPOINT	 "mqtt3.thingspeak.com"


#endif

#endif /* MQTT_CONFIG_H_ */
