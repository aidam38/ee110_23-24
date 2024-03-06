/***************************************************************/
/*                                                             */
/*                        RTOS_demo.c                          */
/*                  RTOS demo (keypad + LCD)                   */
/*                                                             */
/***************************************************************/\

/* RTOS include files */
#include  <ti/sysbios/BIOS.h>

/* local include files */
#include "app_intf.h"


int ResetISR(void)
{
    /* initialization */
    InitKeypad();
    InitLCD();

    /* create tasks */
    App_createTask();

    /* start the RTOS (infinite loops)*/
    BIOS_start();

	return 0;
}
