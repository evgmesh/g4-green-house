#include "green-house_tasks.h"
#include "sco2GMP252.h"
#include "srhtHMP60.h"

Rotary *rot;

static void
overwriteGlobalGHData (GH_DATA *global, GH_DATA *local)
{
  global->co2_val = local->co2_val;
  global->rhum_val = local->rhum_val;
  global->temp_val = local->temp_val;
  xQueueSend (gh_data_queue, (void *)global, 1000);
}

static void
checkForOverflow (GH_DATA *data)
{
  if (data->co2_val > (float)9999)
    {
      data->co2_val = (float)9999;
    }
}

extern "C"
{
  void
  QEI_IRQHandler (void)
  {
    portEND_SWITCHING_ISR (rot->rotate_isr ());
  }

  void
  PIN_INT1_IRQHandler (void)
  {
    portEND_SWITCHING_ISR (rot->press_isr ());
  }
}

void
vMbsensorsTask (void *pvParams)
{
  GH_DATA *gh_data_sensors = static_cast<GH_DATA *> (pvParams);
  Rotary rotor (&action_q);
  rot = &rotor;
  GH_DATA data_local;
  data_local.co2_val = 0;
  data_local.rhum_val = 0;
  data_local.temp_val = 0;
  sco2GMP252 sco2 (0, 10);
  srhtHMP60 rht (0, 10);

  while (1)
    {
      // Read relative humidity.
      rht.read_rhum (data_local.rhum_val, false);
      vTaskDelay (1000);

      // Read temperature.
      rht.read_temp (data_local.temp_val, false);
      vTaskDelay (1000);

// Read co2.
#if ARDUINO_SIM
      // Use imprecise read.
      sco2.read_rapid (data_local.co2_val, false);
#else
      // Use precise read.
      sco2.read (data_local.co2_val, data_local.rhum_val, false);
#endif
      checkForOverflow(&data_local);
      overwriteGlobalGHData (gh_data_sensors, &data_local);
      vTaskDelay (1000);
    }
}
