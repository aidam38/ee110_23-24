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

/* RTOS include files */
#include  <ti/sysbios/knl/Task.h>
#include  <ti/sysbios/knl/Event.h>

/* BLE include files */
#include  <icall.h>
#include  "util.h"
#include  "icall_ble_api.h"      /* BLE API and icall structure definitions */
#include  "ti_ble_config.h"
#include  "ti_ble_gatt_service.h"

/* local include files */
#include <barebot_gatt_profile.h>

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

bStatus_t barebot_ReadAttrCB(uint16_t connHandle, gattAttribute_t *pAttr,
                             uint8_t *pValue, uint16_t *pLen, uint16_t offset,
                             uint16_t maxLen, uint8_t method)
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
            case BAREBOTPROFILE_TURN_UUID:
            case BAREBOTPROFILE_THOUGHTS_UUID:
            case BAREBOTPROFILE_ERROR_UUID:
                /* red and green LED state is a single byte */
                *pLen = 1;
                /* copy the byte from the attribute */
                pValue[0] = *pAttr->pValue;
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
bStatus_t barebot_WriteAttrCB(uint16_t connHandle, gattAttribute_t *pAttr,
                              uint8_t *pValue, uint16_t len, uint16_t offset,
                              uint8_t method)
{
    /* variables */
    uint16 uuid; /* UUID of attribute */

    uint8 changeID = NO_CHANGE; /* ID of changed attribute */

    bStatus_t status = SUCCESS; /* return status, initially good */

    /* not a blob operation, check if using 16-bit UUIDs */
    if (pAttr->type.len == ATT_BT_UUID_SIZE)
    {

        /* 16-bit UUID, form the UUID */
        uuid = BUILD_UINT16(pAttr->type.uuid[0], pAttr->type.uuid[1]);

        /* do the write operation based on UUID */
        switch (uuid)
        {

        case BAREBOTPROFILE_SPEED_UUID:
        case BAREBOTPROFILE_TURN_UUID:
        case BAREBOTPROFILE_THOUGHTS_UUID:
            /* changing red or green LED state */
            /* first make sure it isn't a blob operation */
            if (offset == 0)
            {

                /* not a blob operation - check the size */
                if (len == 1)
                {

                    /* valid write operation - write the data */
                    *((uint8*) (pAttr->pValue)) = pValue[0];

                    /* need to let peripheral know data changed */
                    switch (uuid)
                    {
                    case BAREBOTPROFILE_SPEED_UUID:
                        changeID = BAREBOTPROFILE_SPEED;
                        break;
                    case BAREBOTPROFILE_TURN_UUID:
                        changeID = BAREBOTPROFILE_TURN;
                        break;
                    case BAREBOTPROFILE_THOUGHTS_UUID:
                        changeID = BAREBOTPROFILE_THOUGHTS;
                        break;
                    }
                }
                else
                {

                    /* data is only 1 byte, can't write more */
                    status = ATT_ERR_INVALID_VALUE_SIZE;
                }
            }
            else
            {

                /* is a blob operation - reject since data is 1 byte */
                status = ATT_ERR_ATTR_NOT_LONG;
            }
            break;

        case GATT_CLIENT_CHAR_CFG_UUID:
            /* changing the client configuration */
            /* let the GATT library code handle it */
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
