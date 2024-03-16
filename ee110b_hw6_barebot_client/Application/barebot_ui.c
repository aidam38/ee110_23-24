/****************************************************************************/
/*                                                                          */
/*                             barebot_ui                           */
/*                             Barebot UI                           */
/*                               Bluetooth Demo                             */
/*                                                                          */
/****************************************************************************/

/*



 Revision History:

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
#include "barebot_ui.h"
#include "barebot_central_intf.h"
#include "barebot_server_constants.h"
#include "barebot_synch.h"
#include "lcd/lcd_rtos_intf.h"
#include "lcd/lcd_util.h"
#include "keypad/keypad_rtos_intf.h"

/* shared variables */

/* task structure and task stack */
static Task_Struct buiTask;

#pragma DATA_ALIGN(buiTaskStack, 8)
static uint8_t buiTaskStack[BUI_TASK_STACK_SIZE];

/* entity ID used to check for source and/or destination of messages */
static ICall_EntityID uiEntity;

/* event used to post local events and pend on system and local events */
static ICall_SyncHandle syncEvent;

/* queue object used for app messages */
static Queue_Struct uiMsgQueue;
static Queue_Handle uiMsgQueueHandle;

/* screen state */
static uint8_t screenState;

/* functions */

/*
 BarebotUI_createTask()

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
void BarebotUI_createTask(void)
{
    /* variables */
    Task_Params taskParams; /* parameters for setting up task */

    /* configure task */
    Task_Params_init(&taskParams);
    taskParams.stack = buiTaskStack;
    taskParams.stackSize = BUI_TASK_STACK_SIZE;
    taskParams.priority = BUI_TASK_PRIORITY;

    /* and create the task */
    Task_construct(&buiTask, BarebotUI_taskFxn, &taskParams, NULL);

    /* done creating the task, return */
    return;
}

/*
 BarebotUI_init()

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
static void BarebotUI_init(void)
{
    /* variables */
    /* none */

    /* register the current thread as an ICall dispatcher application */
    /* so that the application can send and receive messages */
    ICall_registerApp(&uiEntity, &syncEvent);

    /* create an RTOS queue for message from profile to be sent to app */
    uiMsgQueueHandle = Util_constructQueue(&uiMsgQueue);

    /* signalize that UI is done initializing */
    Event_post(uiInitDoneHandle, INIT_ALL_EVENTS);

    /* done initializing the barebot UI task, return */
    return;
}

/*
 BarebotUI_taskFxn(UArg, UArg)

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
static void BarebotUI_taskFxn(UArg a0, UArg a1)
{
    /* variables */
    uint32_t events; /* task event flags */
    buiEvt_t *pEvtMsg; /* internal task event/message */

    /* initialize the task */
    BarebotUI_init();

    /* main task loop just loops forever */
    for (;;)
    {

        /* wait for event to be posted associated with the calling thread */
        /*    this includes events that are posted when a message is queued */
        /*    to the message receive queue of the thread */
        events = Event_pend(syncEvent, Event_Id_NONE, BUI_ALL_EVENTS,
        ICALL_TIMEOUT_FOREVER);
        /* if there is an event, process it */
        if (events)
        {
            /* next check if got an RTOS queue event */
            if (events & UTIL_QUEUE_EVENT_ID)
            {
                /* process all RTOS queue entries until queue is empty */
                while (!Queue_empty(uiMsgQueueHandle))
                {

                    /* dequeue the message */
                    pEvtMsg = (buiEvt_t*) Util_dequeueMsg(uiMsgQueueHandle);

                    /* check if got a message */
                    if (pEvtMsg != NULL)
                    {
                        /* got a message so process the message */
                        BarebotUI_processUIMsg(pEvtMsg);
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
 BarebotUI_processUIMsg(bpEvt_t *)

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
static void BarebotUI_processUIMsg(buiEvt_t *pMsg)
{
    /* variables */
    bool dealloc; /* whether should deallocate message data */

    /* figure out what to do based on the message/event type */
    switch (pMsg->event)
    {
    case BUI_EVT_KEY_PRESSED:
        //                       row                   col
        BarebotUI_handleKey(pMsg->data.word >> 8, pMsg->data.word & 0b11111111);
        break;
    case BUI_EVT_SPEED_CHANGED:
        if (screenState == BUI_STATE_CONTROL)
        {
            Display_printf(1, 8, 4, "%d", (int16_t) pMsg->data.hword);
        }
        break;
    case BUI_EVT_TURN_CHANGED:
        if (screenState == BUI_STATE_CONTROL)
        {
            Display_printf(2, 8, 4, "%d", (int16_t) pMsg->data.hword);
        }
        break;
    case BUI_EVT_CENTRAL_STATE_CHANGED:
        ClearDisplay();
        switch (pMsg->data.byte)
        {
        case BC_STATE_IDLE:
            Display(0, 0, "Idle", 16);
            break;
        case BC_STATE_INITIALIZING:
            Display(0, 0, "Initializing", 16);
            break;
        case BC_STATE_SCANNING:
            Display(0, 0, "Scanning", 16);
            break;
        case BC_STATE_CONNECTING:
            Display(0, 0, "Connecting", 16);
            break;
        case BC_STATE_DISC_CHARS:
            Display(0, 0, "Disc chars", 16);
            break;
        case BC_STATE_READY:
            Display(0, 0, "Ready", 16);
            break;
        }
        break;
    default:
        /* unknown UI event - do nothing */
        dealloc = FALSE;
        break;
    }

    /* free message data if it exists and we are supposed to dealloc it */
    if ((pMsg->data.pData != NULL) && (dealloc == TRUE))
        ICall_free(pMsg->data.pData);

    /* done processing the message/event, return */
    return;
}

/*
 BarebotUI_handleKey(uint8_t row, uint8, col)

 Description:

 Operation:

 Arguments:
 Return Value:     None.
 Exceptions:       None.

 Inputs:           None.
 Outputs:          None.

 Error Handling:

 Algorithms:       None.
 Data Structures:  None.

 Revision History:
 */
void BarebotUI_handleKey(uint8_t row, uint8_t col)
{
    /* variables */
    int16_t update;

    /* menu buttons */
    if (row == 3)
    {
        /* clear display */
        ClearDisplay();

        /* menu */
        if (col == 3)
        {
            /* control screen */
            /* change the screen state */
            screenState = BUI_STATE_CONTROL;

            /* display menu title */
            Display(0, 0, "CONTROL", 16);

            /* read current speed and turn values */
            bcReadRsp_t speedRsp = BarebotCentral_read(BAREBOTPROFILE_SPEED);
            bcReadRsp_t turnRsp = BarebotCentral_read(BAREBOTPROFILE_TURN);

            /* display them */
            Display_printf(1, 0, 12, "Speed:  %d", speedRsp.pValue[0]);
            Display_printf(2, 0, 12, "Turn:   %d", turnRsp.pValue[0]);

            /* free response data */
            ICall_free(speedRsp.pValue);
            ICall_free(turnRsp.pValue);

            return;
        }
        else if (col == 2)
        {
            /* thoughts screen */
            /* change the screen state */
            screenState = BUI_STATE_THOUGHTS;

            /* display menu title */
            Display(0, 0, "THOUGHTS", 16);

            /* read current thoughts */
            bcReadRsp_t thoughtsRsp = BarebotCentral_read(
            BAREBOTPROFILE_THOUGHTS);

            /* display thoughts */
            Display(1, 0, (char*)thoughtsRsp.pValue, 16);

            /* free response data */
            ICall_free(thoughtsRsp.pValue);

            return;
        }
    }

    /* speed buttons */
    switch (screenState)
    {
    case BUI_STATE_CONTROL:
        if (row == 0 && col == 2) /* left */
        {
            update = -1;
            BarebotCentral_write(BAREBOTPROFILE_TURNUPDATE, &update);
        }
        else if (row == 0 && col == 1) /* down */
        {
            update = -1;
            BarebotCentral_write(BAREBOTPROFILE_SPEEDUPDATE, &update);
        }
        else if (row == 0 && col == 0) /* right */
        {
            update = +1;
            BarebotCentral_write(BAREBOTPROFILE_TURNUPDATE, &update);
        }
        else if (row == 1 && col == 1) /* up */
        {
            update = +1;
            BarebotCentral_write(BAREBOTPROFILE_SPEEDUPDATE, &update);
        }
        break;
    case BUI_STATE_THOUGHTS:
        /* no keys on the thoughts screen */
        break;
    }
}

/* message queing */

/*
 KeyPressed(uint32_t keyEvt)

 Description:

 Operation:

 Arguments:
 Return Value:     None.
 Exceptions:       None.

 Inputs:           None.
 Outputs:          None.

 Error Handling:

 Algorithms:       None.
 Data Structures:  None.

 Revision History:
 */
void KeyPressed(uint32_t keyEvt)
{
    buiEvtData_t data;
    data.word = keyEvt;
    BarebotUI_enqueueMsg(BUI_EVT_KEY_PRESSED, data);
    return;
}

void BarebotUI_centralStateChanged(uint8 newCentralState)
{
    buiEvtData_t data;
    data.byte = newCentralState;
    BarebotUI_enqueueMsg(BUI_EVT_CENTRAL_STATE_CHANGED, data);
}

void BarebotUI_speedChanged(int16 newSpeed)
{
    buiEvtData_t data;
    data.hword = newSpeed;
    BarebotUI_enqueueMsg(BUI_EVT_SPEED_CHANGED, data);
}

void BarebotUI_turnChanged(int16 newTurn)
{
    buiEvtData_t data;
    data.hword = newTurn;
    BarebotUI_enqueueMsg(BUI_EVT_TURN_CHANGED, data);
}

/* helper functions */
/*
 BarebotUI_enqueueMsg(uint8_t, bpEvtData_t)

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
static status_t BarebotUI_enqueueMsg(uint8_t event, buiEvtData_t data)
{
    /* variables */
    buiEvt_t *pMsg; /* the dynamically allocated enqueued message */

    status_t success; /* whether or not enqueuing was successful */

    /* allocate memory for the message */
    pMsg = ICall_malloc(sizeof(buiEvt_t));

    /* check if memory got allocated */
    if (pMsg != NULL)
    {

        /* memory was allocated, create the event message */
        pMsg->event = event;
        pMsg->data = data;

        /* enqueue the message, watching for errors */
        if (Util_enqueueMsg(uiMsgQueueHandle, syncEvent, (uint8_t*) pMsg))
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
 BarebotUI_spin()

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
static void BarebotUI_spin(void)
{
    /* variables */
    volatile uint8_t j = 1; /* volatile so optimization won't kill loop */

    /* an infinite loop */
    while (j)
        ;

    /* never gets here */
    return;
}
