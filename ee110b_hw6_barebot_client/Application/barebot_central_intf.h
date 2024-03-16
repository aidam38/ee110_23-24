/****************************************************************************/
/*                                                                          */
/*                         barebot_central_intf.h                           */
/*                        barebot Central Interface                         */
/*                               Bluetooth Demo                             */
/*                                Include File                              */
/*                                                                          */
/****************************************************************************/

/*
   This file contains the constants, structures, and function prototypes for
   interfacing with the Bluetooth barebot central defined in
   barebot_central.c.


   Revision History:
      3/10/22  Glen George       initial revision
*/



#ifndef  __BAREBOT_CENTRAL_INTF_H__
    #define  __BAREBOT_CENTRAL_INTF_H__



/* library include files */
    /* none */

/* local include files */
    /* none */




/* constants */

/* states */
#define BC_STATE_IDLE           0
#define BC_STATE_INITIALIZING   1
#define BC_STATE_SCANNING       2
#define BC_STATE_CONNECTING     3
#define BC_STATE_DISC_CHARS     4
#define BC_STATE_ERROR          5
#define BC_STATE_READY          10



/* structures, unions, and typedefs */

/* barebot central read operation response */
typedef struct
{
  uint16 len;    /* length of value */
  uint8 *pValue; /* len bytes of read data */
} bcReadRsp_t;




/* function declarations */

/* create the barebot central task */
void  BarebotCentral_createTask(void);

/* get current state of central */
uint8 BarebotCentral_getState(void);

/* read a characteristic */
bcReadRsp_t BarebotCentral_read(uint8_t charID);

/* write a characteristic */
bool BarebotCentral_write(uint8 charID, uint8 *newValue);

#endif
