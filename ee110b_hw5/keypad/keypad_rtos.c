#include "keypad_rtos.h"

#include  <ti/sysbios/hal/Hwi.h>
#include  <ti/sysbios/knl/Swi.h>

static Hwi_Struct   hwiTask;
Swi_Handle   swiTask;


void Display(UArg r, UArg c, char* str, UArg len);
void KeyPressed(uint32_t evt);
void TimerEventHandler_RTOSHwi();
void TimerEventHandler_RTOSSwi();

void KeypadHwiHandler() {
    /* update clear flag */
    Hwi_clearInterrupt(TIMER_EXCEPTION_NUMBER);

    /* call Swi */
    Swi_post(swiTask);

    return;
}

void KeypadSwiHandler() {
    KeypadScanAndDebounce();

    return;
}

void KeypadInit_RTOS() {
    /* variables */
    Hwi_Params          hwiParams;
    Swi_Params          swiParams;

    /* set up Swi */
    /* setup the parameters */
    Swi_Params_init(&swiParams);
    swiParams.priority = SWI_PRIORITY;

    /* now create Swi task */
    swiTask = Swi_create(KeypadSwiHandler, &swiParams, NULL);

    /* set up Hwi */
    /* setup the parameters */
    Hwi_Params_init(&hwiParams);
    hwiParams.eventId = TIMER_EXCEPTION_NUMBER;
    hwiParams.priority = HWI_PRIORITY;

    /* now create the Hwi task */
    Hwi_construct(&hwiTask, TIMER_EXCEPTION_NUMBER, KeypadHwiHandler, &hwiParams, NULL);

    /* call assembly init function (inits GPIOs and variables) */
    KeypadInit();

    return;
}