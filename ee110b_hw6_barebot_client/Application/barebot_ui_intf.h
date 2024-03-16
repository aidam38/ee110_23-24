/****************************************************************************/
/*                                                                          */
/*                         blinker_central_intf.h                        */
/*                        Blinker Central Interface                      */
/*                               Bluetooth Demo                             */
/*                                Include File                              */
/*                                                                          */
/****************************************************************************/

/*
   This file contains the constants, structures, and function prototypes for
   interfacing with the Bluetooth blinker central defined in
   blinker_central.c.


   Revision History:
      3/10/22  Glen George       initial revision
*/



#ifndef  __BAREBOT_UI_INTF_H__
    #define  __BAREBOT_UI_INTF_H__



/* library include files */
    /* none */

/* local include files */
    /* none */




/* constants */
    /* none */





/* structures, unions, and typedefs */
    /* none */




/* function declarations */

/* create the blinker central task */
void  BarebotUI_createTask(void);

/* alert the UI taht the central state changed */
void  BarebotUI_centralStateChanged(uint8 newState);
void  BarebotUI_speedChanged(int16 newSpeed);
void  BarebotUI_turnChanged(int16 newTurn);

#endif
