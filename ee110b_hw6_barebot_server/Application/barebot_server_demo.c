/****************************************************************************/
/*                                                                          */
/*                             barebot_server_demo                          */
/*                 Barebot Demo Server/Peripheral Component                 */
/*                                                                          */
/****************************************************************************/

/*
   Description:      TODO: Add description

   User Interface:   Pressing onboard buttons resets speed and turn.
   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Revision History:
       2/18/22  Glen George      initial revision
       3/10/22  Glen George      updated to include BLE stack
       3/15/24  Adam Krivka      change to barebot demo
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

/* interface includes */
#include "cc26x2r/cc26x2r_rtos_intf.h"
#include  "barebot_peripheral_intf.h"




/* global variables */

// use the default BLE user defined configuration
icall_userCfg_t  user0Cfg = BLE_USER_CFG;




int  main()

{
    /* variables */


    /* initialize the system */
    PeriphPowerInit();             /* turn on power to everything */
    GPIOClockInit();               /* turn on clocks to everything */
    GPTClockInit();

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
    BarebotPeripheral_createTask();  /* create task for barebot BLE peripheral */

    /* finally ready to start the RTOS and get everything running */
    BIOS_start();


    return  0;                  /* nothing else to do in main */

}
