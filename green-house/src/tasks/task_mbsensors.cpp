#include "green-house_tasks.h"
#include "sco2GMP252.h"
#include "srhtHMP60.h"

QueueHandle_t sensors;

void
vMbsensorsTask (void *pvParams)
{
    sensors = xQueueCreate(1, sizeof(SENSORS_DATA));
    SENSORS_DATA data;
    data.co2_val = 0;
    data.rhum_val = 0;
    data.temp_val = 0;
    sco2GMP252 sco2(0, 10);
    srhtHMP60 rht(0, 10);

    while (1)
    {
        //Poll sensors in here.
        //Read relative humidity.
        rht.read_rhum(data.rhum_val, false);
        //Read temperature.
        rht.read_temp(data.temp_val, false);
        //Read co2.
        sco2.read(data.co2_val, data.rhum_val, false);

        printf("Sensors data:\r\nco2: %.2f\r\nrel hum: %.2f\r\ntemp: %.2f\r\n", data.co2_val, data.rhum_val, data.temp_val);
        vTaskDelay(5000);
    }
}