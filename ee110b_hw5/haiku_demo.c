/****************************************************************************/
/*                                                                          */
/*                                haiku_demo.c                              */
/*                              Haiku Demo Main Loop                        */
/*                                                                          */
/****************************************************************************/

/* Haiku Demo Main Loop. Initializes the peripherals and starts the RTOS.

   Revision History:
       3/6/24  Adam Krivka      initial revision
*/

/* C includes */
#include <stdint.h>

/* RTOS includes */
#include <ti/sysbios/BIOS.h>

/* interface includes */
#include "haiku_app_intf.h"
#include "cc26x2r/cc26x2r_rtos_intf.h"


/* 
    main()
    
    Description:     This function initializes the peripherals and starts the RTOS.
                     It initializes the peripheral power domain, GPIO clock, GPT 
                     clock, and SSI clock. It then creates the main app task and 
                     starts the RTOS.
*/
int main(void)
{
    /* initialization */
    PeriphPowerInit();              // turn on peripheral power domain
    GPIOClockInit();                // turn on GPIO module clock gate
    GPTClockInit();                 // turn on GPT module clock gate
    SSIClockInit();                 // turn on Serial module clock gate

    /* create app task */
    App_createTask();

    /* start the RTOS */
    BIOS_start();

    /* we should never get here */
    return (0);
}
