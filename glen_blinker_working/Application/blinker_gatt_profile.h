/****************************************************************************/
/*                                                                          */
/*                           blinker_gatt_profile.h                         */
/*                          GATT Callback Functions                         */
/*                               Bluetooth Demo                             */
/*                                Include File                              */
/*                                                                          */
/****************************************************************************/

/*
   This file contains the constants, structures, and function prototypes for
   the Bluetooth blinker GATT profile defined in blinker_gatt_profile.c.


   Revision History:
       3/10/22  Glen George      initial revision
*/



#ifndef  __BLINKER_GATT_PROFILE_H__
    #define  __BLINKER_GATT_PROFILE_H__



/* library include files */
    /* none */

/* local include files */
    /* none */




/* constants */

#define  NO_CHANGE   0xFF     /* ID when no attributes are changing */




/* structures, unions, and typedefs */
    /* none */




/* function declarations */

/* callback when reading an attribute value */
bStatus_t  blinker_ReadAttrCB(uint16_t connHandle, gattAttribute_t *pAttr,
                              uint8_t *pValue, uint16_t *pLen, uint16_t offset,
                              uint16_t maxLen, uint8_t method);

/* callback when writing an attribute value */
bStatus_t  blinker_WriteAttrCB(uint16_t connHandle, gattAttribute_t *pAttr,
                               uint8_t *pValue, uint16_t len, uint16_t offset,
                               uint8_t method);


#endif
