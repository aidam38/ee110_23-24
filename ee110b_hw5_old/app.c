/* RTOS include files */
#include  <ti/sysbios/knl/Task.h>
#include  <ti/sysbios/knl/Clock.h>
#include  <ti/sysbios/knl/Event.h>
#include  <ti/sysbios/knl/Queue.h>

/* local include files */
#include "app.h"

/* shared variables */

/* task structure and task stack */
static  Task_Struct  appTask;

#pragma DATA_ALIGN(bpTaskStack, 8)
static  uint8_t  appTaskStack[APP_TASK_STACK_SIZE];

/* queue object used for app messages */
static  Queue_Struct  appEventQueue;

void EnqueueEvent(uint32_t evt) {
	Queue_enqueue(appEventQueue, evt);
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
	appEventQueue = Queue_create(NULL, NULL);

	return;
}

/* run the app */
void App_run(void) {
	/* variables */

	/* initialize the app */
	App_init();

	/* main loop */
	while (true) {
		if (Queue_empty(appEventQueue)) {
			Task_yield();
		} else {
			/* process the event */
			evt = Queue_dequeue(appEventQueue);
		}
	}
}

void App_processEvent(uint32_t evt) {
	/* variables */

	char[] hello_world = "Hello, World!";
	Display(0, 0, &hello_world, 16);

	return;
}