
#include "green-house_tasks.h"
#define mqttTOPIC 		"channels/1955513/publish"
#define mqttMESSAGE 	"field1=%4.0f&field2=%4.1f&field3=%3.1f&field4=%d&field5=%u"
#define BUFSIZE			100

void printFormat(char *, int, const char *, ...);

void vMQTTTask(void *pvParameters)
{
	GH_DATA dataSet;
	mqtt mqtt;
	char buffer[BUFSIZE];
	while (true)
	{
		xQueuePeek(sensors_q, &dataSet, portMAX_DELAY);
		printFormat(buffer, BUFSIZE, mqttMESSAGE,
					dataSet.co2_val, dataSet.rhum_val,
					dataSet.temp_val, (int)dataSet.valve_open,
					dataSet.set_point);
		std::string message = buffer;
		mqtt.publish(mqttTOPIC, message);
		printf("%s\n", message.c_str());
	}
}

void printFormat(char *buf, int size, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, size, fmt, args);
	va_end(args);
}
