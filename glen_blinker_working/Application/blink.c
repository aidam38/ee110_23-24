/****************************************************************************/
/*                                                                          */
/*                                   blink                                  */
/*                             LED Blinking Tasks                           */
/*                               Bluetooth Demo                             */
/*                                                                          */
/****************************************************************************/

/*
   This file contains the tasks for blinking the LEDs for the RTOS-based event
   handler demo.  This version busy waits on an event to happen.  The global
   functions included are:
      GreenLEDTaskCreate - create the green LED task
      RedLEDTaskCreate   - create the red LED task

   The local functions included are:
      GreenLEDTaskRun - the actual task code for blinking the green LED
      RedLEDTaskRun   - the actual task code for blinking the red LED

   The global variables included are:
      redLEDEvent - event handle for signal the red LED should blink


   Revision History:
       2/18/22  Glen George      initial revision
       3/9/22   Glen George      added alignment of task stacks
       3/10/22  Glen George      changed to updating the LED state via the
                                    blinker peripheral
*/



/* RTOS include files */
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Event.h>


/* local include files */
#include  "EHdemo.h"
#include  "blink.h"
#include  "blinker_peripheral_intf.h"




/* global variables */

Event_Handle  redLEDEvent;      /* the red LED event - posted when it is    */
                                /* time to blink the red LED, global        */
                                /* because the assembly timer event handler */
                                /* needs to access it                       */



/* shared variables */

static  Task_Struct  redLEDTask;        /* task for blinking the red LED */
static  Task_Struct  greenLEDTask;      /* task for blinking the green LED */

                                        /* stacks for both tasks */
#pragma DATA_ALIGN(redLEDTaskStack, 8)
static  uint8_t      redLEDTaskStack[LED_TASK_STACK_SIZE];

#pragma DATA_ALIGN(greenLEDTaskStack, 8)
static  uint8_t      greenLEDTaskStack[LED_TASK_STACK_SIZE];




/*
   GreenLEDTaskCreate()

   Description:      This function creates the task for blinking the green
                     LED.

   Operation:        The function creates the parameter structure and then
                     fills in the appropriate values for the green LED task.
                     It then creates the task using the shared task structure
                     variable greenLEDTask.

   Arguments:        None.
   Return Value:     None.
   Exceptions:       None.

   Inputs:           None.
   Outputs:          None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Revision History: 02/18/22  Glen George      initial revision
*/

void  GreenLEDTaskCreate(void)
{
    /* variables */
    Task_Params  taskParams;            /* task parameter structure */



    /* create the task parameter structure */
    Task_Params_init(&taskParams);

    /* fill in non-default values which set the priority and use a */
    /*    statically allocated stack */
    taskParams.stack = greenLEDTaskStack;
    taskParams.stackSize = LED_TASK_STACK_SIZE;
    taskParams.priority = GREEN_LED_TASK_PRIORITY;


    /* now actually construct the task using the statically allocated */
    /*    task structure */
    Task_construct(&greenLEDTask, GreenLEDTaskRun, &taskParams, NULL);


    /* done creating and installing the task, return */
    return;

}




/*
   RedLEDTaskCreate()

   Description:      This function creates the task for blinking the red LED.

   Operation:        The function creates the parameter structure and then
                     fills in the appropriate values for the red LED task.  It
                     then creates the task using the shared task structure
                     variable redLEDTask.

   Arguments:        None.
   Return Value:     None.
   Exceptions:       None.

   Inputs:           None.
   Outputs:          None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Revision History: 02/18/22  Glen George      initial revision
*/

void  RedLEDTaskCreate(void)
{
    /* variables */
    Task_Params  taskParams;            /* task parameter structure */



    /* create the task parameter structure */
    Task_Params_init(&taskParams);

    /* fill in non-default values which set the priority and use a */
    /*    statically allocated stack */
    taskParams.stack = redLEDTaskStack;
    taskParams.stackSize = LED_TASK_STACK_SIZE;
    taskParams.priority = RED_LED_TASK_PRIORITY;


    /* now actually construct the task using the statically allocated */
    /*    task structure */
    Task_construct(&redLEDTask, RedLEDTaskRun, &taskParams, NULL);


    /* done creating and installing the task, return */
    return;

}




/*
   GreenLEDTaskRun(UArg, UArg)

   Description:      This function runs the task that updates the green LED.

   Operation:        The function just loops forever counting how many times
                     it has looped.  Every LOOPS_PER_BLINK times it requests
                     that the blinker peripheral update the LED state.

   Arguments:        a1 (UArg) - first argument (unused).
                     a2 (UArg) - second argument (unused).
   Return Value:     None.
   Exceptions:       None.

   Inputs:           None.
   Outputs:          None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Revision History: 02/18/22  Glen George      initial revision
                     03/10/22  Glen George      changed to calling peripheral
                                                   update function instead of
                                                   toggling the LED
*/

static  void  GreenLEDTaskRun(UArg a1, UArg a2)
{
    /* variables */
    int  loopCount;             /* number of times have looped */



    /* task initialization */

    /* have not looped yet */
    loopCount = 0;



    /* main task loop, just loop counting how many times have looped */
    while (TRUE)  {

        /* update the loop counter */
        loopCount++;

        /* check if it is time to toggle the green LED */
        if (loopCount >= LOOPS_PER_BLINK)  {

            /* time to update the LED */
            BlinkerPeripheral_updateGreen();

            /* and reset the loop counter */
            loopCount = 0;
        }

        /* change to #if 1 to see the effects of yielding instead of pre-empting */
        #if 0
            /* yield the CPU so other tasks can run */
                Task_yield();
        #endif
    }


    /* should never get here since running an infinite loop */
    return;

}




/*
   RedLEDTaskRun(UArg, UArg)

   Description:      This function runs the task that updates the red LED.

   Operation:        The function creates the redLEDEvent which is used to
                     blink the LED.  It then loops forever on waiting for a
                     redLEDEvent to occur.  Each time a redLEDEvent occurs
                     the blinker peripheral is told to update the LED state.

   Arguments:        a1 (UArg) - first argument (unused).
                     a2 (UArg) - second argument (unused).
   Return Value:     None.
   Exceptions:       None.

   Inputs:           None.
   Outputs:          None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Revision History: 02/18/22  Glen George      initial revision
                     03/10/22  Glen George      changed to calling peripheral
                                                   update function instead of
                                                   toggling the LED
*/

static  void  RedLEDTaskRun(UArg a1, UArg a2)
{
    /* variables */
    UInt  events;       /* currently active events */



    /* task initialization */

    /* just create the event for the LED */
    redLEDEvent = Event_create(NULL, NULL);



    /* main task loop, just watch for LED events and toggle the LED when get one */
    while (TRUE)  {

        /* wait for an event to occur */
        events = Event_pend(redLEDEvent, Event_Id_NONE, ALL_EVENTS, EVENT_TIMEOUT);


        /* check if got an event (should have) */
        if (events != 0)  {

            /* there is only one redLED event so no need to check value */
            /* got a red LED event so tell the peripheral to update it */
            BlinkerPeripheral_updateRed();
        }
    }


    /* should never get here since running an infinite loop */
    return;

}
