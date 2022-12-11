#include "Rotary.h"

Rotary::Rotary (QueueHandle_t *level_q) : level (150, 1024)
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
Rotary::isr ()
{
  portBASE_TYPE xHigherPriorityWoken = pdFALSE;
  uint8_t data;
  signal[SIGNAL_ROTATE_CLO].read () ? data = ROTARY_CLOCKWISE
                                    : data = ROTARY_CCLOCKWISE;
  BaseType_t isr_sent
      = xQueueSendFromISR (*_level_q, &data, &xHigherPriorityWoken);
  assert (isr_sent);
  Chip_PININT_ClearIntStatus (LPC_GPIO_PIN_INT, PININTCH (0));
  return xHigherPriorityWoken;
}
