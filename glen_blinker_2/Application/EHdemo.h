/****************************************************************************/
/*                                                                          */
/*                                  EHdemo.h                                */
/*                             Event Handler Demo                           */
/*                                Include File                              */
/*                                                                          */
/****************************************************************************/

/*
   This file contains the constants and function prototypes for the event
   handler with RTOS demonstration program.


   Revision History:
      2/18/22  Glen George       initial revision
      3/7/22   Glen George       added function prototypes (from util.h).
      3/10/22  Glen George       removed <Tab>s
*/



#ifndef  __EHDEMO_H__
    #define  __EHDEMO_H__



/* library include files */
    /* none */

/* local include files */
    /* none */




/* constants */

/* GPT0A timer constants */
#define  GPT0A_PRIORITY     3           /* priority for the interrupt */
#define  GPT0A_EX_NUM       31          /* GPT0A is exception number 31 */




/* structures, unions, and typedefs */
    /* none */




/* function declarations */

void  GPT0AEventHandler();      /* event handler for GPTOA interrupt */

void  InitClocks();             /* turn on the clock to the peripherals */
void  InitGPIO();               /* initialize the I/O pins */
void  InitGPT0();               /* initialize the timers (just GPT0A) */
void  InitLEDs();               /* initialize the red and green LEDs */
void  InitPower();              /* turn on power to the peripherals */

void  SetGreenLED(int);         /* turn the green LED on or off*/
void  SetRedLED(int);           /* turn the red LED on or off */
void  ToggleGreenLED();         /* toggle the green LED */
void  ToggleRedLED();           /* toggle the red LED */


#endif
