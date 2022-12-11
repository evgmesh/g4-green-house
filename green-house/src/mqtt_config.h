/*
 * mqtt_config.h
 *
 *  Created on: 11 Dec 2022
 *      Author: evgenymeshcheryakov
 */

#ifndef MQTT_CONFIG_H_
#define MQTT_CONFIG_H_

/**
 * @brief The MQTT client identifier used in this example.  Each client identifier
 * must be unique so edit as required to ensure no two clients connecting to the
 * same broker use the same client identifier.
 *
 *!!! Please note a #defined constant is used for convenience of demonstration
 *!!! only.  Production devices can use something unique to the device that can
 *!!! be read by software, such as a production serial number, instead of a
 *!!! hard coded constant.
 *
 * #define democonfigCLIENT_IDENTIFIER				"insert here."
 */
#define CLIENT_IDENTIFIER				"ESP-MQTT-01"

/**
 * @brief The port to use for the demo.
 *
 * #define democonfigMQTT_BROKER_PORT					( insert here. )
 */
#define MQTT_BROKER_PORT					1883

/*
 * @brief MQTT broker end point to connect to.

 * #define democonfigMQTT_BROKER_ENDPOINT	"insert here."
 */
#define MQTT_BROKER_ENDPOINT	 "192.168.100.13"
#define WIFI_SSID	    		 "Kotiboksi_64EE"
#define WIFI_PASS       		 "24C622949E993"


#endif /* MQTT_CONFIG_H_ */
