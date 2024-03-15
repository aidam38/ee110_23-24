/****************************************************************************/
/*                                                                          */
/*                                   EHdemo                                 */
/*                             Event Handler Demo                           */
/*                                                                          */
/****************************************************************************/

/*
   Description:      This program is a demonstration program to show an
                     example of using the RTOS with timer interrupts. It
                     blinks the red and green LEDs.  The red LED is blinked
                     at the rate of MS_PER_BLINK (milliseconds per blink)
                     using a timer interrupt.  The green LED is blinked at
                     the rate of LOOPS_PER_BLINK in the main loop.

   Input:            None.
   Output:           The LEDs are blinked at the rate of MS_PER_BLINK for the
                     red LED and LOOPS_PER_BLINK for the green LED.  The red
                     LED starts off on and the green LED starts off off.

   User Interface:   None, LEDs are just blinked.
   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Revision History:
       2/18/22  Glen George      initial revision
       3/10/22  Glen George      updated to include BLE stack
*/


/* RTOS include files */
#include  <ti/sysbios/BIOS.h>
#include  <ti/sysbios/hal/Hwi.h>
#include  <icall.h>
#include  <ti/sysbios/knl/Clock.h>

/* header files required to enable instruction fetch cache */
#include  <inc/hw_memmap.h>
#include  <driverlib/vims.h>

/* BLE include files */
#include  "ble_user_config.h"

/* local include files */
#include  "EHdemo.h"
#include  "blink_intf.h"
#include  "blinker_peripheral_intf.h"




/* global variables */

// use the default BLE user defined configuration
icall_userCfg_t  user0Cfg = BLE_USER_CFG;




int  main()
{
    /* variables */
    static  Hwi_Struct  GPT0A_Task;     /* GPT0A timer task */

    Hwi_Params          GPT0A_Params;   /* parameters for GPT0A timer task */



    /* initialize the system */
    InitPower();                /* turn on power to everything */
    InitClocks();               /* turn on clocks to everything */
    InitGPIO();                 /* setup the I/O (only output) */
    InitGPT0();                 /* initialize the internal timer */

    InitLEDs();                 /* initialize the LED state */


    // Enable iCache prefetching
    VIMSConfigure(VIMS_BASE, TRUE, TRUE);
    // Enable cache
    VIMSModeSet(VIMS_BASE, VIMS_MODE_ENABLED);


    /* Update User Configuration of the stack */
    user0Cfg.appServiceInfo->timerTickPeriod = Clock_tickPeriod;
    user0Cfg.appServiceInfo->timerMaxMillisecond  = ICall_getMaxMSecs();


    /* Initialize ICall module */
    ICall_init();

    /* Start tasks of external images - Priority 5 */
    ICall_createRemoteTasks();


    /* create tasks */
    //RedLEDTaskCreate();                 /* create the task for the red LED */
    GreenLEDTaskCreate();               /* create the task for the green LED */
    BlinkerPeripheral_createTask();  /* create task for blinker BLE peripheral */


    /* setup Hardware Interrupt Task for timer */

    /* setup the parameters */
    Hwi_Params_init(&GPT0A_Params);
    GPT0A_Params.eventId = GPT0A_EX_NUM;
    GPT0A_Params.priority = GPT0A_PRIORITY;

    /* now create the task */
    //Hwi_construct(&GPT0A_Task, GPT0A_EX_NUM, GPT0AEventHandler, &GPT0A_Params, NULL);


    /* finally ready to start the RTOS and get everything running */
    BIOS_start();


    return  0;                  /* nothing else to do in main */

}
