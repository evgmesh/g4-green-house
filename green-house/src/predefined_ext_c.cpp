/*
 * predefined_ext_c.cpp
 *
 *  Created on: Dec 9, 2022
 *      Author: tylen
 */

#include "common_values.h"

/* The following is required if runtime statistics are to be collected
 * Copy the code to the source file where other you initialize hardware */
extern "C"
{

  void
  vConfigureTimerForRunTimeStats (void)
  {
    Chip_SCT_Init (LPC_SCTSMALL1);
    LPC_SCTSMALL1->CONFIG = SCT_CONFIG_32BIT_COUNTER;
    LPC_SCTSMALL1->CTRL_U = SCT_CTRL_PRE_L (255)
                            | SCT_CTRL_CLRCTR_L; // set prescaler to 256 (255 +
                                                 // 1), and start timer
  }
}
/* end runtime statictics collection */