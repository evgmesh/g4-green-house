#include "Rotary.h"

Rotary::Rotary (QueueHandle_t *level_q)
{
  /* Initialize PININT driver */
  Chip_PININT_Init (LPC_GPIO_PIN_INT);

  /* Enable PININT clock */
  Chip_Clock_EnablePeriphClock (SYSCTL_CLOCK_PININT);

  /* Reset the PININT block */
  Chip_SYSCTL_PeriphReset (RESET_PININT);

  /* Configure interrupt channel for the GPIO pin in INMUX block */
  Chip_INMUX_PinIntSel (0, 0, 5);

  /* Configure channel interrupt as edge sensitive and falling edge interrupt*/
  Chip_PININT_ClearIntStatus (LPC_GPIO_PIN_INT, PININTCH (0));
  Chip_PININT_SetPinModeEdge (LPC_GPIO_PIN_INT, PININTCH (0));
  Chip_PININT_EnableIntLow (LPC_GPIO_PIN_INT, PININTCH (0));
  Chip_PININT_DisableIntHigh (LPC_GPIO_PIN_INT, PININTCH (0));

  /* Enable interrupt in the NVIC */
  NVIC_ClearPendingIRQ (PIN_INT0_IRQn);
  NVIC_SetPriority (PIN_INT0_IRQn, configMAX_SYSCALL_INTERRUPT_PRIORITY + 1);
  NVIC_EnableIRQ (PIN_INT0_IRQn);

  _level_q = level_q;
}

Rotary::~Rotary () {}

portBASE_TYPE
Rotary::rotate_isr ()
{
  portBASE_TYPE xHigherPriorityWoken = pdFALSE;
  uint8_t data;
  signal[ROTARY_ACTION::ROTARY_CLOCKWISE].read ()
      ? data = ROTARY_ACTION::ROTARY_CLOCKWISE
      : data = ROTARY_ACTION::ROTARY_CCLOCKWISE;
  sendActionAndAssertQueue (&data, &xHigherPriorityWoken);
  Chip_PININT_ClearIntStatus (LPC_GPIO_PIN_INT, PININTCH (0));
  return xHigherPriorityWoken;
}

portBASE_TYPE
Rotary::press_isr ()
{
  portBASE_TYPE xHigherPriorityWoken = pdFALSE;
  uint8_t data = ROTARY_ACTION::ROTARY_PRESS;
  sendActionAndAssertQueue (&data, &xHigherPriorityWoken);
  Chip_PININT_ClearIntStatus (LPC_GPIO_PIN_INT, PININTCH (1));
  return xHigherPriorityWoken;
}

inline void
Rotary::sendActionAndAssertQueue (uint8_t *data, BaseType_t *const pxHPW)
{
  assert (xQueueSendFromISR (*_level_q, data, pxHPW));
}
