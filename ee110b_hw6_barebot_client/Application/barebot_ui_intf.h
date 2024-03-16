/****************************************************************************/
/*                                                                          */
/*                             barebot_ui_intf.h                            */
/*                            barebot ui Interface                          */
/*                                Include File                              */
/*                                                                          */
/****************************************************************************/

/*
   This file contains the constants, structures, and function prototypes for
   interfacing with the Bluetooth barebot ui defined in
   barebot_ui.c. 


   Revision History:
      3/15/24 Adam Krivka       initial revision
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

/* create the barebot ui task */
void  BarebotUI_createTask(void);

/* alert the UI taht the ui state changed */
void  BarebotUI_uiStateChanged(uint8 newState);
void  BarebotUI_speedChanged(int16 newSpeed);
void  BarebotUI_turnChanged(int16 newTurn);

#endif
