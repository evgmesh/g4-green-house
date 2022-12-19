
#include "green-house_tasks.h"

void
vMQTTTask (void *pvParameters)
{
  GH_DATA *dataSet = static_cast<GH_DATA *> (pvParameters);
  ND network_data = { { 0 }, { 0 } };
  xQueueReceive (network_q, (void *)&network_data, portMAX_DELAY);

  mqtt mqtt(network_data.ssid, network_data.password);
  char buffer[BUFSIZE];
  while (true)
    {
      xSemaphoreTake (publish_signal, DELAY_BETWEEN_PUBLISHES);

      printFormat (buffer, BUFSIZE, mqttMESSAGE, dataSet->co2_val,
                   dataSet->rhum_val, dataSet->temp_val,
                   (int)dataSet->valve_open, dataSet->set_point);

      std::string message = buffer;
      mqtt.publish (mqttTOPIC, message);
      printf ("%s\n", message.c_str ());
    }
}

void
printFormat (char *buf, int size, const char *fmt, ...)
{
  va_list args;
  va_start (args, fmt);
  vsnprintf (buf, size, fmt, args);
  va_end (args);
}
