/****************************************************************************/
/*                                                                          */
/*                                blink_intf.h                              */
/*                        LED Blinking Tasks Interface                      */
/*                               Bluetooth Demo                             */
/*                                Include File                              */
/*                                                                          */
/****************************************************************************/

/*
   This file contains the constants, structures, and function prototypes for
   interfacing with the blinking tasks defined in blink.c.


   Revision History:
      3/10/22  Glen George       initial revision
*/



#ifndef  __BLINK_INTF_H__
    #define  __BLINK_INTF_H__



/* library include files */
    /* none */


/* local include files */
    /* none */




/* constants */
    /* none */




/* structures, unions, and typedefs */
    /* none */




/* function declarations */
void  GreenLEDTaskCreate(void);         /* create the green LED task */
void  RedLEDTaskCreate(void);           /* create the red LED task */



#endif
