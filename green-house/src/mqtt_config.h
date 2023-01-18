/*
 * mqtt_config.h
 *
 *  Created on: 11 Dec 2022
 *      Author: evgenymeshcheryakov
 */

#ifndef MQTT_CONFIG_H_
#define MQTT_CONFIG_H_

#define GREENHOUSEDEMO 1

#define MQTT_BROKER_PORT 1883
#define WIFI_SSID "Network SSID"
#define WIFI_PASS "Network Password"

#if GREENHOUSEDEMO
#define BROKER_USER_NAME "DCAmDzgFFhoKKy8kCBw3NQA"
#define BROKER_PASSWORD "qKxqzEzD+xMf2LMg0SU24WYk"
#define CLIENT_IDENTIFIER "DCAmDzgFFhoKKy8kCBw3NQA"
#define MQTT_BROKER_ENDPOINT "mqtt3.thingspeak.com"

#else
#define BROKER_USER_NAME "username"
#define BROKER_PASSWORD "password"
#define CLIENT_IDENTIFIER "clientID"
#define MQTT_BROKER_ENDPOINT "mqtt3.example.com"

#endif

#endif /* MQTT_CONFIG_H_ */
