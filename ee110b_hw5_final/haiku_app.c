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

/* Driver Header files */
#include <ti/drivers/GPIO.h>
// #include <ti/drivers/I2C.h>
// #include <ti/drivers/SPI.h>
// #include <ti/drivers/Watchdog.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Event.h>

/* Driver configuration */
#include "ti_drivers_config.h"

/* local includes */
#include "haiku_app.h"

/* task structure and task stack */
static  Task_Struct  appTask;

#pragma DATA_ALIGN(appTaskStack, 8)
static  uint8_t  appTaskStack[APP_TASK_STACK_SIZE];

/* queue object used for app messages */
static  Queue_Struct  appEventQueue;
static  Queue_Handle  appEventQueueHandle;

void EnqueueEvent(uint32_t evt) {
    // Queue_enqueue(appEventQueueHandle, evt);
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
   // appEventQueueHandle = Util_constructQueue(&appEventQueue);

    return;
}

/* run the app */
static void App_run(UArg a0, UArg a1) {
    /* 1 second delay */
    uint32_t time = 3;

    /* Call driver init functions */
    GPIO_init();
    // I2C_init();
    // SPI_init();
    // Watchdog_init();

    /* Configure the LED pin */
    GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);

    /* Turn on user LED */
    GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);

    while (1)
    {
        sleep(time);
        GPIO_toggle(CONFIG_GPIO_LED_0);
    }

    return;

    /* variables */

    /* initialize the app */
    App_init();

    /* main loop */
    //while (true) {
   //     if (Queue_empty(appEventQueueHandle)) {
    //        Task_yield();
    //    } else {
            /* process the event */
    //        evt = Queue_dequeue(appEventQueueHandle);
    //    }
   // }
}

void App_processEvent(uint32_t evt) {
    /* variables */

    //char[] hello_world = "Hello, World!";
    //Display(0, 0, &hello_world, 16);

    return;
}
