/****************************************************************************/
/*                                                                          */
/*                            barebot_gatt_profile                          */
/*                          GATT Callback Functions                         */
/*                             Event Handler Demo                           */
/*                                                                          */
/****************************************************************************/

/*
 This file contains the profile callback functions for blinking the LEDs
 for the BLE-based event handler demo.  The functions read or update the
 profile data and then send an appropriate event to the peripheral code
 if the data has changed.
 barebot_ReadAttrCB  - callback function for reading an attribute
 barebot_WriteAttrCB - callback fucntion for writing an attribute


 Revision History:
 3/10/22  Glen George      initial revision
 */

/*********************************************************************
 * INCLUDES
 *********************************************************************/

/* BLE include files */
#include  <icall.h>
#include  "util.h"
#include  "icall_ble_api.h"      /* BLE API and icall structure definitions */
#include  "ti_ble_config.h"

/* local include files */
#include <barebot_gatt_profile.h>

/*********************************************************************
 * GLOBAL VARIABLES
 *********************************************************************/

// BarebotProfile Service UUID
CONST uint8 BarebotProfileServUUID[ATT_BT_UUID_SIZE] =
        { LO_UINT16(BAREBOTPROFILE_SERV_UUID), HI_UINT16(
                BAREBOTPROFILE_SERV_UUID) };

// Thoughts UUID
CONST uint8 BarebotProfileThoughtsUUID[ATT_BT_UUID_SIZE] = {
        LO_UINT16(BAREBOTPROFILE_THOUGHTS_UUID), HI_UINT16(
                BAREBOTPROFILE_THOUGHTS_UUID) };

// Speed UUID
CONST uint8 BarebotProfileSpeedUUID[ATT_BT_UUID_SIZE] = {
        LO_UINT16(BAREBOTPROFILE_SPEED_UUID), HI_UINT16(
                BAREBOTPROFILE_SPEED_UUID) };

// Turn UUID
CONST uint8 BarebotProfileTurnUUID[ATT_BT_UUID_SIZE] =
        { LO_UINT16(BAREBOTPROFILE_TURN_UUID), HI_UINT16(
                BAREBOTPROFILE_TURN_UUID) };

// SpeedUpdate UUID
CONST uint8 BarebotProfileSpeedUpdateUUID[ATT_BT_UUID_SIZE] = {
        LO_UINT16(BAREBOTPROFILE_SPEEDUPDATE_UUID), HI_UINT16(
                BAREBOTPROFILE_SPEEDUPDATE_UUID) };

// TurnUpdate UUID
CONST uint8 BarebotProfileTurnUpdateUUID[ATT_BT_UUID_SIZE] = {
        LO_UINT16(BAREBOTPROFILE_TURNUPDATE_UUID), HI_UINT16(
                BAREBOTPROFILE_TURNUPDATE_UUID) };

/*********************************************************************
 * LOCAL VARIABLES
 *********************************************************************/
BarebotProfileCBs_t *BarebotProfile_AppCBs = NULL;

/*********************************************************************
 * Profile Attributes - variables
 *********************************************************************/

// Service declaration
static CONST gattAttrType_t BarebotProfileService = { ATT_BT_UUID_SIZE,
                                                      BarebotProfileServUUID };

// Characteristic "Thoughts" Properties (for declaration)
static uint8 BarebotProfileThoughtsProps = GATT_PROP_READ | GATT_PROP_WRITE;
// Characteristic "Thoughts" Value variable
uint8 BarebotProfileThoughts[BAREBOTPROFILE_THOUGHTS_LEN] = { 0x0 };
// Characteristic "Thoughts" User Description
static uint8 BarebotProfileThoughtsUserDesp[] = "Barebot's Thoughts";

// Characteristic "Speed" Properties (for declaration)
static uint8 BarebotProfileSpeedProps = GATT_PROP_NOTIFY | GATT_PROP_READ
        | GATT_PROP_WRITE;
// Characteristic "Speed" Value variable
uint8 BarebotProfileSpeed[BAREBOTPROFILE_SPEED_LEN] = { 0x0 };
// Characteristic "Speed" User Description
static uint8 BarebotProfileSpeedUserDesp[] =
        "Barebot's Speed (forward/backward)";
// Characteristic "Speed" CCCD
gattCharCfg_t *BarebotProfileSpeedConfig;

// Characteristic "Turn" Properties (for declaration)
static uint8 BarebotProfileTurnProps = GATT_PROP_NOTIFY | GATT_PROP_READ
        | GATT_PROP_WRITE;
// Characteristic "Turn" Value variable
uint8 BarebotProfileTurn[BAREBOTPROFILE_TURN_LEN] = { 0x0 };
// Characteristic "Turn" User Description
static uint8 BarebotProfileTurnUserDesp[] = "Barebot's Turn (left/right)";
// Characteristic "Turn" CCCD
gattCharCfg_t *BarebotProfileTurnConfig;

// Characteristic "SpeedUpdate" Properties (for declaration)
static uint8 BarebotProfileSpeedUpdateProps = GATT_PROP_WRITE;
// Characteristic "SpeedUpdate" Value variable
uint8 BarebotProfileSpeedUpdate = 0x0;
// Characteristic "SpeedUpdate" User Description
static uint8 BarebotProfileSpeedUpdateUserDesp[] = "Update Barebot Speed";

// Characteristic "TurnUpdate" Properties (for declaration)
static uint8 BarebotProfileTurnUpdateProps = GATT_PROP_WRITE;
// Characteristic "TurnUpdate" Value variable
uint8 BarebotProfileTurnUpdate = 0x0;
// Characteristic "TurnUpdate" User Description
static uint8 BarebotProfileTurnUpdateUserDesp[] = "Update Barebot Turn";
/*********************************************************************
 * Profile Attributes - Table
 *********************************************************************/
static gattAttribute_t BarebotProfileAttrTbl[] = { {
        { ATT_BT_UUID_SIZE, primaryServiceUUID },
        GATT_PERMIT_READ,
        0, (uint8*) &BarebotProfileService },

                                                   // Thoughts Characteristic Declaration
        { { ATT_BT_UUID_SIZE, characterUUID },
        GATT_PERMIT_READ,
          0, &BarebotProfileThoughtsProps },

        // Thoughts Characteristic Value
        { { ATT_BT_UUID_SIZE, BarebotProfileThoughtsUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
          0, BarebotProfileThoughts },

        // Characteristic Thoughts User Description
        { { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
          0, BarebotProfileThoughtsUserDesp },

        // Speed Characteristic Declaration
        { { ATT_BT_UUID_SIZE, characterUUID },
        GATT_PERMIT_READ,
          0, &BarebotProfileSpeedProps },

        // Speed Characteristic Value
        { { ATT_BT_UUID_SIZE, BarebotProfileSpeedUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
          0, BarebotProfileSpeed },

        // Characteristic Speed User Description
        { { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
          0, BarebotProfileSpeedUserDesp },

        // Speed configuration
        { { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
          0, (uint8*) &BarebotProfileSpeedConfig },

        // Turn Characteristic Declaration
        { { ATT_BT_UUID_SIZE, characterUUID },
        GATT_PERMIT_READ,
          0, &BarebotProfileTurnProps },

        // Turn Characteristic Value
        { { ATT_BT_UUID_SIZE, BarebotProfileTurnUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
          0, BarebotProfileTurn },

        // Characteristic Turn User Description
        { { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
          0, BarebotProfileTurnUserDesp },

        // Turn configuration
        { { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
          0, (uint8*) &BarebotProfileTurnConfig },

        // SpeedUpdate Characteristic Declaration
        { { ATT_BT_UUID_SIZE, characterUUID },
        GATT_PERMIT_READ,
          0, &BarebotProfileSpeedUpdateProps },

        // SpeedUpdate Characteristic Value
        { { ATT_BT_UUID_SIZE, BarebotProfileSpeedUpdateUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
          0, &BarebotProfileSpeedUpdate },

        // Characteristic SpeedUpdate User Description
        { { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
          0, BarebotProfileSpeedUpdateUserDesp },

        // TurnUpdate Characteristic Declaration
        { { ATT_BT_UUID_SIZE, characterUUID },
        GATT_PERMIT_READ,
          0, &BarebotProfileTurnUpdateProps },

        // TurnUpdate Characteristic Value
        { { ATT_BT_UUID_SIZE, BarebotProfileTurnUpdateUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
          0, &BarebotProfileTurnUpdate },

        // Characteristic TurnUpdate User Description
        { { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
          0, BarebotProfileTurnUpdateUserDesp },

};

/*********************************************************************
 * LOCAL FUNCTIONS
 *********************************************************************/
static bStatus_t gatt_BarebotProfile_ReadAttrCB(uint16_t connHandle,
                                                gattAttribute_t *pAttr,
                                                uint8_t *pValue, uint16_t *pLen,
                                                uint16_t offset,
                                                uint16_t maxLen, uint8_t method);
static bStatus_t gatt_BarebotProfile_WriteAttrCB(uint16_t connHandle,
                                                 gattAttribute_t *pAttr,
                                                 uint8_t *pValue, uint16_t len,
                                                 uint16_t offset,
                                                 uint8_t method);

/*********************************************************************
 * PROFILE CALLBACKS
 *********************************************************************/
// BarebotProfile Service Callbacks
CONST gattServiceCBs_t BarebotProfileCBs = { gatt_BarebotProfile_ReadAttrCB, // Read callback function pointer
        gatt_BarebotProfile_WriteAttrCB, // Write callback function pointer
        NULL  // Authorization callback function pointer
        };

/* functions */

/*
 barebot_ReadAttrCB(uint16_t, gattAttribute_t *, uint8_t *, uint16_t *,
 uint16_t, uint16_t, uint8_t)

 Description:      This function is the callback function for reading an
 attribute for the barebot profile.  It just gets the
 value from the attribute table and returns it.

 Operation:        The function looks up the value by 16-bit UUID and then
 returns that value via the passed pointers.  If the UUID
 is not 16-bits, or it is a blob read, or the UUID is
 unvalid, no data is returned (returned length is 0) and
 an error code is returned by the function.

 Arguments:        connHandle (uint16_t)     - connection message was
 received on.
 pAttr (gattAttribute_t *) - pointer to attribute to read.
 pValue (uint8_t *)        - pointer to where to store read
 data.
 pLen (uint16_t *)         - length of data to be read.
 offset (uint16_t)         - offset of the first octet to
 be read.
 maxLen (uint16_t)         - maximum length of data to be
 read.
 method (uint8_t)          - type of read message.
 Return Value:     (bstatus_t) - success (SUCCESS) or failure (error code) of
 the read operation.
 Exceptions:       None.

 Inputs:           None.
 Outputs:          None.

 Error Handling:   If a blob operation is requested the error code
 ATT__ERR_ATTR_NOT_LONG is returned.  If a 128-bit UUID is
 used, ATT_ERR_INVALID_HANDLE is returned.  If the passed
 attribute does not exist ATT_ERR_ATTR_NOT_FOUND is
 returned.

 Algorithms:       None.
 Data Structures:  None.

 Revision History: 03/07/22  Glen George      initial revision
 */

bStatus_t gatt_BarebotProfile_ReadAttrCB(uint16_t connHandle,
                                         gattAttribute_t *pAttr,
                                         uint8_t *pValue, uint16_t *pLen,
                                         uint16_t offset, uint16_t maxLen,
                                         uint8_t method)
{
    /* variables */
    uint16 uuid; /* UUID of attribute */

    bStatus_t status = SUCCESS; /* return status, initially good */

    /* only do the read if it is not a blob operation */
    if (offset == 0)
    {

        /* make sure using 16-bit attributes */
        if (pAttr->type.len == ATT_BT_UUID_SIZE)
        {

            /* get the 16-bit UUID */
            uuid = BUILD_UINT16(pAttr->type.uuid[0], pAttr->type.uuid[1]);

            /* handle the attributes based on UUID */
            switch (uuid)
            {
            case BAREBOTPROFILE_SPEED_UUID:
                *pLen = BAREBOTPROFILE_SPEED_LEN;
                memcpy(pValue, pAttr->pValue, BAREBOTPROFILE_SPEED_LEN);
                break;
            case BAREBOTPROFILE_TURN_UUID:
                *pLen = BAREBOTPROFILE_TURN_LEN;
                memcpy(pValue, pAttr->pValue, BAREBOTPROFILE_TURN_LEN);
                break;
            case BAREBOTPROFILE_THOUGHTS_UUID:
                *pLen = BAREBOTPROFILE_THOUGHTS_LEN;
                memcpy(pValue, pAttr->pValue, BAREBOTPROFILE_THOUGHTS_LEN);
                break;
            default:
                /* should never get here */
                /* nothing to return and its an error */
                *pLen = 0;
                status = ATT_ERR_ATTR_NOT_FOUND;
                break;
            }
        }
        else
        {

            /* not using 16-bit UUID's - return nothing and its an error */
            *pLen = 0;
            status = ATT_ERR_INVALID_HANDLE;
        }
    }
    else
    {

        /* have an offset and thus a blob request - return nothing and an error */
        /* reject blob operations since no attributes in this profile are long */
        *pLen = 0;
        status = ATT_ERR_ATTR_NOT_LONG;
    }

    /* all done, return with error status */
    return status;
}

/*
 barebot_WriteAttrCB(uint16_t, gattAttribute_t *, uint8_t *, uint16_t,
 uint16_t, uint8_t)

 Description:      This function is the callback function for writing an
 attribute for the barebot profile.  It writes the passed
 value to the attribute and then informs the peripheral of
 the new value.

 Operation:        The function looks up the value by 16-bit UUID and then
 writes the passed value.  If the UUID is not 16-bits, or
 it is a blob read, or the UUID is unvalid, no data is
 written and an error code is returned by the function.  If
 a value is successfully changed, the peripheral is
 notified through a callback function.

 Arguments:        connHandle (uint16_t)     - connection message was
 received on.
 pAttr (gattAttribute_t *) - pointer to attribute to write.
 pValue (uint8_t *)        - pointer to value to write.
 len (uint16_t)            - length of data to write.
 offset (uint16_t)         - offset of the first octet to
 write.
 method (uint8_t)          - type of write message.
 Return Value:     (bstatus_t) - success (SUCCESS) or failure (error code) of
 the write operation.
 Exceptions:       None.

 Inputs:           None.
 Outputs:          None.

 Error Handling:   If a blob operation is requested the error code
 ATT__ERR_ATTR_NOT_LONG is returned.  If a 128-bit UUID is
 used, ATT_ERR_INVALID_HANDLE is returned.  If the passed
 attribute does not exist ATT_ERR_ATTR_NOT_FOUND is
 returned.

 Algorithms:       None.
 Data Structures:  None.

 Revision History: 03/09/22  Glen George      initial revision
 */
bStatus_t gatt_BarebotProfile_WriteAttrCB(uint16_t connHandle,
                                          gattAttribute_t *pAttr,
                                          uint8_t *pValue, uint16_t len,
                                          uint16_t offset, uint8_t method)
{
    /* variables */
    uint16 uuid; /* UUID of attribute */

    uint8 changeID = NO_CHANGE; /* ID of changed attribute */

    bStatus_t status = SUCCESS; /* return status, initially good */

    int16 current; /* current value of speed or turn */

    attHandleValueNoti_t noti;

    /* not a blob operation, check if using 16-bit UUIDs */
    if (pAttr->type.len == ATT_BT_UUID_SIZE)
    {

        /* 16-bit UUID, form the UUID */
        uuid = BUILD_UINT16(pAttr->type.uuid[0], pAttr->type.uuid[1]);

        /* do the write operation based on UUID */
        switch (uuid)
        {

        case BAREBOTPROFILE_SPEED_UUID:
            memcpy(pAttr->pValue, pValue, len);
            changeID = BAREBOTPROFILE_SPEED;
            break;
        case BAREBOTPROFILE_TURN_UUID:
            memcpy(pAttr->pValue, pValue, len);
            changeID = BAREBOTPROFILE_TURN;
            break;
        case BAREBOTPROFILE_SPEEDUPDATE_UUID:
            /* get current speed from GATT table */
            memcpy(&current, BarebotProfileSpeed, BAREBOTPROFILE_SPEED_LEN);

            /* update it with the increment */
            current += (int16_t)BUILD_UINT16(pValue[0], pValue[1]);

            /* write it back */
            memcpy(BarebotProfileSpeed, &current, len);

            changeID = BAREBOTPROFILE_SPEED;
            break;
        case BAREBOTPROFILE_TURNUPDATE_UUID:
            /* get current speed from GATT table */
            memcpy(&current, BarebotProfileTurn, BAREBOTPROFILE_TURN_LEN);

            /* update it with the increment */
            current += (int16_t)BUILD_UINT16(pValue[0], pValue[1]);

            /* write it back */
            memcpy(BarebotProfileTurn, &current, len);

            changeID = BAREBOTPROFILE_TURN;
            break;
        case BAREBOTPROFILE_THOUGHTS_UUID:
            memcpy(pAttr->pValue, pValue, len);
            changeID = BAREBOTPROFILE_THOUGHTS;
            break;
        case GATT_CLIENT_CHAR_CFG_UUID:
            /* changing the client configuration */
            /* let the GATT library code handle it */
            /* this was originally necessary for turning on notifications from the phone app
             * it is now not required because we turn on notifications automatically for all
             * connections, but leaving it here because it still might be useful in some cases
             */
            status = GATTServApp_ProcessCCCWriteReq(connHandle, pAttr, pValue,
                                                    len, offset,
                                                    GATT_CLIENT_CFG_NOTIFY);
            break;

        default:
            /* should never get here (no other attributes) */
            /* nothing to return and its an error */
            status = ATT_ERR_ATTR_NOT_FOUND;
            break;
        }
    }
    else
    {

        /* not using 16-bit UUID's - its an error */
        status = ATT_ERR_INVALID_HANDLE;
    }

    /* if a characteristic value changed then use the callback function to */
    /*    notify the peripheral of the change (only if there is a callback) */
    if ((changeID != NO_CHANGE) && (BarebotProfile_AppCBs != NULL)
            && (BarebotProfile_AppCBs->pfnSimpleProfileChange != NULL))

        /* have a callback and there was a change, let the peripheral know */
        BarebotProfile_AppCBs->pfnSimpleProfileChange(changeID);

    /* finally done, return with the error/success status */
    return status;
}

/*
 * AddService - Initializes the services by registering
 * GATT attributes with the GATT server.
 *
 */
bStatus_t BarebotProfile_AddService(uint32 services)
{
    uint8 status;

    // Allocate Client Characteristic Configuration table
    BarebotProfileSpeedConfig = (gattCharCfg_t*) ICall_malloc(
            sizeof(gattCharCfg_t) * MAX_NUM_BLE_CONNS);
    if (BarebotProfileSpeedConfig == NULL)
    {
        return ( bleMemAllocError);
    }
    // Initialize Client Characteristic Configuration attributes
    GATTServApp_InitCharCfg( LINKDB_CONNHANDLE_INVALID,
                            BarebotProfileSpeedConfig);

    // Allocate Client Characteristic Configuration table
    BarebotProfileTurnConfig = (gattCharCfg_t*) ICall_malloc(
            sizeof(gattCharCfg_t) * MAX_NUM_BLE_CONNS);
    if (BarebotProfileTurnConfig == NULL)
    {
        return ( bleMemAllocError);
    }
    // Initialize Client Characteristic Configuration attributes
    GATTServApp_InitCharCfg( LINKDB_CONNHANDLE_INVALID,
                            BarebotProfileTurnConfig);
    if (services)
    {
        // Register GATT attribute list and CBs with GATT Server App
        status = GATTServApp_RegisterService(
                BarebotProfileAttrTbl, GATT_NUM_ATTRS( BarebotProfileAttrTbl ),
                GATT_MAX_ENCRYPT_KEY_SIZE, &BarebotProfileCBs);
    }
    else
    {
        status = SUCCESS;
    }
    return (status);
}

/***************************************************************
 * RegisterAppCBs - Registers the application callback function.
 *                  Only call this function once.
 *
 * appCallbacks - pointer to application callbacks.
 ***************************************************************/

bStatus_t BarebotProfile_RegisterAppCBs(BarebotProfileCBs_t *appCallbacks)
{
    if (appCallbacks)
    {
        BarebotProfile_AppCBs = appCallbacks;
        return ( SUCCESS);
    }
    else
    {
        return ( bleAlreadyInRequestedMode);
    }
}

/*******************************************************************
 * SetParameter - Set a service parameter.
 *
 *    param - Profile parameter ID
 *    len - length of data to right
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 ********************************************************************/
bStatus_t BarebotProfile_SetParameter(uint8 param, uint8 len, void *value)
{
    bStatus_t ret = SUCCESS;
    switch (param)
    {

    case BAREBOTPROFILE_THOUGHTS:
        if (len == sizeof(uint8))
        {
            memcpy(BarebotProfileThoughts, value, len);
        }
        else
        {
            ret = bleInvalidRange;
        }
        break;

    case BAREBOTPROFILE_SPEED:
        memcpy(BarebotProfileSpeed, value, len);
        // Try to send notification.
        GATTServApp_ProcessCharCfg(BarebotProfileSpeedConfig,
                                   (uint8*) &BarebotProfileSpeed, FALSE,
                                   BarebotProfileAttrTbl,
                                   GATT_NUM_ATTRS(BarebotProfileAttrTbl),
                                   INVALID_TASK_ID,
                                   gatt_BarebotProfile_ReadAttrCB);
        break;

    case BAREBOTPROFILE_TURN:
        memcpy(BarebotProfileTurn, value, len);
        // Try to send notification.
        GATTServApp_ProcessCharCfg(BarebotProfileTurnConfig,
                                   (uint8*) &BarebotProfileTurn, FALSE,
                                   BarebotProfileAttrTbl,
                                   GATT_NUM_ATTRS(BarebotProfileAttrTbl),
                                   INVALID_TASK_ID,
                                   gatt_BarebotProfile_ReadAttrCB);
        break;

    case BAREBOTPROFILE_SPEEDUPDATE:
        if (len == sizeof(uint8))
        {
            memcpy(&BarebotProfileSpeedUpdate, value, len);
        }
        else
        {
            ret = bleInvalidRange;
        }
        break;

    case BAREBOTPROFILE_TURNUPDATE:
        if (len == sizeof(uint8))
        {
            memcpy(&BarebotProfileTurnUpdate, value, len);
        }
        else
        {
            ret = bleInvalidRange;
        }
        break;

    default:
        ret = INVALIDPARAMETER;
        break;
    }
    return ret;
}

/******************************************************************
 * GetParameter - Get a service parameter.
 *
 * param - Profile parameter ID
 * value - pointer to data to write.  This is dependent on
 *         the parameter ID and WILL be cast to the appropriate
 *         data type (example: data type of uint16 will be cast to
 *         uint16 pointer).
 ******************************************************************/

bStatus_t BarebotProfile_NotifyParameter(uint8 param)
{
    bStatus_t ret = SUCCESS;
    switch (param)
    {
    ;
case BAREBOTPROFILE_SPEED:
    // Try to send notification.
    GATTServApp_ProcessCharCfg(BarebotProfileSpeedConfig,
                               (uint8*) &BarebotProfileSpeed, FALSE,
                               BarebotProfileAttrTbl,
                               GATT_NUM_ATTRS(BarebotProfileAttrTbl),
                               INVALID_TASK_ID,
                               gatt_BarebotProfile_ReadAttrCB);
    break;

case BAREBOTPROFILE_TURN:
    // Try to send notification.
    GATTServApp_ProcessCharCfg(BarebotProfileTurnConfig,
                               (uint8*) &BarebotProfileTurn, FALSE,
                               BarebotProfileAttrTbl,
                               GATT_NUM_ATTRS(BarebotProfileAttrTbl),
                               INVALID_TASK_ID,
                               gatt_BarebotProfile_ReadAttrCB);
    break;

default:
    ret = INVALIDPARAMETER;
    break;
    }
    return ret;
}

/******************************************************************
 * GetParameter - Get a service parameter.
 *
 * param - Profile parameter ID
 * value - pointer to data to write.  This is dependent on
 *         the parameter ID and WILL be cast to the appropriate
 *         data type (example: data type of uint16 will be cast to
 *         uint16 pointer).
 ******************************************************************/
bStatus_t BarebotProfile_GetParameter(uint8 param, void *value)
{
    bStatus_t ret = SUCCESS;
    switch (param)
    {
    case BAREBOTPROFILE_THOUGHTS:
    {
        memcpy(value, BarebotProfileThoughts, BAREBOTPROFILE_THOUGHTS_LEN);
        break;
    }

    case BAREBOTPROFILE_SPEED:
    {
        memcpy(value, BarebotProfileSpeed, BAREBOTPROFILE_SPEED_LEN);
        break;
    }

    case BAREBOTPROFILE_TURN:
    {
        memcpy(value, BarebotProfileTurn, BAREBOTPROFILE_TURN_LEN);
        break;
    }

    case BAREBOTPROFILE_SPEEDUPDATE:
    {
        memcpy(value, &BarebotProfileSpeedUpdate,
        BAREBOTPROFILE_SPEEDUPDATE_LEN);
        break;
    }

    case BAREBOTPROFILE_TURNUPDATE:
    {
        memcpy(value, &BarebotProfileTurnUpdate, BAREBOTPROFILE_TURNUPDATE_LEN);
        break;
    }

    default:
    {
        ret = INVALIDPARAMETER;
        break;
    }
    }
    return ret;
}
