/*
 * Copyright (c) 2015-2019, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== empty.c ========
 */

/* For usleep() */
#include <unistd.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#include <xdc/runtime/System.h>


/* Driver Header files */
#include <ti/drivers/GPIO.h>
// #include <ti/drivers/I2C.h>
// #include <ti/drivers/SPI.h>
// #include <ti/drivers/Watchdog.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Event.h>
#include  <ti/sysbios/knl/Swi.h>
#include  <ti/sysbios/runtime/Memory.h>

#include "lcd/lcd_rtos_intf.h"
#include "keypad/keypad_rtos_intf.h"



/* Driver configuration */
#include "ti_drivers_config.h"

/* local includes */
#include "haiku_app.h"

/* task structure and task stack */
static  Task_Struct  appTask;

#pragma DATA_ALIGN(appTaskStack, 8)
static  uint8_t  appTaskStack[APP_TASK_STACK_SIZE];

/* queue object used for app messages */
static Queue_Handle  appMsgQueue;

void KeyPressed(uint32_t keyEvt) {
    event_t *evt = Memory_alloc(NULL, sizeof(event_t), 0, NULL);
    evt->data = keyEvt;
    Queue_enqueue(appMsgQueue, &(evt->elem));
    return;
}

/* create task */
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

void App_init(void) {
    /* create the app event queue */
    appMsgQueue = Queue_create(NULL, NULL);
    if (appMsgQueue == NULL) {
        System_abort("Queue create failed!");
    }

    return;
}

/* run the app */
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
        while (!Queue_empty(appMsgQueue)) {
            /* process the event */
            evt = Queue_dequeue(appMsgQueue);
            App_processEvent(evt);
            Memory_free(NULL, evt, sizeof(event_t));
        }
    }
}

void App_processEvent(event_t* evt) {
    /* variables */
    uint8_t col = evt->data & 0b11111111;
    uint8_t row = evt->data >> 8;
    haiku_t haiku;
    line_t line;

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

    ClearDisplay();
    for (int i = 0; i < 4; i++) {
        Display(line.parts[i].row, line.parts[i].col, line.parts[i].text, -1);
    }

    return;
}
