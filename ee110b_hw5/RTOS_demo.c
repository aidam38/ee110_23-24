/***************************************************************/
/*                                                             */
/*                        RTOS_demo.c                          */
/*                  RTOS demo (keypad + LCD)                   */
/*                                                             */
/***************************************************************/\

/* RTOS include files */
#include  <ti/sysbios/BIOS.h>



int ResetISR(void)
{


    /* start the RTOS (infinite loops)*/
    BIOS_start();

	return 0;
}
