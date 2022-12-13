
#include "green-house_tasks.h"

void vMQTTTask (void *pvParameters) {
	GH_DATA buffer;
	char buf[256];
	int rc = 0;
	std::string topic("channels/1955513/publish");
	std::string message("field1=399&field2=399&field3=399&field4=0&field5=100&created_at=");
	mqtt mqtt;
	while(true) {
		xQueuePeek(sensors_q, &buffer, portMAX_DELAY);
		rc = snprintf(buf, 256, "field1=%f&field2=%f&field3=%f&field4=1&field5=90&created_at=",
			buffer.co2_val, buffer.rhum_val, buffer.temp_val);
		std::string mess = buf;
		printf("%s\n", mess.c_str());
		mqtt.publish(topic, mess);
	}
}
