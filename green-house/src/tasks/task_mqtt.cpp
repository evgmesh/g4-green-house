
#include "green-house_tasks.h"

void vMQTTTask (void *pvParameters) {
	std::string topic("channels/1955513/publish");
	std::string message("field1=299&field2=299&field3=299&field4=1&field5=90&created_at=");
	mqtt mqtt;
	while(true) {
		mqtt.publish(topic, message);
	}
}
