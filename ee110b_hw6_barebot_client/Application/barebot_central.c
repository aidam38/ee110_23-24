/****************************************************************************/
/*                                                                          */
/*                             barebot_central                           */
/*                             Barebot Central                           */
/*                               Bluetooth Demo                             */
/*                                                                          */
/****************************************************************************/

/*
   This file contains the tasks and support code for implementing the barebot
   central device for the Bluetooth demo.  The global functions included
   are:
      BarebotCentral_createTask  - create the barebot central task
      BarebotCentral_updateRed   - want to update the red LED
      BarebotCentral_updateGreen - want to update the green LED

   The local functions included are:
      BarebotCentral_enqueueMsg        - enqueue a message for the task
      BarebotCentral_init              - initialize barebot central task
      BarebotCentral_processAppMsg     - process messages from the task
      BarebotCentral_processGapMessage - process GAP messages
      BarebotCentral_processStackMsg   - process BLE stack messages
      BarebotCentral_spin              - infinite loop (for debugging)
      BarebotCentral_taskFxn           - run the barebot central task


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
#include "lcd/lcd_rtos_intf.h"



/* shared variables */

/* task structure and task stack */
static  Task_Struct  bpTask;

#pragma DATA_ALIGN(bpTaskStack, 8)
static  uint8_t  bpTaskStack[BC_TASK_STACK_SIZE];

/* handle for current connection */
static   uint16_t  curr_conn_handle;

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

/* text buffer for one line of the display to use with sprintf */
char textBuf[17];    /* text buffer for printing to Display */


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

void  BarebotCentral_createTask(void)
{
    /* variables */
    Task_Params  taskParams;            /* parameters for setting up task */



    /* configure task */
    Task_Params_init(&taskParams);
    taskParams.stack     = bpTaskStack;
    taskParams.stackSize = BC_TASK_STACK_SIZE;
    taskParams.priority  = BC_TASK_PRIORITY;

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

static  void  BarebotCentral_init(void)
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
    GAP_SetParamValue(GAP_PARAM_LINK_UPDATE_DECISION, GAP_UPDATE_REQ_ACCEPT_ALL);

    /* Initialize GATT Client */
    GATT_InitClient();

    /* register to receive incoming ATT Indications/Notifications */
    GATT_RegisterForInd(selfEntity);

    /* initialize GATT attributes */
    GGS_AddService(GAP_SERVICE);                    /* GAP GATT Service */
    GATTServApp_AddService(GATT_ALL_SERVICES);      /* GATT Service */

    /* register with GAP for HCI/Host messages - needed to receive HCI events */
    GAP_RegisterForMsgs(selfEntity);

    /* register for GATT local events and ATT Responses pending for transmission */
    GATT_RegisterForMsgs(selfEntity);


    /* set default values for Data Length Extension (enabled by default) */
    HCI_LE_WriteSuggestedDefaultDataLenCmd(BC_SUGGESTED_PDU_SIZE, BC_SUGGESTED_TX_TIME);


    /* initialize GAP layer for Central role and register to receive GAP events */
    GAP_DeviceInit(GAP_PROFILE_CENTRAL, selfEntity, DEFAULT_ADDRESS_MODE, &pRandomAddress);


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

static  void  BarebotCentral_taskFxn(UArg a0, UArg a1)
{
    /* variables */
    uint32_t            events;         /* task event flags */

    ICall_ServiceEnum   src;            /* source and destination for BLE */
    ICall_EntityID      dest;           /*    stack messages */
    ICall_HciExtEvt    *pMsg = NULL;    /* HCI event message pointer */

    bpEvt_t            *pEvtMsg;        /* internal task event/message */

    uint8_t             safeToDealloc = TRUE;  /* whether or not to deallocate message */


    /* initialize the task */
    BarebotCentral_init();

    /* dislay initialization message */
    Display(0, 0, "Initialized", -1);


    /* main task loop just loops forever */
    for (;;)
    {

        /* wait for event to be posted associated with the calling thread */
        /*    this includes events that are posted when a message is queued */
        /*    to the message receive queue of the thread */
        events = Event_pend(syncEvent, Event_Id_NONE, BC_ALL_EVENTS, ICALL_TIMEOUT_FOREVER);
        /* if there is an event, process it */
        if (events)  {

            /* have an event, get any available messages from the stack */
            if (ICall_fetchServiceMsg(&src, &dest, (void **)&pMsg) == ICALL_ERRNO_SUCCESS)  {

                /* check if it is a BLE message for this task */
                if ((src == ICALL_SERVICE_CLASS_BLE) && (dest == selfEntity))  {

                    /* check for BLE stack events first */
                    if (((ICall_Stack_Event *) pMsg)->signature != 0xffff)
                        /* have an inter-task message, process it */
                        safeToDealloc = BarebotCentral_processStackMsg((ICall_Hdr *)pMsg);
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




/*
   BarebotCentral_processStackMsg(ICall_Hdr *)

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

static  uint8_t  BarebotCentral_processStackMsg(ICall_Hdr *pMsg)
{
    /* variables */
      /* none */



    /* message processing is based on the type of message */
    switch (pMsg->event)  {

        case GAP_MSG_EVENT:
                /* process GAP message */
                BarebotCentral_processGapMessage((gapEventHdr_t*) pMsg);
                break;

        case GATT_MSG_EVENT:
                /* process GATT message - nothing to process */
                BarebotCentral_processGattMessage((gattMsgEvent_t *) pMsg);
                break;

        case HCI_GAP_EVENT_EVENT:
                /* if got an error, spin, otherwise ignore the message */
                if (pMsg->status == HCI_BLE_HARDWARE_ERROR_EVENT_CODE)
                        BarebotCentral_spin();
                break;

        default:
                /* unknown event type - do nothing */
                break;
    }


    /* done processing the BLE stack message, return */
    return  TRUE;
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
int n = 0;
static  void  BarebotCentral_processAppMsg(bpEvt_t *pMsg)
{
    /* variables */
    bool  dealloc;      /* whether should deallocate message data */
    static GapScan_Evt_AdvRpt_t* pAdvRpt; /* event advertising report data */


    /* figure out what to do based on the message/event type */
    switch (pMsg->event)  {
        case BC_EVT_KEY_PRESSED:

            break;
        case BC_EVT_ADV_REPORT:
            /* debug */
            n += 1;
            System_sprintf((char *)textBuf, "%d", n);
            Display(3, 13, textBuf, -1);

            /* get report data */
            pAdvRpt = (GapScan_Evt_AdvRpt_t*) (pMsg->data.pData);

            /* display name */
            BarebotCentral_findDeviceName((uint8_t *)pAdvRpt->pData, pAdvRpt->dataLen, (char *)textBuf, 16);
            Display(0, 0, (char *)textBuf, -1);

            /* display address */
            char *addr = Util_convertBdAddr2Str(pAdvRpt->addr);
            Display(1, 0, addr, -1);

            if (osal_memcmp((uint8_t *)pAdvRpt->addr, BAREBOT_SERVER_ADDR, 12)) {
                Display(2, 0, "barebot server!", -1);
            }

            /* check for address of the server board */
            if (true) {
                /* connect to it */
            }

            /* Free report payload data */
            if (pAdvRpt->pData != NULL) {
                ICall_free(pAdvRpt->pData);
            }
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

static  void  BarebotCentral_processGapMessage(gapEventHdr_t *pMsg)
{
    /* variables */
    uint8_t temp8;      /* 8-bit buffer to hold configuration values */
    uint16_t temp16;    /* 16-bit buffer to hold configuration values */



    /* process the message based on the opcode that generated it */
    switch(pMsg->opcode)  {

        case GAP_DEVICE_INIT_DONE_EVENT:
                /* GAP is done with intitialization, check if successful */
                if(((gapDeviceInitDoneEvent_t *) pMsg)->hdr.status != SUCCESS)
                    break;

                /* register callback to process Scanner events */
                GapScan_registerCb(BarebotCentral_scanCb, NULL);

                /* we only need advertising reports */
                GapScan_setEventMask(GAP_EVT_ADV_REPORT);

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
                temp16 = SCAN_FLT_PDU_CONNECTABLE_ONLY | SCAN_FLT_PDU_COMPLETE_ONLY;
                GapScan_setParam(SCAN_PARAM_FLT_PDU_TYPE, &temp16);

                /* Set initiating PHY parameters */
                GapInit_setPhyParam(DEFAULT_INIT_PHY, INIT_PHYPARAM_CONN_INT_MIN,
                                          INIT_PHYPARAM_MIN_CONN_INT);
                GapInit_setPhyParam(DEFAULT_INIT_PHY, INIT_PHYPARAM_CONN_INT_MAX,
                                          INIT_PHYPARAM_MAX_CONN_INT);

                /* === at this point all scanning and initiating parameters are set up === */

                /* start scanning */
                GapScan_enable(0, DEFAULT_SCAN_DURATION, 0);

                /* show message on Display */
                Display(0, 0, "Scanning...", -1);

            break;

        case GAP_LINK_ESTABLISHED_EVENT:
                /* link was established, make sure it was successful */
                if (((gapEstLinkReqEvent_t *) pMsg)->hdr.status == SUCCESS)  {

                    /* have a connection - remember it (only 1 allowed) */
                    curr_conn_handle = ((gapEstLinkReqEvent_t *) pMsg)->connectionHandle;

                    /* stop scanning */
                    GapScan_disable();

                    /* stopping scanning */
                    Display(1, 0, "stop scan", -1);
                }
                break;

        case GAP_LINK_TERMINATED_EVENT:
                /* link was terminated, be sure it was this link */
                if (curr_conn_handle == ((gapTerminateLinkEvent_t *)pMsg)->connectionHandle)  {

                    /* indicate there is no connected handle */
                    curr_conn_handle = LINKDB_CONNHANDLE_INVALID;

                    /* start scanning again */
                    GapScan_enable(0, DEFAULT_SCAN_DURATION, 0);
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

static  void  BarebotCentral_processGattMessage(gattMsgEvent_t *pMsg)
{
    /* variables */
    uint8_t  systemID[DEVINFO_SYSTEM_ID_LEN];     /* system ID */

    return;
}



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

static  void  BarebotCentral_scanCb(uint32_t evt, void *pMsg, uintptr_t arg)
{
    /* variables */
    uint8_t event;

    if (evt & GAP_EVT_ADV_REPORT) {
      event = BC_EVT_ADV_REPORT;
    }
    else {
      return;
    }

    if(BarebotCentral_enqueueMsg(event, (bpEvtData_t)pMsg) != SUCCESS) {
      ICall_free(pMsg);
    }
    return;
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

static  status_t  BarebotCentral_enqueueMsg(uint8_t event, bpEvtData_t data)
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
   BarebotCentral_findDeviceName(uint8_t *, uint16_t, char *, uint8_t)

   Description:

   Operation:

   Arguments:
   Return Value:
   Exceptions:       None.

   Inputs:           None.
   Outputs:          None.

   Error Handling:

   Algorithms:       None.
   Data Structures:  None.

   Revision History: https://e2e.ti.com/support/wireless-connectivity/bluetooth-group/bluetooth/f/bluetooth-forum/1258776/cc2642r-display-advertiser-name-in-simple_central/4773912?tisearch=e2e-sitesearch&keymatch=get%252520device%252520name%252520from%252520advertising%252520report#4773912
*/
static bool BarebotCentral_findDeviceName(uint8_t *pData, uint16_t dataLen, char *deviceName, uint8_t maxNameLength)
{
    uint8_t adLen;
    uint8_t adType;
    uint8_t *pEnd;

    // Erase the content of deviceName
    {
        int i;
        for (i=0; i<maxNameLength ; i++)
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
            if ((adType == GAP_ADTYPE_LOCAL_NAME_SHORT) ||
                (adType == GAP_ADTYPE_LOCAL_NAME_COMPLETE ))
            {
                // For the whole length of the device name found
                int i;
                for (i=0; i<= adLen-1; i++) // looping until (adLen-1) because adLen also accounts for the size of the adType (1 byte)
                {
                    if(i < maxNameLength)
                    {
                        deviceName[i] = (char)*pData++;
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
    volatile  uint8_t  j = 1;   /* volatile so optimization won't kill loop */


    /* an infinite loop */
    while(j);


    /* never gets here */
    return;
}
