/****************************************************************************/
/*                                                                          */
/*                             blinker_peripheral                           */
/*                             Blinker Peripheral                           */
/*                               Bluetooth Demo                             */
/*                                                                          */
/****************************************************************************/

/*
   This file contains the tasks and support code for implementing the blinker
   peripheral device for the Bluetooth demo.  The global functions included
   are:
      BlinkerPeripheral_createTask  - create the blinker peripheral task
      BlinkerPeripheral_updateRed   - want to update the red LED
      BlinkerPeripheral_updateGreen - want to update the green LED

   The local functions included are:
      BlinkerPeripheral_advCallback       - GAP advertising callback
      BlinkerPeripheral_charValueChangeCB - GATT value change callback
      BlinkerPeripheral_enqueueMsg        - enqueue a message for the task
      BlinkerPeripheral_init              - initialize blinker peripheral task
      BlinkerPeripheral_processAdvEvent   - process advertising events
      BlinkerPeripheral_processAppMsg     - process messages from the task
      BlinkerPeripheral_processCharValueChangeEvt - handle value changes
      BlinkerPeripheral_processGapMessage - process GAP messages
      BlinkerPeripheral_processStackMsg   - process BLE stack messages
      BlinkerPeripheral_spin              - infinite loop (for debugging)
      BlinkerPeripheral_taskFxn           - run the blinker peripheral task


   Revision History:
       3/10/22  Glen George      initial revision
*/



/* RTOS include files */
#include  <ti/sysbios/knl/Task.h>
#include  <ti/sysbios/knl/Clock.h>
#include  <ti/sysbios/knl/Event.h>
#include  <ti/sysbios/knl/Queue.h>

/* BLE include files */
#include  <icall.h>
#include  "util.h"
#include  <bcomdef.h>
#include  <icall_ble_api.h>
#include  <devinfoservice.h>
#include  "ti_ble_config.h"
#include  "ti_ble_gatt_service.h"

/* C library include files */
#include  <string.h>

/* local include files */
#include  <blinker_peripheral.h>
#include  "EHdemo.h"


/* shared variables */

/* task structure and task stack */
static  Task_Struct  bpTask;

#pragma DATA_ALIGN(bpTaskStack, 8)
static  uint8_t  bpTaskStack[BP_TASK_STACK_SIZE];

/* handle for current connection */
static   uint16_t  curr_conn_handle = LINKDB_CONNHANDLE_INVALID;

/* entity ID used to check for source and/or destination of messages */
static  ICall_EntityID  selfEntity;

/* event used to post local events and pend on system and local events */
static  ICall_SyncHandle  syncEvent;

/* queue object used for app messages */
static  Queue_Struct  appMsgQueue;
static  Queue_Handle  appMsgQueueHandle;

/* advertising handles */
static  uint8  advHandleLegacy;         /* handle for legacy advertising */
static  uint8  advHandleLongRange;      /* handle for BLE long range advertising */

/* Simple GATT Profile Callbacks */
static    BlinkerProfileCBs_t    BlinkerPeripheral_ProfileCBs = {
    BlinkerPeripheral_charValueChangeCB,     /* GATT Characteristic value change callback */
    NULL                                     /* GATT Configureation change callback */
};




/*
   BlinkerPeripheral_createTask()

   Description:      This function creates the task for running the blinker
                     peripheral.

   Operation:        The function creates the parameter structure and then
                     fills in the appropriate values for the blinker peripheral
                     task.  It then creates the task using the shared task
                     structure variable bpTask.

   Arguments:        None.
   Return Value:     None.
   Exceptions:       None.

   Inputs:           None.
   Outputs:          None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Revision History: 03/10/22  Glen George      initial revision
*/

void  BlinkerPeripheral_createTask(void)
{
    /* variables */
    Task_Params  taskParams;            /* parameters for setting up task */



    /* configure task */
    Task_Params_init(&taskParams);
    taskParams.stack     = bpTaskStack;
    taskParams.stackSize = BP_TASK_STACK_SIZE;
    taskParams.priority  = BP_TASK_PRIORITY;

    /* and create the task */
    Task_construct(&bpTask, BlinkerPeripheral_taskFxn, &taskParams, NULL);


    /* done creating the task, return */
    return;
}




/*
   BlinkerPeripheral_init()

   Description:      This function initializes the blinker peripheral task.  It
                     is expected to be called when the task starts.

   Operation:        The function registers the task with ICall and creates a
                     queue for the task.  It then configures the GAP, GATT,
                     and GAP/GATT modules.  Following this it registers the
                     callbacks and message reception.  Finally it sets up the
                     link layer and turns on the GAP layer.

   Arguments:        None.
   Return Value:     None.
   Exceptions:       None.

   Inputs:           None.
   Outputs:          None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Revision History: 03/10/22  Glen George      initial revision
*/

static  void  BlinkerPeripheral_init(void)
{
    /* variables */
      /* none */



    /* register the current thread as an ICall dispatcher application */
    /* so that the application can send and receive messages */
    ICall_registerApp(&selfEntity, &syncEvent);

    /* create an RTOS queue for message from profile to be sent to app */
    appMsgQueueHandle = Util_constructQueue(&appMsgQueue);


    /* set the Device Name characteristic in the GAP GATT Service */
    GGS_SetParameter(GGS_DEVICE_NAME_ATT, GAP_DEVICE_NAME_LEN, attDeviceName);

    /* configure GAP - let link take care of all parameter updates */
    GAP_SetParamValue(GAP_PARAM_LINK_UPDATE_DECISION, DEFAULT_PARAM_UPDATE_REQ_DECISION);

    /* initialize GATT attributes */
    GGS_AddService(GAP_SERVICE);                    /* GAP GATT Service */
    GATTServApp_AddService(GATT_ALL_SERVICES);      /* GATT Service */
    DevInfo_AddService();                           /* Device Information Service */
    BlinkerProfile_AddService(GATT_ALL_SERVICES);   /* Blinker GATT Profile */


    /* register callback with Blinker GATT profile */
    BlinkerProfile_RegisterAppCBs(&BlinkerPeripheral_ProfileCBs);

    /* register with GAP for HCI/Host messages - needed to receive HCI events */
    GAP_RegisterForMsgs(selfEntity);

    /* register for GATT local events and ATT Responses pending for transmission */
    GATT_RegisterForMsgs(selfEntity);


    /* set default values for Data Length Extension (enabled by default) */
    HCI_LE_WriteSuggestedDefaultDataLenCmd(BP_SUGGESTED_PDU_SIZE, BP_SUGGESTED_TX_TIME);


    /* initialize GAP layer for Peripheral role and register to receive GAP events */
    GAP_DeviceInit(GAP_PROFILE_PERIPHERAL, selfEntity, DEFAULT_ADDRESS_MODE, &pRandomAddress);


    /* done initializing the blinker peripheral task, return */
    return;
}




/*
   BlinkerPeripheral_taskFxn(UArg, UArg)

   Description:      This function runs the task that implements the Bluetooth
                     blinker peripheral device.

   Operation:        The function loops forever processing messages and events
                     from the application and Bluetooth stack.  The messages
                     and events are processed using helper functions.

   Arguments:        a1 (UArg) - first argument (unused).
                     a2 (UArg) - second argument (unused).
   Return Value:     None.
   Exceptions:       None.

   Inputs:           None.
   Outputs:          None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Revision History: 03/10/22  Glen George      initial revision
*/

static  void  BlinkerPeripheral_taskFxn(UArg a0, UArg a1)
{
    /* variables */
    uint32_t            events;         /* task event flags */

    ICall_ServiceEnum   src;            /* source and destination for BLE */
    ICall_EntityID      dest;           /*    stack messages */
    ICall_HciExtEvt    *pMsg = NULL;    /* HCI event message pointer */

    bpEvt_t            *pEvtMsg;        /* internal task event/message */

    uint8_t             safeToDealloc = TRUE;  /* whether or not to deallocate message */



    /* initialize the task */
    BlinkerPeripheral_init();


    /* main task loop just loops forever */
    for (;;)
    {

        /* wait for event to be posted associated with the calling thread */
        /*    this includes events that are posted when a message is queued */
        /*    to the message receive queue of the thread */
        events = Event_pend(syncEvent, Event_Id_NONE, BP_ALL_EVENTS, ICALL_TIMEOUT_FOREVER);

        /* if there is an event, process it */
        if (events)  {

            /* have an event, get any available messages from the stack */
            if (ICall_fetchServiceMsg(&src, &dest, (void **)&pMsg) == ICALL_ERRNO_SUCCESS)  {

                /* check if it is a BLE message for this task */
                if ((src == ICALL_SERVICE_CLASS_BLE) && (dest == selfEntity))  {

                    /* check for BLE stack events first */
                    if (((ICall_Stack_Event *) pMsg)->signature != 0xffff)
                        /* have an inter-task message, process it */
                        safeToDealloc = BlinkerPeripheral_processStackMsg((ICall_Hdr *)pMsg);
                }

                /* if there is a message and it can be deallocated, do so */
                if ((pMsg != NULL) && safeToDealloc)
                    ICall_freeMsg(pMsg);
            }

            /* next check if got an RTOS queue event */
            if (events & UTIL_QUEUE_EVENT_ID)  {

                /* process all RTOS queue entries until queue is empty */
                while (!Queue_empty(appMsgQueueHandle))  {

                    /* dequeue the message */
                    pEvtMsg = (bpEvt_t *)Util_dequeueMsg(appMsgQueueHandle);

                    /* check if got a message */
                    if (pEvtMsg != NULL)  {

                        /* got a message so process the message */
                        BlinkerPeripheral_processAppMsg(pEvtMsg);
                        /* now can free the memory allocated to the message */
                        ICall_free(pEvtMsg);
                    }
                }
            }
        }
    }


    /* done with the task (should never get here) */
    return;
}




/*
   BlinkerPeripheral_processStackMsg(ICall_Hdr *)

   Description:      This function processes messages from the BLE stack.

   Operation:        The message is processed based on the type of message.
                     GAP and GATT messages have their own processing functions.
                     HCI messages are ignored unless it is an error, in which
                     case an infinite loop is entered.  All messages can be
                     deallocated after this function processes them so TRUE is
                     always returned.

   Arguments:        pMsg (ICall_Hdr *) - pointer to the message to process.
   Return Value:     (uint8_t) - TRUE if the passed message should be
                        deallocated on return an FALSE if not (always TRUE).
   Exceptions:       None.

   Inputs:           None.
   Outputs:          None.

   Error Handling:   HCI errors cause the function to enter an infinite loop
                     for debugging purposes.  Unknown message types are
                     silently ignored.

   Algorithms:       None.
   Data Structures:  None.

   Revision History: 03/10/22  Glen George      initial revision
*/

static  uint8_t  BlinkerPeripheral_processStackMsg(ICall_Hdr *pMsg)
{
    /* variables */
      /* none */



    /* message processing is based on the type of message */
    switch (pMsg->event)  {

        case GAP_MSG_EVENT:
                /* process GAP message */
                BlinkerPeripheral_processGapMessage((gapEventHdr_t*) pMsg);
                break;

        case GATT_MSG_EVENT:
                /* process GATT message - nothing to process */
                /* just free message payload (only needed for ATT Protocol messages) */
                GATT_bm_free(&((gattMsgEvent_t *) pMsg)->msg, ((gattMsgEvent_t *) pMsg)->method);
                break;

        case HCI_GAP_EVENT_EVENT:
                /* if got an error, spin, otherwise ignore the message */
                if (pMsg->status == HCI_BLE_HARDWARE_ERROR_EVENT_CODE)
                        BlinkerPeripheral_spin();
                break;

        default:
                /* unknown event type - do nothing */
                break;
    }


    /* done processing the BLE stack message, return */
    return  TRUE;
}




/*
   BlinkerPeripheral_processAppMsg(bpEvt_t *)

   Description:      This function processes the task messages received by this
                     task (in other words, messages sent by the task itself).
                     These messages are generally due to callback functions
                     being called by the BLE stack.

   Operation:        The function processes the messages based on the type of
                     event that generated the message.  Unknown event types are
                     ignored.

   Arguments:        pMsg (bpEvt_t *) - pointer to the task message to process.
   Return Value:     None.
   Exceptions:       None.

   Inputs:           None.
   Outputs:          None.

   Error Handling:   Unknown event types are silently ignored.

   Algorithms:       None.
   Data Structures:  None.

   Revision History: 03/10/22  Glen George      initial revision
*/

static  void  BlinkerPeripheral_processAppMsg(bpEvt_t *pMsg)
{
    /* variables */
    bool  dealloc;      /* whether should deallocate message data */
    //attHandleValueNoti_t *notif;
    attHandleValueNoti_t notif;


    /* figure out what to do based on the message/event type */
    switch (pMsg->event)  {

        case BP_CHAR_CHANGE_EVT:
                /* a characteristic value changed, handle it */
                dealloc = BlinkerPeripheral_processCharValueChangeEvt(pMsg->data);
                break;

        case BP_ADV_EVT:
                /* there was an advertising event */
                dealloc = BlinkerPeripheral_processAdvEvent(pMsg->data);
                break;
        case BP_GREEN_TOGGLE:
                /* send notification */
                //notif = ICall_malloc(sizeof(attHandleValueNoti_t));
                notif.handle = 37;
                notif.len = BLINKERPROFILE_GREENTOGGLE_LEN;
                notif.pValue = &BlinkerProfileGreenToggle;
                bStatus_t error = GATT_Notification(curr_conn_handle, &notif, false);
                break;
        default:
                /* unknown application event - do nothing, but deallocate message */
                dealloc = TRUE;
                break;
    }

    /* free message data if it exists and we are supposed to dealloc it */
    if ((pMsg->data.pData != NULL) && (dealloc == TRUE))
        ICall_free(pMsg->data.pData);


    /* done processing the message/event, return */
    return;
}




/*
   BlinkerPeripheral_processGapMessage(gapEventHdr_t *)

   Description:      This function processes the GAP event messages received
                     by this task from the BLE stack.

   Operation:        The function processes the messages based on the opcode
                     that generated the message. Initialization done events
                     cause the system ID to be set and advertising to start.
                     Link established events cause the connection handle to be
                     stored and advertising to stop.  Link termination events
                     forget the saved connection handle and start advertising
                     again.  Unknown opcodes/event types are ignored.

   Arguments:        pMsg (gapEventHdr_t *) - pointer to the GAP event message
                                              to process.
   Return Value:     None.
   Exceptions:       None.

   Inputs:           None.
   Outputs:          None.

   Error Handling:   Unknown event opcodes are silently ignored.  In the debug
                     version if a BLE function fails the system goes into an
                     infinite loop.

   Algorithms:       None.
   Data Structures:  None.

   Revision History: 03/10/22  Glen George      initial revision
*/

static  void  BlinkerPeripheral_processGapMessage(gapEventHdr_t *pMsg)
{
    /* variables */
    uint8_t  systemID[DEVINFO_SYSTEM_ID_LEN];     /* system ID */



    /* process the message based on the opcode that generated it */
    switch(pMsg->opcode)  {

        case GAP_DEVICE_INIT_DONE_EVENT:
                /* GAP is done with intitialization, check if successful */
                if(((gapDeviceInitDoneEvent_t *) pMsg)->hdr.status == SUCCESS)  {

                    /* successfully initialized the GAP device */
                    /* create system ID using the 6 byte address (just for demo) */
                    systemID[0] = ((gapDeviceInitDoneEvent_t *) pMsg)->devAddr[0];
                    systemID[1] = ((gapDeviceInitDoneEvent_t *) pMsg)->devAddr[1];
                    systemID[2] = ((gapDeviceInitDoneEvent_t *) pMsg)->devAddr[2];
                    systemID[3] = 0x00;
                    systemID[4] = 0x00;
                    systemID[5] = ((gapDeviceInitDoneEvent_t *) pMsg)->devAddr[3];
                    systemID[6] = ((gapDeviceInitDoneEvent_t *) pMsg)->devAddr[4];
                    systemID[7] = ((gapDeviceInitDoneEvent_t *) pMsg)->devAddr[5];

                    /* set the sysytem ID in the Device Info Service */
                    DevInfo_SetParameter(DEVINFO_SYSTEM_ID, DEVINFO_SYSTEM_ID_LEN, systemID);

                    /* setup and start advertising */

                    /* create Advertisement set #1 and assign handle */
                    BP_VERIFY(GapAdv_create(&BlinkerPeripheral_advCallback, &advParams1,
                                            &advHandleLegacy));
                    /* load advertising data for set #1 (setup in sysconfig) */
                    BP_VERIFY(GapAdv_loadByHandle(advHandleLegacy, GAP_ADV_DATA_TYPE_ADV,
                                                  sizeof(advData1), advData1));
                    /* load scan response data for set #1 (setup in sysconfig) */
                    BP_VERIFY(GapAdv_loadByHandle(advHandleLegacy, GAP_ADV_DATA_TYPE_SCAN_RSP,
                                                  sizeof(scanResData1), scanResData1));
                    /* set event mask for set #1 */
                    BP_VERIFY(GapAdv_setEventMask(advHandleLegacy,
                                                  GAP_ADV_EVT_MASK_START_AFTER_ENABLE |
                                                  GAP_ADV_EVT_MASK_END_AFTER_DISABLE |
                                                  GAP_ADV_EVT_MASK_SET_TERMINATED));
                    /* enable legacy advertising for set #1 */
                    BP_VERIFY(GapAdv_enable(advHandleLegacy, GAP_ADV_ENABLE_OPTIONS_USE_MAX , 0));

                    /* create Advertisement set #2 and assign handle */
                    BP_VERIFY(GapAdv_create(&BlinkerPeripheral_advCallback, &advParams2,
                                            &advHandleLongRange));
                    /* load advertising data for set #2 (setup in sysconfig) */
                    BP_VERIFY(GapAdv_loadByHandle(advHandleLongRange, GAP_ADV_DATA_TYPE_ADV,
                                                  sizeof(advData2), advData2));
                    /* set event mask for set #2 */
                    BP_VERIFY(GapAdv_setEventMask(advHandleLongRange,
                                                  GAP_ADV_EVT_MASK_START_AFTER_ENABLE |
                                                  GAP_ADV_EVT_MASK_END_AFTER_DISABLE |
                                                  GAP_ADV_EVT_MASK_SET_TERMINATED));
                    /* enable long range advertising for set #2 */
                    BP_VERIFY(GapAdv_enable(advHandleLongRange, GAP_ADV_ENABLE_OPTIONS_USE_MAX , 0));
            }
            break;

        case GAP_LINK_ESTABLISHED_EVENT:
                /* link was established, make sure it was successful */
                if (((gapEstLinkReqEvent_t *) pMsg)->hdr.status == SUCCESS)  {

                    /* have a connection - remember it (only 1 allowed) */
                    curr_conn_handle = ((gapEstLinkReqEvent_t *) pMsg)->connectionHandle;

                    /* stop advertising since only one connection is allowed */
                    GapAdv_disable(advHandleLongRange);
                    GapAdv_disable(advHandleLegacy);
                }
                break;

        case GAP_LINK_TERMINATED_EVENT:
                /* link was terminated, be sure it was this link */
                if (curr_conn_handle == ((gapTerminateLinkEvent_t *)pMsg)->connectionHandle)  {

                    /* indicate there is no connected handle */
                    curr_conn_handle = LINKDB_CONNHANDLE_INVALID;

                    /* start advertising again since there is no longer a connection */
                    GapAdv_enable(advHandleLegacy, GAP_ADV_ENABLE_OPTIONS_USE_MAX , 0);
                    GapAdv_enable(advHandleLongRange, GAP_ADV_ENABLE_OPTIONS_USE_MAX , 0);
                }
                break;

        default:
                /* unknown opcode/event - just ignore it */
                break;
    }


    /* done processing the GAP message, return */
    return;
}




/*
   BlinkerPeripheral_charValueChangeCB(uint8_t)

   Description:      This function is called by the GATT profile code when a
                     a characteristic value is changed.

   Operation:        The function copies the passed parameter ID into the data
                     of the event message and then enqueues the message).

   Arguments:        paramID (uint8_t) - the parameter ID of the value in the
                                         profile that was changed.
   Return Value:     None.
   Exceptions:       None.

   Inputs:           None.
   Outputs:          None.

   Error Handling:   If there is an error enqueuing the message the function
                     goes into an infinite loop in the debug version.

   Algorithms:       None.
   Data Structures:  None.

   Revision History: 03/10/22  Glen George      initial revision
*/

static  void  BlinkerPeripheral_charValueChangeCB(uint8_t paramID)
{
    /* variables */
    bpEvtData_t  d;             /* data to be enqueued with message */



    /* copy the parameter ID to the message data */
    d.byte = paramID;

    /* generate a characteristic changed event and pass the parameter ID as */
    /*    the message data, ignoring for errors (no dynamic allocation to */
    /*    clean up in the case of an error) */
    BP_VERIFY(BlinkerPeripheral_enqueueMsg(BP_CHAR_CHANGE_EVT, d));


    /* done with the callback, return */
    return;
}




/*
   BlinkerPeripheral_processCharValueChangeEvt(bpEvtData_t)

   Description:      This function processes characteristic value change events
                     that are passed to this task.  This is needed so that when
                     a value is changed the hardware (LEDs) can be updated to
                     match the new value.

   Operation:        The event is processed based on the passed message data.
                     The byte value in the message data has the parameter ID
                     of the characteristic whose value changed.  If it was the
                     red LED value, the red LED is turned on or off based on
                     the new value.  If it was the green LED value, the green
                     LED is turned on or off based on the new value.  Any other
                     parameter IDs are ignored.

   Arguments:        msg_data (bpEvtData_t) - the data for the message.
   Return Value:     (bool) - TRUE to indicated the passed message data should
                        be deallocated and FALSE to indicate it is not to be
                        deallocated (FALSE is always returned).
   Exceptions:       None.

   Inputs:           None.
   Outputs:          None.

   Error Handling:   Invalid parameter ID's are silently ignored.

   Algorithms:       None.
   Data Structures:  None.

   Revision History: 03/10/22  Glen George      initial revision
*/

static  bool  BlinkerPeripheral_processCharValueChangeEvt(bpEvtData_t msg_data)
{
    /* variables */
    uint8_t  newValue;      /* new value of the changed characteristic value */



    /* figure out which characteristic changed (byte of the message data has the ID) */
    switch(msg_data.byte)    {

        case BLINKERPROFILE_REDLED:
                /* changed the red LED characteristic value */
                /* get the new value and set the red LED to this value */
                BlinkerProfile_GetParameter(BLINKERPROFILE_REDLED, &newValue);
                SetRedLED(newValue);
                break;

        case BLINKERPROFILE_GREENLED:
                /* changed the green LED characteristic value */
                /* get the new value and set the green LED to this value */
                BlinkerProfile_GetParameter(BLINKERPROFILE_GREENLED, &newValue);
                SetGreenLED(newValue);
                break;

        default:
                /* unknown parameter ID, shouldn't get here, do nothing */
                break;
    }


    /* done processing the characteristic value changed event */
    /* return that there is no memory to deallocate */
    return  FALSE;
}




/*
   BlinkerPeripheral_advCallback(uint32_t event, void *pBuf, uintptr_t arg)

   Description:      This is the callback function for the GAP advertising
                     module.  It will be called when a masked advertising
                     event occurs.

   Operation:        The function dynamically allocates memory for the passed
                     message structure.  If this is successfull it copies the
                     passed data to the memory and enqueues the event.

   Arguments:        event (uint32_t) - type of advertising event that occurred.
                     pBuf (void *)    - pointer to the buffer containing the
                                        advertising event data/message.
                     arg (uintptr_t)  - unused.
   Return Value:     None.
   Exceptions:       None.

   Inputs:           None.
   Outputs:          None.

   Error Handling:   If memory can't be allocated the event is ignored.  If
                     there is an error enqueuing the message the allocated
                     memory is free and no event is enqueued.

   Algorithms:       None.
   Data Structures:  None.

   Revision History: 03/10/22  Glen George      initial revision
*/

static  void  BlinkerPeripheral_advCallback(uint32_t event, void *pBuf, uintptr_t arg)
{
    /* variables */
    bpEvtData_t  data;          /* data to associate with the event */



    /* need to dynamically allocate space for advertising event structure */
    data.pData = ICall_malloc(sizeof(bpGapAdvEventData_t));

    /* check if got the memory */
    if (data.pData != NULL)  {

        /* got the memory, copy the advertising event information into it */
        ((bpGapAdvEventData_t *) (data.pData))->event = event;
        ((bpGapAdvEventData_t *) (data.pData))->pBuf = pBuf;

        /* enqueue the event, watching for error */
        if(BlinkerPeripheral_enqueueMsg(BP_ADV_EVT, data) != SUCCESS)
            /* error enqueuing the event - deallocate memory now */
            ICall_free(data.pData);
    }


    /* done processing the advertising even, return */
    return;
}




/*
   BlinkerPeripheral_processAdvEvent(bpEvtData_t)

   Description:      This function processes advertising events that are passed
                     to this task.  No advertising events are actually
                     processed by this task, it just frees the memory
                     associated with the message.

   Operation:        If the advertising event is not an insufficient memory
                     event, the dynamically allocated buffer associated with
                     the message is freed.  It is always the case that the
                     message itself should be freed.

   Arguments:        eventData (bpEvtData_t) - the data for the event.
   Return Value:     (bool) - TRUE to indicated the passed message data should
                        be deallocated and FALSE to indicate it is not to be
                        deallocated (TRUE is always returned).
   Exceptions:       None.

   Inputs:           None.
   Outputs:          None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Revision History: 03/10/22  Glen George      initial revision
*/

static  bool  BlinkerPeripheral_processAdvEvent(bpEvtData_t eventData)
{
    /* variables */
      /* none */



    /* all events have a buffer to free except the insufficient memory event */
    if (((bpGapAdvEventData_t *) (eventData.pData))->event != GAP_EVT_INSUFFICIENT_MEMORY)
        /* not an insufficient memory event, so free the original buffer */
        ICall_free(((bpGapAdvEventData_t *) (eventData.pData))->pBuf);


    /* done, return indicating data was dynamically allocated and needs to be freed */
    return  TRUE;
}




/*
   BlinkerPeripheral_updateGreen()

   Description:      This function processes requests to update the value of
                     the green LED.

   Operation:        The value of the green LED toggle parameter is set to 0
                     to cause a notification/indication.

   Arguments:        None.
   Return Value:     None.
   Exceptions:       None.

   Inputs:           None.
   Outputs:          None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Revision History: 03/10/22  Glen George      initial revision
*/
void  BlinkerPeripheral_updateGreen(void)
{
    /* variables */
    uint8_t  dummy = 0;         /* dummy parameter value */

    if (curr_conn_handle == LINKDB_CONNHANDLE_INVALID) return;


    /* set the parameter value to cause a notification/indication */
    //BlinkerProfile_SetParameter(BLINKERPROFILE_GREENTOGGLE, sizeof(uint8_t), &dummy);

    bpEvtData_t data;
    data.byte = 0;
    BlinkerPeripheral_enqueueMsg(BP_GREEN_TOGGLE, data);


    /* done requesting an update of the green LED, return */
    return;
}




/*
   BlinkerPeripheral_updateRed()

   Description:      This function processes requests to update the value of
                     the red LED.

   Operation:        The value of the red LED toggle parameter is set to 0 to
                     cause a notification/indication.

   Arguments:        None.
   Return Value:     None.
   Exceptions:       None.

   Inputs:           None.
   Outputs:          None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Revision History: 03/10/22  Glen George      initial revision
*/

void  BlinkerPeripheral_updateRed(void)
{
    /* variables */
    uint8_t  dummy = 0;         /* dummy parameter value */



    /* set the parameter value to cause a notification/indication */
    BlinkerProfile_SetParameter(BLINKERPROFILE_REDTOGGLE, sizeof(uint8_t), &dummy);


    /* done requesting an update of the red LED, return */
    return;
}




/*
   BlinkerPeripheral_enqueueMsg(uint8_t, bpEvtData_t)

   Description:      This function creates a message and puts it into the
                     RTOS queue.

   Operation:        The function dynamically allocates memory for the message
                     and then copies the passed event and data into this
                     message.  The message is then enqueued using the RTOS
                     function that also creates an event on enqueuing.

   Arguments:        event (uint8_t)    - event ID for the message to enqueue.
                     data (bpEvtData_t) - data for the message to enqueue.
   Return Value:     (status_t) - SUCCESS if the message was successfully
                        enqueued, FAILURE if there was an error engueuing the
                        message, and bleMemAllocError if there was an error
                        allocating memory for the message.
   Exceptions:       None.

   Inputs:           None.
   Outputs:          None.

   Error Handling:   If there is an error allocating memory for the message,
                     no message is enqueued and bleMemAllocError is returned.
                     If there is an error enqueuing the message, FAILURE is
                     returned.

   Algorithms:       None.
   Data Structures:  None.

   Revision History: 03/10/22  Glen George      initial revision
*/

static  status_t  BlinkerPeripheral_enqueueMsg(uint8_t event, bpEvtData_t data)
{
    /* variables */
    bpEvt_t   *pMsg;            /* the dynamically allocated enqueued message */

    status_t   success;         /* whether or not enqueuing was successful */



    /* allocate memory for the message */
    pMsg = ICall_malloc(sizeof(bpEvt_t));

    /* check if memory got allocated */
    if (pMsg != NULL)  {

        /* memory was allocated, create the event message */
        pMsg->event = event;
        pMsg->data = data;

        /* enqueue the message, watching for errors */
        if (Util_enqueueMsg(appMsgQueueHandle, syncEvent, (uint8_t *)pMsg))
            /* successfully enqueued the message */
            success = SUCCESS;
        else
            /* there was an error enqueuing the message */
            /*    note - Util_enqueueMsg() freed the message on failure */
            success = FAILURE;
    }
    else  {

        /* memory allocation error, let caller know */
        success = bleMemAllocError;
    }


    /* done enqueuing the message, return the error status */
    return  success;
}




/*
   BlinkerPeripheral_spin()

   Description:      This function loops infinitely in order to stall the
                     peripheral in case of an error.

   Operation:        The function implements an infinite loop.

   Arguments:        None.
   Return Value:     None.
   Exceptions:       None.

   Inputs:           None.
   Outputs:          None.

   Error Handling:   None.

   Algorithms:       None.
   Data Structures:  None.

   Revision History: 03/10/22  Glen George      initial revision
*/

static void BlinkerPeripheral_spin(void)
{
    /* variables */
    volatile  uint8_t  j = 1;   /* volatile so optimization won't kill loop */


    /* an infinite loop */
    while(j);


    /* never gets here */
    return;
}
