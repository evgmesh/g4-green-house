#include "Rotary.h"

Rotary::Rotary (QueueHandle_t *level_q)
{

  /* Initialize PININT driver */
  Chip_PININT_Init (LPC_GPIO_PIN_INT);
  Chip_Clock_EnablePeriphClock (SYSCTL_CLOCK_QEI);
  Chip_SYSCTL_PeriphReset (RESET_QEI0);
  Chip_SWM_MovablePortPinAssign (SWM_QEI0_PHA_I, 0, 5);
  Chip_SWM_MovablePortPinAssign (SWM_QEI0_PHB_I, 0, 6);
  LPC_QEI->CON |= 0x2;
  LPC_QEI->CONF = LPC_QEI->IES = 0x20;
  LPC_QEI->FILTERPHA = 200;
  LPC_QEI->FILTERPHB = 200;
  LPC_QEI->WINDOW = 1;

  Chip_INMUX_PinIntSel (1, 1, 8);
  Chip_PININT_SetPinModeEdge (LPC_GPIO_PIN_INT, PININTCH (1));
  Chip_PININT_EnableIntHigh (LPC_GPIO_PIN_INT, PININTCH (1));
  Chip_PININT_ClearIntStatus (LPC_GPIO_PIN_INT, PININTCH (1));

  NVIC_ClearPendingIRQ (PIN_INT1_IRQn);
  NVIC_ClearPendingIRQ (QEI_IRQn);

  NVIC_SetPriority (QEI_IRQn, configMAX_SYSCALL_INTERRUPT_PRIORITY + 1);
  NVIC_SetPriority (PIN_INT1_IRQn, configMAX_SYSCALL_INTERRUPT_PRIORITY + 1);

  NVIC_EnableIRQ (PIN_INT1_IRQn);
  NVIC_EnableIRQ (QEI_IRQn);

  _level_q = level_q;
  prev_qei_val = 0;
}

Rotary::~Rotary () {}

portBASE_TYPE
Rotary::rotate_isr ()
{
  portBASE_TYPE xHigherPriorityWoken = pdFALSE;
  LPC_QEI->CLR = 0x20;
  int32_t current_qei_val = (LPC_QEI->INXCNT) / 2;
  if (prev_qei_val != current_qei_val)
    {
      uint8_t data = (prev_qei_val < current_qei_val);
      prev_qei_val = current_qei_val;
      sendActionAndAssertQueue (&data, &xHigherPriorityWoken);
    }

  return xHigherPriorityWoken;
}

portBASE_TYPE
Rotary::press_isr ()
{
  Chip_PININT_ClearIntStatus (LPC_GPIO_PIN_INT, PININTCH (1));
  portBASE_TYPE xHigherPriorityWoken = pdFALSE;
  uint8_t data = ROTARY_ACTION::ROTARY_PRESS;
  sendActionAndAssertQueue (&data, &xHigherPriorityWoken);
  return xHigherPriorityWoken;
}

inline void
Rotary::sendActionAndAssertQueue (uint8_t *data, BaseType_t *const pxHPW)
{
  assert (xQueueSendFromISR (*_level_q, data, pxHPW));
}
