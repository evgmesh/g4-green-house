#include "green-house_tasks.h"
#include "sco2GMP252.h"
#include "srhtHMP60.h"

void
vMbsensorsTask (void *pvParams)
{
    //sensors_q = xQueueCreate(1, sizeof(GH_DATA));
    GH_DATA data;
    data.co2_val = 0;
    data.rhum_val = 0;
    data.temp_val = 0;
    sco2GMP252 sco2(0, 10);
    srhtHMP60 rht(0, 10);

    while (1)
    {
        //Read relative humidity.
        rht.read_rhum(data.rhum_val, false);
        vTaskDelay(1000);

        //Read temperature.
        rht.read_temp(data.temp_val, false);
        vTaskDelay(1000);

        //Read co2.
        #if ARDUINO_SIM
        //Use imprecise read.
        sco2.read_rapid(data.co2_val, false);
        #else
        //Use precise read.
        sco2.read(data.co2_val, data.rhum_val, false);
        #endif

        xQueueOverwrite(sensors_q, (void *)&data);

        printf("[SENSORS] Sensors data:\r\nco2: %.2f\r\nrel hum: %.2f\r\ntemp: %.2f\r\n", data.co2_val, data.rhum_val, data.temp_val);
        vTaskDelay(1000);
    }
}