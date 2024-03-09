/****************************************************************************/
/*                                                                          */
/*                         blinker_peripheral_intf.h                        */
/*                        Blinker Peripheral Interface                      */
/*                               Bluetooth Demo                             */
/*                                Include File                              */
/*                                                                          */
/****************************************************************************/

/*
   This file contains the constants, structures, and function prototypes for
   interfacing with the Bluetooth blinker peripheral defined in
   blinker_peripheral.c.


   Revision History:
      3/10/22  Glen George       initial revision
*/



#ifndef  __BLINKER_PERIPHERAL_INTF_H__
    #define  __BLINKER_PERIPHERAL_INTF_H__



/* library include files */
    /* none */

/* local include files */
    /* none */




/* constants */
    /* none */





/* structures, unions, and typedefs */
    /* none */




/* function declarations */

/* create the blinker peripheral task */
void  BlinkerPeripheral_createTask(void);

/* signal state of red or green LED should be updated */
void  BlinkerPeripheral_updateRed(void);
void  BlinkerPeripheral_updateGreen(void);


#endif
