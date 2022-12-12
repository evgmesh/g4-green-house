#include "green-house_tasks.h"

void vMQTTTask (void *pvParameters) {
	std::string topic("channels/1955513/publish");
	std::string message("field1=250&field2=34&field3=22&field4=1&field5=100&created_at=");
	mqtt mqtt;
	mqtt.publish(topic, message);

	while(true) {
		vTaskDelay(1000);
	}
}
