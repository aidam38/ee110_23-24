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

/*********************************************************************
* INCLUDES
*********************************************************************/
#include <stdint.h>
#include <bcomdef.h>
/*********************************************************************
* CONSTANTS
*********************************************************************/

#define  NO_CHANGE   0xFF     /* ID when no attributes are changing */

// Profile Parameters
// Service UUID
#define BAREBOTPROFILE_SERV_UUID 0xFFF0
// Characteristic defines
#define BAREBOTPROFILE_THOUGHTS   0
#define BAREBOTPROFILE_THOUGHTS_UUID 0xFFF1
#define BAREBOTPROFILE_THOUGHTS_LEN  32
// Characteristic defines
#define BAREBOTPROFILE_SPEED   1
#define BAREBOTPROFILE_SPEED_UUID 0xFFF2
#define BAREBOTPROFILE_SPEED_LEN  2
// Characteristic defines
#define BAREBOTPROFILE_TURN   2
#define BAREBOTPROFILE_TURN_UUID 0xFFF3
#define BAREBOTPROFILE_TURN_LEN  2
// Characteristic defines
#define BAREBOTPROFILE_SPEEDUPDATE   3
#define BAREBOTPROFILE_SPEEDUPDATE_UUID 0xFFF4
#define BAREBOTPROFILE_SPEEDUPDATE_LEN  2
// Characteristic defines
#define BAREBOTPROFILE_TURNUPDATE   4
#define BAREBOTPROFILE_TURNUPDATE_UUID 0xFFF5
#define BAREBOTPROFILE_TURNUPDATE_LEN  2


/*********************************************************************
 * TYPEDEFS
*********************************************************************/

/*********************************************************************
 * MACROS
*********************************************************************/

/*********************************************************************
 * Profile Callbacks
*********************************************************************/

// Callback when a characteristic value has changed
typedef void (*BarebotProfileChange_t)( uint8_t paramID);

typedef struct
{
  BarebotProfileChange_t        pfnSimpleProfileChange;  // Called when characteristic value changes
  BarebotProfileChange_t        pfnCfgChangeCb;
} BarebotProfileCBs_t;
/*********************************************************************
 * API FUNCTIONS
*********************************************************************/
/*
 * _AddService- Initializes the service by registering
 *          GATT attributes with the GATT server.
 *
 */
extern bStatus_t BarebotProfile_AddService( uint32 services);

/*
 * _RegisterAppCBs - Registers the application callback function.
 *                    Only call this function once.
 *
 *    appCallbacks - pointer to application callbacks.
 */
extern bStatus_t BarebotProfile_RegisterAppCBs( BarebotProfileCBs_t *appCallbacks );

/*
 * _SetParameter - Set a service parameter.
 *
 *    param - Profile parameter ID
 *    len - length of data to right
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 */
extern bStatus_t BarebotProfile_SetParameter(uint8 param, uint8 len, void *value);

/*
 * _GetParameter - Get a service parameter.
 *
 *    param - Profile parameter ID
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 */
extern bStatus_t BarebotProfile_GetParameter(uint8 param, void *value);

extern bStatus_t BarebotProfile_NotifyParameter(uint8 param);

/*****************************************************
Extern variables
*****************************************************/
extern BarebotProfileCBs_t *BarebotProfile_AppCBs;
extern uint8 BarebotProfileThoughts[BAREBOTPROFILE_THOUGHTS_LEN];
extern uint8 BarebotProfileSpeed[BAREBOTPROFILE_SPEED_LEN];
extern uint8 BarebotProfileTurn[BAREBOTPROFILE_TURN_LEN];
extern uint8 BarebotProfileSpeedUpdate;
extern uint8 BarebotProfileTurnUpdate;
extern gattCharCfg_t *BarebotProfileSpeedConfig;
extern gattCharCfg_t *BarebotProfileTurnConfig;
/*********************************************************************
*********************************************************************/

#endif
