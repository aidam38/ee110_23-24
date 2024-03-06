#include "keypad_rtos.h"

#include  <ti/sysbios/hal/Hwi.h>
#include  <ti/sysbios/knl/Swi.h>

static Hwi_Struct   hwiTask;
static Swi_Struct   swiTask;

void KeypadHwiHandler() {
    /* call Swi */
    Swi_post(&swiTask);

    /* update clear flag */
    Hwi_clearInterrupt(TIMER_EXCEPTION_NUMBER);

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

    /* call assembly init function (inits GPIOs and variables) */
    KeypadInit();

    /* set up Hwi */
    /* setup the parameters */
    Hwi_Params_init(&hwiParams);
    hwiParams.eventId = 13;
    hwiParams.priority = HWI_PRIORITY;

    /* now create the Hwi task */
    Hwi_construct(&hwiTask, TIMER_EXCEPTION_NUMBER, KeypadHwiHandler, &hwiParams, NULL);

    /* set up Swi */
    /* setup the parameters */
    Swi_Params_init(&swiParams);
    swiParams.priority = SWI_PRIORITY;

    /* now create Swi task */
    Swi_construct(&swiTask, KeypadSwiHandler, &swiParams, NULL);


    return;
}
