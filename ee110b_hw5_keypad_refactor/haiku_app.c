/****************************************************************************/
/*                                                                          */
/*                                haiku_app.c                               */
/*                            Haiku Application                             */
/*                                                                          */
/****************************************************************************/

/* Displays AI-generated haikus about embedded systems on the LCD 
    based on keypad input - each row of 4x4 keypad contains one haiku
    and pressing keys left to right displays each line (last line is which
    AI was used to generate it). 

   Revision History:
       3/6/24  Adam Krivka      initial revision
*/


/* C library */
#include <unistd.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

/* RTOS includes */
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Task.h>
#include  <ti/sysbios/knl/Swi.h>
#include  <ti/sysbios/runtime/Memory.h>

/* interface includes */
#include "lcd/lcd_rtos_intf.h"
#include "keypad/keypad_rtos_intf.h"

/* local includes */
#include "haiku_app.h"


/* shared variables */

/* task structure and task stack */
static  Task_Struct  appTask;

/* app stack */
#pragma DATA_ALIGN(appTaskStack, 8)
static  uint8_t  appTaskStack[APP_TASK_STACK_SIZE];

/* queue object used for app messages */
static Queue_Handle  appMsgQueue;

/* 
    KeyPressed(uint32_t keyEvt)
    
    Description:    This function is called by the keypad interrupt handler
                    when a key is pressed. It creates an event and enqueues
                    it to the app message queue.
*/
void KeyPressed(uint32_t keyEvt) {
    /* create event struct */
    event_t *evt = Memory_alloc(NULL, sizeof(event_t), 0, NULL);
    evt->data = keyEvt;

    /* enqueue to the app message queue */
    Queue_enqueue(appMsgQueue, &(evt->elem));

    return;
}

/* 
    App_createTask()
    
    Description:    This function creates the app task. It sets up the task
                    parameters and creates the task.
*/
void App_createTask(void) {
    /* variables */
    Task_Params  taskParams;            /* parameters for setting up task */

    /* configure task */
    Task_Params_init(&taskParams);
    taskParams.stack     = appTaskStack;
    taskParams.stackSize = APP_TASK_STACK_SIZE;
    taskParams.priority  = APP_TASK_PRIORITY;

    /* and create the task */
    Task_construct(&appTask, App_run, &taskParams, NULL);

    /* done creating the task, return */
    return;
}

/*
    App_init()
    
    Description:    This function initializes the app. It creates the app
                    message queue.
*/
void App_init(void) {
    /* create the app event queue */
    appMsgQueue = Queue_create(NULL, NULL);
    if (appMsgQueue == NULL) {
        System_abort("Queue create failed!");
    }

    return;
}

/*
    App_run()
    
    Description:    This function is the main loop of the app. It processes
                    events from the app message queue.
*/
static void App_run(UArg a0, UArg a1) {
    /* variables */
    event_t* evt;

    /* initialize the app */
    App_init();

    /* initialize hardware */
    LCDInit();
    KeypadInit_RTOS();

    /* main loop */
    while (true) {
        /* process events while there are any */
        while (!Queue_empty(appMsgQueue)) {
            /* dequeue the event */
            evt = Queue_dequeue(appMsgQueue);

            /* process the event */
            App_processEvent(evt);

            /* free because we shouldn't need event struct anymore */
            Memory_free(NULL, evt, sizeof(event_t));
        }
    }
}


/*
    App_processEvent(event_t* evt)
    
    Description:    This function processes an event from the app message
                    queue. It displays the haiku on the LCD based on the
                    keypad input.
*/
void App_processEvent(event_t* evt) {
    /* variables */
    uint8_t col = evt->data & 0b11111111;
    uint8_t row = evt->data >> 8;
    haiku_t haiku;
    line_t line;

    /* choose haiku */
    switch (row) {
        case 0:
            haiku = haiku1;
            break;
        case 1:
            haiku = haiku2;
            break;
        case 2:
            haiku = haiku3;
            break;
        case 3:
            haiku = haiku4;
            break;
        default:
            Display(0, 0, "row error", 16);
            return;
    }

    /* choose line */
    switch (col) {
        case 0:
            line = haiku.lines[3];
            break;
        case 1:
            line = haiku.lines[2];
            break;
        case 2:
            line = haiku.lines[1];
            break;
        case 3:
            line = haiku.lines[0];
            break;
        default:
            Display(0, 0, "col error", 16);
            return;
    }

    /* clear whole display before displaying new line*/
    ClearDisplay();

    /* for each line part, display it at the given row and col position*/
    for (int i = 0; i < 4; i++) {
        Display(line.parts[i].row, line.parts[i].col, line.parts[i].text, -1);
    }

    return;
}
