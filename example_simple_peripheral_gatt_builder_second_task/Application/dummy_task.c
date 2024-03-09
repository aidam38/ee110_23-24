#include "dummy_task.h"
#include <ti/sysbios/knl/Task.h>


Task_Struct dmTask;


#if defined __TI_COMPILER_VERSION__
#pragma DATA_ALIGN(spTaskStack, 8)
#else
#pragma data_alignment=8
#endif
uint8_t dmTaskStack[DM_TASK_STACK_SIZE];


static void Dummy_taskFxn(UArg a0, UArg a1);

void Dummy_createTask(void)
{
  Task_Params taskParams;

  // Configure task
  Task_Params_init(&taskParams);
  taskParams.stack = dmTaskStack;
  taskParams.stackSize = DM_TASK_STACK_SIZE;
  taskParams.priority = DM_TASK_PRIORITY;

  Task_construct(&dmTask, Dummy_taskFxn, &taskParams, NULL);
}

static void Dummy_taskFxn(UArg a0, UArg a1) {
    while (TRUE) {
        Task_yield();
    }
}
