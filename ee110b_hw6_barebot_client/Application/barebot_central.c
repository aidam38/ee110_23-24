/****************************************************************************/
/*                                                                          */
/*                             barebot_central                           */
/*                             Barebot Central                           */
/*                               Bluetooth Demo                             */
/*                                                                          */
/****************************************************************************/

/*
 This file contains the tasks and support code for implementing the barebot
 central device for the Barebot demo.  The global functions included
 are:
 BarebotCentral_createTask  - create the barebot central task
 BarebotCentral_getState    - get the current state of the central
 BarebotCentral_read        - read a characteristic
 BarebotCentral_write       - write a characteristic

 The local functions included are:
 BarebotCentral_enqueueMsg        - enqueue a message for the task
 BarebotCentral_init              - initialize barebot central task
 BarebotCentral_processAppMsg     - process messages from the task
 BarebotCentral_processGapMessage - process GAP messages
 BarebotCentral_processStackMsg   - process BLE stack messages
 BarebotCentral_spin              - infinite loop (for debugging)
 BarebotCentral_taskFxn           - run the barebot central task
 BarebotCentral_setState          - set the state of the central
 BarebotCentral_startScanning     - start scanning for devices


 Revision History:
 3/10/22  Glen George      initial revision
 */

/* RTOS include files */
#include  <ti/sysbios/knl/Task.h>
#include  <ti/sysbios/knl/Clock.h>
#include  <ti/sysbios/knl/Event.h>
#include  <ti/sysbios/knl/Queue.h>
#include  <xdc/runtime/System.h>

/* BLE include files */
#include  <icall.h>
#include  "util.h"
#include  <bcomdef.h>
#include  <icall_ble_api.h>
#include  <devinfoservice.h>
#include  "ti_ble_config.h"
#include  "osal.h"

/* C library include files */
#include  <string.h>

/* local include files */
#include "barebot_central.h"
#include "barebot_ui_intf.h"
#include "barebot_server_constants.h"
#include "barebot_synch.h"

/* shared variables */

/* task structure and task stack */
static Task_Struct bpTask;

#pragma DATA_ALIGN(bpTaskStack, 8)
static uint8_t bpTaskStack[BC_TASK_STACK_SIZE];

/* handle for current connection */
static uint16_t curr_conn_handle;

/* entity ID used to check for source and/or destination of messages */
static ICall_EntityID centralEntity;

/* event used to post local events and pend on system and local events */
static ICall_SyncHandle syncEvent;

/* queue object used for app messages */
static Queue_Struct appMsgQueue;
static Queue_Handle appMsgQueueHandle;

/* event used to signal that a read operation has completed */
static Event_Struct readEvent;
static Event_Handle readEventHandle;

/* read response buffers */
static uint16 readLen;
static uint8 readValue[BC_MAX_READ_VALUE_LENGTH];

/* error response buffer */
static attErrorRsp_t errorRsp;

/* GATT handles */
static uint16_t barebotProfileServiceStartHandle;
static uint16_t barebotProfileServiceEndHandle;
static uint16_t speedCharHandle;
static uint16_t turnCharHandle;
static uint16_t speedUpdateCharHandle;
static uint16_t turnUpdateCharHandle;
static uint16_t thoughtsCharHandle;

/* state of the central */
static uint8 centralState;

/* functions */

/*
 BarebotCentral_createTask()

 Description:      This function creates the task for running the barebot
 central.

 Operation:        The function creates the parameter structure and then
 fills in the appropriate values for the barebot central
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
void BarebotCentral_createTask(void)
{
    /* variables */
    Task_Params taskParams; /* parameters for setting up task */

    /* configure task */
    Task_Params_init(&taskParams);
    taskParams.stack = bpTaskStack;
    taskParams.stackSize = BC_TASK_STACK_SIZE;
    taskParams.priority = BC_TASK_PRIORITY;

    /* and create the task */
    Task_construct(&bpTask, BarebotCentral_taskFxn, &taskParams, NULL);

    /* done creating the task, return */
    return;
}

/*
 BarebotCentral_init()

 Description:      This function initializes the barebot central task.  It
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
static void BarebotCentral_init(void)
{
    /* variables */
    /* none */

    /* register the current thread as an ICall dispatcher application */
    /* so that the application can send and receive messages */
    ICall_registerApp(&centralEntity, &syncEvent);

    /* create an RTOS queue for message from profile to be sent to app */
    appMsgQueueHandle = Util_constructQueue(&appMsgQueue);

    /* initialize read event struct */
    readEventHandle = Event_construct(&readEvent, NULL);

    /* set the Device Name characteristic in the GAP GATT Service */
    GGS_SetParameter(GGS_DEVICE_NAME_ATT, GAP_DEVICE_NAME_LEN, attDeviceName);

    /* configure GAP - let link take care of all parameter updates */
    GAP_SetParamValue(GAP_PARAM_LINK_UPDATE_DECISION,
                      GAP_UPDATE_REQ_ACCEPT_ALL);

    /* Initialize GATT Client */
    GATT_InitClient();

    /* register to receive incoming ATT Indications/Notifications */
    GATT_RegisterForInd(centralEntity);

    /* initialize GATT attributes */
    GGS_AddService(GAP_SERVICE); /* GAP GATT Service */
    GATTServApp_AddService(GATT_ALL_SERVICES); /* GATT Service */

    /* register with GAP for HCI/Host messages - needed to receive HCI events */
    GAP_RegisterForMsgs(centralEntity);

    /* register for GATT local events and ATT Responses pending for transmission */
    GATT_RegisterForMsgs(centralEntity);

    /* set default values for Data Length Extension (enabled by default) */
    HCI_LE_WriteSuggestedDefaultDataLenCmd(BC_SUGGESTED_PDU_SIZE,
                                           BC_SUGGESTED_TX_TIME);

    /* initialize GAP layer for Central role and register to receive GAP events */
    GAP_DeviceInit(GAP_PROFILE_CENTRAL, centralEntity, DEFAULT_ADDRESS_MODE,
                   &pRandomAddress);

    /* done initializing the barebot central task, return */
    return;
}

/*
 BarebotCentral_taskFxn(UArg, UArg)

 Description:      This function runs the task that implements the Bluetooth
 barebot central device.

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
static void BarebotCentral_taskFxn(UArg a0, UArg a1)
{
    /* variables */
    uint32_t events; /* task event flags */

    ICall_ServiceEnum src; /* source and destination for BLE */
    ICall_EntityID dest; /*    stack messages */
    ICall_HciExtEvt *pMsg = NULL; /* HCI event message pointer */

    bpEvt_t *pEvtMsg; /* internal task event/message */

    uint8_t safeToDealloc = TRUE; /* whether or not to deallocate message */

    /* wait for UI to initialize */
    Event_pend(uiInitDoneHandle, Event_Id_NONE, INIT_ALL_EVENTS,
    ICALL_TIMEOUT_FOREVER);

    /* set state to initializing */
    BarebotCentral_setState(BC_STATE_INITIALIZING);

    /* initialize the task */
    BarebotCentral_init();

    /* main task loop just loops forever */
    for (;;)
    {

        /* wait for event to be posted associated with the calling thread */
        /*    this includes events that are posted when a message is queued */
        /*    to the message receive queue of the thread */
        events = Event_pend(syncEvent, Event_Id_NONE, BC_ALL_EVENTS,
        ICALL_TIMEOUT_FOREVER);
        /* if there is an event, process it */
        if (events)
        {

            /* have an event, get any available messages from the stack */
            if (ICall_fetchServiceMsg(&src, &dest,
                                      (void**) &pMsg) == ICALL_ERRNO_SUCCESS)
            {

                /* check if it is a BLE message for this task */
                if ((src == ICALL_SERVICE_CLASS_BLE) && (dest == centralEntity))
                {

                    /* check for BLE stack events first */
                    if (((ICall_Stack_Event*) pMsg)->signature != 0xffff)
                    {
                        /* have an inter-task message, process it */
                        switch (((ICall_Hdr*) pMsg)->event)
                        {
                        case GAP_MSG_EVENT:
                            /* process GAP message */
                            BarebotCentral_processGapMessage(
                                    (gapEventHdr_t*) pMsg);
                            break;

                        case GATT_MSG_EVENT:
                            /* process GATT message - nothing to process */
                            BarebotCentral_processGattMessage(
                                    (gattMsgEvent_t*) pMsg);
                            break;

                        case HCI_GAP_EVENT_EVENT:
                            /* if got an error, spin, otherwise ignore the message */
                            if (((ICall_Hdr*) pMsg)->status
                                    == HCI_BLE_HARDWARE_ERROR_EVENT_CODE)
                                BarebotCentral_spin();
                            break;

                        default:
                            /* unknown event type - do nothing */
                            break;
                        }
                        safeToDealloc = TRUE;
                    }
                }

                /* if there is a message and it can be deallocated, do so */
                if ((pMsg != NULL) && safeToDealloc)
                    ICall_freeMsg(pMsg);
            }

            /* next check if got an RTOS queue event */
            if (events & UTIL_QUEUE_EVENT_ID)
            {

                /* process all RTOS queue entries until queue is empty */
                while (!Queue_empty(appMsgQueueHandle))
                {

                    /* dequeue the message */
                    pEvtMsg = (bpEvt_t*) Util_dequeueMsg(appMsgQueueHandle);

                    /* check if got a message */
                    if (pEvtMsg != NULL)
                    {

                        /* got a message so process the message */
                        BarebotCentral_processAppMsg(pEvtMsg);
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

/* message processing */

/*
 BarebotCentral_processGapMessage(gapEventHdr_t *)

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
static void BarebotCentral_processGapMessage(gapEventHdr_t *pMsg)
{
    /* variables */
    uint8_t temp8; /* 8-bit buffer to hold configuration values */
    uint16_t temp16; /* 16-bit buffer to hold configuration values */

    /* process the message based on the opcode that generated it */
    switch (pMsg->opcode)
    {

    case GAP_DEVICE_INIT_DONE_EVENT:
        /* GAP is done with intitialization, check if successful */
        if (((gapDeviceInitDoneEvent_t*) pMsg)->hdr.status != SUCCESS)
            break;

        /* register callback to process Scanner events */
        GapScan_registerCb(BarebotCentral_scanCb, NULL);

        /* we only need advertising reports */
        GapScan_setEventMask(
                GAP_EVT_ADV_REPORT | GAP_EVT_SCAN_DUR_ENDED | GAP_EVT_SCAN_PRD_ENDED);

        /* set default scan PHY parameters*/
        GapScan_setPhyParams(DEFAULT_SCAN_PHY, DEFAULT_SCAN_TYPE,
                             DEFAULT_SCAN_INTERVAL, DEFAULT_SCAN_WINDOW);

        /* Set Advertising report fields to keep (address and address type) */
        temp16 = ADV_RPT_FIELDS;
        GapScan_setParam(SCAN_PARAM_RPT_FIELDS, &temp16);

        /* Set Scanning Primary PHY */
        temp8 = DEFAULT_SCAN_PHY;
        GapScan_setParam(SCAN_PARAM_PRIM_PHYS, &temp8);

        /* Set LL Duplicate Filter (enable duplicate packet filtering) */
        temp8 = SCANNER_DUPLICATE_FILTER;
        GapScan_setParam(SCAN_PARAM_FLT_DUP, &temp8);

        /* Only 'Connectable' and 'Complete' packets are desired. */
        //temp16 = 0;
        temp16 = SCAN_FLT_PDU_CONNECTABLE_ONLY | SCAN_FLT_PDU_COMPLETE_ONLY;
        GapScan_setParam(SCAN_PARAM_FLT_PDU_TYPE, &temp16);

        /* Set initiating PHY parameters */
        GapInit_setPhyParam(DEFAULT_INIT_PHY, INIT_PHYPARAM_CONN_INT_MIN,
                            INIT_PHYPARAM_MIN_CONN_INT);
        GapInit_setPhyParam(DEFAULT_INIT_PHY, INIT_PHYPARAM_CONN_INT_MAX,
                            INIT_PHYPARAM_MAX_CONN_INT);

        /* ===at this point all scanning and initiating parameters are set up == */

        BarebotCentral_startScanning();

        break;

    case GAP_LINK_ESTABLISHED_EVENT:
        /* link was established, make sure it was successful */
        if (((gapEstLinkReqEvent_t*) pMsg)->hdr.status == SUCCESS)
        {

            /* have a connection - remember it (only 1 allowed) */
            curr_conn_handle = ((gapEstLinkReqEvent_t*) pMsg)->connectionHandle;

            /* stop scanning */
            GapScan_disable();

            /* discover barebot profile service */
            //GATT_DiscPrimaryServiceByUUID(curr_conn_handle,
            //                              &barebotProfileServUUID, 2,
            //                              selfEntity);
            //Display(0, 0, "Disc service", 16);
            /* service discovery doesn't work well, hardcode start and end handles for now */
            barebotProfileServiceStartHandle = 0x020;
            barebotProfileServiceEndHandle = 0xFFFF;

            /* discover all characteristics */
            GATT_DiscAllChars(curr_conn_handle,
                              barebotProfileServiceStartHandle,
                              barebotProfileServiceEndHandle, centralEntity);

            /* set state to discovering characteristics */
            BarebotCentral_setState(BC_STATE_DISC_CHARS);

            break;
        }
        break;

    case GAP_LINK_TERMINATED_EVENT:
        /* link was terminated, be sure it was this link */
        if (curr_conn_handle
                == ((gapTerminateLinkEvent_t*) pMsg)->connectionHandle)
        {

            /* indicate there is no connected handle */
            curr_conn_handle = LINKDB_CONNHANDLE_INVALID;

            /* start scanning again */
            BarebotCentral_startScanning();
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
 BarebotCentral_processAppMsg(bpEvt_t *)

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
static void BarebotCentral_processAppMsg(bpEvt_t *pMsg)
{
    /* variables */
    bool dealloc; /* whether should deallocate message data */
    GapScan_Evt_AdvRpt_t *pAdvRpt; /* event advertising report data */
    char deviceName[DEVICE_NAME_MAX_LENGTH];

    /* figure out what to do based on the message/event type */
    switch (pMsg->event)
    {
    case BC_EVT_ADV_REPORT:
        /* get report data */
        pAdvRpt = (GapScan_Evt_AdvRpt_t*) (pMsg->data.pData);

        /* get name */
        BarebotCentral_findDeviceName((uint8_t*) pAdvRpt->pData,
                                      pAdvRpt->dataLen, (char*) deviceName, 16);

        /* check if name matches the server board */
        if (osal_memcmp(deviceName, BAREBOT_SERVER_LOCAL_NAME, 2))
        {
            /* connect to it */
            GapInit_connect(pAdvRpt->addrType, pAdvRpt->addr, DEFAULT_INIT_PHY,
                            0);

            /* set state to connecting */
            BarebotCentral_setState(BC_STATE_CONNECTING);
        }

        /* Free report payload data */
        if (pAdvRpt->pData != NULL)
        {
            ICall_free(pAdvRpt->pData);
        }
        break;
    case BC_EVT_SCAN_DUR_ENDED:
        BarebotCentral_setState(BC_STATE_IDLE);
        break;
    case BC_EVT_SCAN_PRD_ENDED:
        BarebotCentral_setState(BC_STATE_SCANNING);
        break;
        /* case BC_EVT_SVC_DISCOVERED:
         GATT_DiscAllChars(curr_conn_handle, barebotProfileServiceStartHandle,
         barebotProfileServiceEndHandle, centralEntity);
         Display(0, 0, "Disc chars", 16);
         break; *//* unused */
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
 BarebotCentral_processGattMessage(gattMsgEvent_t *)

 Description:

 Operation:

 Arguments:        pMsg (gattMsgEvent_t *) - pointer to the GATT event message
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

 Revision History:
 */
static void BarebotCentral_processGattMessage(gattMsgEvent_t *pMsg)
{
    /* variables */
    bpAttReadByTypeHandlePair_t *handle_pair;

    /* check status*/
    if (pMsg->hdr.status != SUCCESS)
    {
        // TODO
    }

    switch (pMsg->method)
    {
    case ATT_ERROR_RSP:
        /* error received */
        errorRsp = pMsg->msg.errorRsp;
        BarebotCentral_setState(BC_STATE_ERROR);
        break;
    case ATT_READ_RSP:
        /* read response received */
        /* copy its contents to shared variable */
        readLen = pMsg->msg.readRsp.len;
        osal_memcpy(readValue, pMsg->msg.readRsp.pValue, readLen);
        /* unblock function that initiated read operation */
        Event_post(readEventHandle, BC_ALL_EVENTS);
        break;
    case ATT_HANDLE_VALUE_NOTI:
        /* notification received */
        if (pMsg->msg.handleValueNoti.handle == speedCharHandle)
        {
            /* update speed value in UI */
            BarebotUI_speedChanged(
                    (int16) BUILD_UINT16(pMsg->msg.handleValueNoti.pValue[0],
                                         pMsg->msg.handleValueNoti.pValue[1]));

        }
        else if (pMsg->msg.handleValueNoti.handle == turnCharHandle)
        {
            /* update turn value in UI */
            BarebotUI_turnChanged(
                    (int16) BUILD_UINT16(pMsg->msg.handleValueNoti.pValue[0],
                                         pMsg->msg.handleValueNoti.pValue[1]));

        }
        break;
    case ATT_READ_BY_TYPE_RSP:
        /* if we've already discovered all characteristics, ignore these responses */
        if (speedCharHandle != 0 && turnCharHandle != 0
                && speedUpdateCharHandle != 0 && turnUpdateCharHandle != 0
                && thoughtsCharHandle != 0)
        {
            return;
        }

        /* loop over the response and find UUID<>handle pairs */
        for (int i = 0; i < pMsg->msg.readByTypeRsp.numPairs; i++)
        {
            handle_pair =
                    (bpAttReadByTypeHandlePair_t*) &(pMsg->msg.readByTypeRsp.pDataList[i
                            * pMsg->msg.readByTypeRsp.len]);
            switch (handle_pair->uuid)
            {
            case BAREBOTPROFILE_SPEED_UUID:
                speedCharHandle = handle_pair->handle;
                break;
            case BAREBOTPROFILE_TURN_UUID:
                turnCharHandle = handle_pair->handle;
                break;
            case BAREBOTPROFILE_SPEEDUPDATE_UUID:
                speedUpdateCharHandle = handle_pair->handle;
                break;
            case BAREBOTPROFILE_TURNUPDATE_UUID:
                turnUpdateCharHandle = handle_pair->handle;
                break;
            case BAREBOTPROFILE_THOUGHTS_UUID:
                thoughtsCharHandle = handle_pair->handle;
                break;
            }
        }

        /* if all handles discovered, display ready */
        if (speedCharHandle != 0 && turnCharHandle != 0
                && speedUpdateCharHandle != 0 && turnUpdateCharHandle != 0
                && thoughtsCharHandle != 0)
        {
            BarebotCentral_setState(BC_STATE_READY);
        }
        break;
        /* unused */
        /* case ATT_FIND_BY_TYPE_VALUE_RSP:
         if (pMsg->msg.findByTypeValueRsp.numInfo == 0)
         {
         break;
         }
         barebotProfileServiceStartHandle =
         ((bpAttFindByTypeValueInfo_t*) &(pMsg->msg.findByTypeValueRsp.pHandlesInfo[0]))->startHandle;
         barebotProfileServiceEndHandle =
         ((bpAttFindByTypeValueInfo_t*) &(pMsg->msg.findByTypeValueRsp.pHandlesInfo[0]))->endHandle;

         BarebotCentral_enqueueMsg(BC_EVT_SVC_DISCOVERED,
         (bpEvtData_t) (void*) 0);
         break; */
    default:
        break;
    }
    return;
}

/* message queing */

/*
 BarebotCentral_scanCb(uint32_t event, void *pBuf, uintptr_t arg)

 Description:

 Operation:

 Arguments:
 Return Value:     None.
 Exceptions:       None.

 Inputs:           None.
 Outputs:          None.

 Error Handling:   If memory can't be allocated the event is ignored.  If
 there is an error enqueuing the message the allocated
 memory is free and no event is enqueued.

 Algorithms:       None.
 Data Structures:  None.

 Revision History:
 */
static void BarebotCentral_scanCb(uint32_t evt, void *pMsg, uintptr_t arg)
{
    /* variables */
    uint8_t event;

    /* this function is called from the BLE stack task, so
     * we should forward these events to the central task */
    if (evt & GAP_EVT_ADV_REPORT)
    {
        event = BC_EVT_ADV_REPORT;
    }
    else if (evt & GAP_EVT_SCAN_DUR_ENDED)
    {
        event = BC_EVT_SCAN_DUR_ENDED;
    }
    else if (evt & GAP_EVT_SCAN_PRD_ENDED)
    {
        event = BC_EVT_SCAN_PRD_ENDED;
    }
    else if (evt & GAP_EVT_INSUFFICIENT_MEMORY)
    {
        event = BC_EVT_INSUFFICIENT_MEM;
    }
    else
    {
        return;
    }

    if (BarebotCentral_enqueueMsg(event, (bpEvtData_t) pMsg) != SUCCESS)
    {
        ICall_free(pMsg);
    }
    return;
}

/* helper functions */


/*
 BarebotCentral_enqueueMsg(uint8_t, bpEvtData_t)

 Description:      Starts scanning for devices.

 Operation:        

 Arguments:        None.

 Inputs:           None.
 Outputs:          None.

 Error Handling:   None

 Algorithms:       None.
 Data Structures:  None.

 Revision History:  /15/24  Adam Krivka      initial revision

 */
static void BarebotCentral_startScanning()
{
    /* start scanning */
    GapScan_enable(BC_SCAN_PERIOD, DEFAULT_SCAN_DURATION, 0);

    /* set state to scanning */
    BarebotCentral_setState(BC_STATE_SCANNING);
}

/*
 BarebotCentral_enqueueMsg(uint8_t, bpEvtData_t)

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
static status_t BarebotCentral_enqueueMsg(uint8_t event, bpEvtData_t data)
{
    /* variables */
    bpEvt_t *pMsg; /* the dynamically allocated enqueued message */

    status_t success; /* whether or not enqueuing was successful */

    /* allocate memory for the message */
    pMsg = ICall_malloc(sizeof(bpEvt_t));

    /* check if memory got allocated */
    if (pMsg != NULL)
    {

        /* memory was allocated, create the event message */
        pMsg->event = event;
        pMsg->data = data;

        /* enqueue the message, watching for errors */
        if (Util_enqueueMsg(appMsgQueueHandle, syncEvent, (uint8_t*) pMsg))
            /* successfully enqueued the message */
            success = SUCCESS;
        else
            /* there was an error enqueuing the message */
            /*    note - Util_enqueueMsg() freed the message on failure */
            success = FAILURE;
    }
    else
    {

        /* memory allocation error, let caller know */
        success = bleMemAllocError;
    }

    /* done enqueuing the message, return the error status */
    return success;
}

/*
 BarebotCentral_findDeviceName(uint8_t *, uint16_t, char *, uint8_t)

 Description:       This function finds the device name in the advertisement
                    data.

 Operation:         The function parses the advertisement data looking for
                    the device name.  If it finds the device name it copies
                    it into the passed buffer.

 Arguments:         pData (uint8_t *) - pointer to the advertisement data.
                    dataLen (uint16_t) - length of the advertisement data.
                    deviceName (char *) - buffer to hold the device name.
                    maxNameLength (uint8_t) - maximum length of the device name.
 Return Value:      (bool) - TRUE if the device name was found, FALSE if it
                    was not found.
 Exceptions:        None.

 Inputs:            None.
 Outputs:           None.

 Error Handling:    None.

 Algorithms:        None.
 Data Structures:   None.

 Source: https://e2e.ti.com/support/wireless-connectivity/bluetooth-group/bluetooth/f/bluetooth-forum/1258776/cc2642r-display-advertiser-name-in-simple_central/4773912?tisearch=e2e-sitesearch&keymatch=get%252520device%252520name%252520from%252520advertising%252520report#4773912
 */
static bool BarebotCentral_findDeviceName(uint8_t *pData, uint16_t dataLen,
                                          char *deviceName,
                                          uint8_t maxNameLength)
{
    uint8_t adLen;
    uint8_t adType;
    uint8_t *pEnd;

    // Erase the content of deviceName
    {
        int i;
        for (i = 0; i < maxNameLength; i++)
        {
            deviceName[i] = '\0';
        }
    }

    // Parse the advertisement data
    if (dataLen > 0)
    {
        pEnd = pData + dataLen - 1;

        // While end of data not reached
        while (pData < pEnd)
        {
            // Get length of next AD item
            adLen = *pData++;

            if (adLen > 0)
            {
                adType = *pData++;

                // If AD type is for device name
                if ((adType == GAP_ADTYPE_LOCAL_NAME_SHORT)
                        || (adType == GAP_ADTYPE_LOCAL_NAME_COMPLETE))
                {
                    // For the whole length of the device name found
                    int i;
                    for (i = 0; i <= adLen - 1; i++) // looping until (adLen-1) because adLen also accounts for the size of the adType (1 byte)
                    {
                        if (i < maxNameLength)
                        {
                            deviceName[i] = (char) *pData++;
                        }
                    }

                    return TRUE;
                }

                else
                {
                    // Go to next item
                    pData += adLen;
                }
            }
        }
    }
}

/*
 BarebotCentral_read(uint8)

 Description:       This function reads a characteristic from the server.

 Operation:         The function sends a read request to the server and waits
                    for the response.

 Arguments:         charID (uint8) - ID of the characteristic to read.
 Return Value:      (bcReadRsp_t) - response to the read request.
 Exceptions:        None.

 Inputs:            None.
 Outputs:           None.

 Error Handling:    None.

 Algorithms:        None.
 Data Structures:   None.

 Revision History:  3/15/24  Adam Krivka      initial revision

 */
bcReadRsp_t BarebotCentral_read(uint8_t charID)
{
    /* variables */
    attReadReq_t req; /* read request struct */
    uint32_t events; /* read event */

    /* get associated char handle */
    switch (charID)
    {
    case BAREBOTPROFILE_SPEED:
        req.handle = speedCharHandle;
        break;
    case BAREBOTPROFILE_TURN:
        req.handle = turnCharHandle;
        break;
    case BAREBOTPROFILE_THOUGHTS:
        req.handle = thoughtsCharHandle;
        break;
    default:
        // handle invalid charID
        break;
    }

    /* start a read operation */
    GATT_ReadCharValue(curr_conn_handle, &req, centralEntity);

    /* wait until read operation is done */
    events = Event_pend(readEventHandle, Event_Id_NONE, BC_ALL_EVENTS,
    ICALL_TIMEOUT_FOREVER);

    /* received event */
    if (events & UTIL_QUEUE_EVENT_ID)
    {
        /* allocate a new struct for the response */
        bcReadRsp_t rsp;
        rsp.len = readLen;
        rsp.pValue = ICall_malloc(readLen * sizeof(uint8));

        /* copy the data into it */
        memcpy(rsp.pValue, readValue, readLen);

        /* return struct */
        return rsp;
    }
}

/*
 BarebotCentral_write(uint8, uint8 *)

 Description:       This function writes a characteristic to the server.

 Operation:         The function sends a write request to the server.

 Arguments:         charID (uint8) - ID of the characteristic to write.
                    newValue (uint8 *) - new value to write to the characteristic.
 Return Value:      (bool) - TRUE if the write was successful, FALSE if it was not.
 Exceptions:        None.

 Inputs:            None.
 Outputs:           None.

 Error Handling:    None.

 Algorithms:        None.
 Data Structures:   None.

 Revision History:  3/15/24  Adam Krivka      initial revision
 */
bool BarebotCentral_write(uint8 charID, uint8 *newValue)
{
    /* variables */
    attWriteReq_t req; /* write request struct */

    /* get handle and length */
    switch (charID)
    {
    case BAREBOTPROFILE_SPEED:
        req.handle = speedCharHandle;
        req.len = BAREBOTPROFILE_SPEED_LEN;
        break;
    case BAREBOTPROFILE_TURN:
        req.handle = turnCharHandle;
        req.len = BAREBOTPROFILE_TURN_LEN;
        break;
    case BAREBOTPROFILE_SPEEDUPDATE:
        req.handle = speedUpdateCharHandle;
        req.len = BAREBOTPROFILE_SPEEDUPDATE_LEN;
        break;
    case BAREBOTPROFILE_TURNUPDATE:
        req.handle = turnUpdateCharHandle;
        req.len = BAREBOTPROFILE_TURNUPDATE_LEN;
        break;
    case BAREBOTPROFILE_THOUGHTS:
        req.handle = thoughtsCharHandle;
        req.len = BAREBOTPROFILE_THOUGHTS_LEN;
        break;
    }

    /* allocate data with GATT specific function */
    req.pValue = GATT_bm_alloc(curr_conn_handle, ATT_WRITE_REQ, 1, NULL);
    memcpy(req.pValue, newValue, req.len);

    /* no signature or command (not really sure what they do) */
    req.sig = 0;
    req.cmd = 0;

    /* send write request */
    BC_VERIFY(GATT_WriteCharValue(curr_conn_handle, &req, centralEntity));

    return (true);
}

/*
 BarebotCentral_setState(uint8)

 Description:       This function sets the state of the central.

 Arguments:         newState (uint8) - new state of the central.
 Return Value:      None.
 Exceptions:        None.

 Inputs:            None.
 Outputs:           None.

 Error Handling:    None.

 Algorithms:        None.
 Data Structures:   None.

 Revision History:  3/15/24  Adam Krivka      initial revision
 */
static void BarebotCentral_setState(uint8 newState)
{
    centralState = newState;
    BarebotUI_centralStateChanged(centralState);
}

/*
 BarebotCentral_getState(void)

 Description:       This function gets the state of the central.

 Operation:         The function returns the current state of the central.

 Arguments:         None.
 Return Value:      (uint8) - current state of the central.
 Exceptions:        None.

 Inputs:            None.
 Outputs:           None.

 Error Handling:    None.

 Algorithms:        None.
 Data Structures:   None.

 Revision History:  3/15/24  Adam Krivka      initial revision
 */
uint8 BarebotCentral_getState()
{
    return centralState;
}

/*
 BarebotCentral_spin()

 Description:      This function loops infinitely in order to stall the
 central in case of an error.

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
static void BarebotCentral_spin(void)
{
    /* variables */
    volatile uint8_t j = 1; /* volatile so optimization won't kill loop */

    /* an infinite loop */
    while (j)
        ;

    /* never gets here */
    return;
}
