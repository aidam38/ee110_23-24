#ifndef  __APP_H__
    #define  __APP_H__

/* local include files */
#include  "haiku_app_intf.h"

#define APP_TASK_PRIORITY          	2
#define APP_TASK_STACK_SIZE 		1024

typedef struct event {
    Queue_Elem elem;
    uint32_t data;
} event_t;

void App_init(void);
static void App_run(UArg a0, UArg a1);
void App_processEvent(event_t* evt);

/* includes */
void Display(UArg r, UArg c, char* str, UArg len);

#endif
