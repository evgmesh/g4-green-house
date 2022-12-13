
#include "green-house_tasks.h"

void vMQTTTask (void *pvParameters) {
	std::string topic("channels/1955513/publish");
	std::string message("field1=399&field2=399&field3=399&field4=0&field5=100&created_at=");
	mqtt mqtt;
	while(true) {
		mqtt.publish(topic, message);
	}
}
