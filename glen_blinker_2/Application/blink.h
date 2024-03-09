/****************************************************************************/
/*                                                                          */
/*                                  blink.h                                 */
/*                             LED Blinking Tasks                           */
/*                             Event Handler Demo                           */
/*                                Include File                              */
/*                                                                          */
/****************************************************************************/

/*
   This file contains the constants, structures, and function prototypes for
   the blinking tasks for the event handler with RTOS demonstration program.


   Revision History:
      2/18/22  Glen George       initial revision
      3/7/22   Glen George       changed task priorities and green LED loop
                                    count
      3/10/22  Glen George       split into blink.h and blink_intf.h
*/



#ifndef  __BLINK_H__
    #define  __BLINK_H__



/* library include files */
#include  <ti/sysbios/BIOS.h>


/* local include files */
    /* none */




/* constants */

#define  LED_TASK_STACK_SIZE            512     /* size of stack for LED tasks */

#define  RED_LED_TASK_PRIORITY          2       /* priority of red LED task */
#define  GREEN_LED_TASK_PRIORITY        1       /* run green LED task at low priority */

/* mask for all event flags */
#define  ALL_EVENTS                     0xFFFFFFFF

/* timeout when waiting for an event */
#define  EVENT_TIMEOUT                  BIOS_WAIT_FOREVER

/* number of loops for toggling green LED */
#define  LOOPS_PER_BLINK                300000




/* structures, unions, and typedefs */
    /* none */




/* function declarations */

/* local functions - task loops */
static  void  GreenLEDTaskRun(UArg, UArg);  /* task to update the green LED */
static  void  RedLEDTaskRun(UArg, UArg);    /* task to update the red LED */


#endif
