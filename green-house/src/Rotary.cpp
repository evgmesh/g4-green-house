#include "Rotary.h"

extern "C"
{
  void
  PIN_INT0_IRQHandler (void)
  {
    portEND_SWITCHING_ISR (this->isr ());
  }
}

Rotary::Rotary ()
{
  /* Initialize PININT driver */
  Chip_PININT_Init (LPC_GPIO_PIN_INT);

  /* Enable PININT clock */
  Chip_Clock_EnablePeriphClock (SYSCTL_CLOCK_PININT);

  /* Reset the PININT block */
  Chip_SYSCTL_PeriphReset (RESET_PININT);

  /* Configure interrupt channel for the GPIO pin in INMUX block */
  Chip_INMUX_PinIntSel (0, 0, 6);

  /* Configure channel interrupt as edge sensitive and falling edge interrupt*/
  Chip_PININT_ClearIntStatus (LPC_GPIO_PIN_INT, PININTCH (0));
  Chip_PININT_SetPinModeEdge (LPC_GPIO_PIN_INT, PININTCH (0));
  Chip_PININT_EnableIntLow (LPC_GPIO_PIN_INT, PININTCH (0));
  Chip_PININT_DisableIntHigh (LPC_GPIO_PIN_INT, PININTCH (0));

  /* Enable interrupt in the NVIC */
  NVIC_ClearPendingIRQ (PIN_INT0_IRQn);
  NVIC_SetPriority (PIN_INT0_IRQn, configMAX_SYSCALL_INTERRUPT_PRIORITY + 1);
  NVIC_EnableIRQ (PIN_INT0_IRQn);
}

Rotary::~Rotary () {}

portBASE_TYPE
Rotary::isr ()
{
  portBASE_TYPE xHigherPriorityWoken = pdFALSE;
  int data = 0;
  sw_a3.read () ? data = -1 : data = 1;
  // Do something with data
  Chip_PININT_ClearIntStatus (LPC_GPIO_PIN_INT, PININTCH (0));
  return xHigherPriorityWoken;
}