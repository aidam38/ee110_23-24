#include "../hw.h"

/* assembly imports */
void KeypadInit();
void KeypadScanAndDebounce();


/* interrupt constants */
#define HWI_PRIORITY 0
#define SWI_PRIORITY 2

/* hardware redefines */
#define TIMER_EXCEPTION_NUMBER KEYPAD_TIMER_EXCEPTION_NUMBER
#define TIMER_IRQ_NUMBER KEYPAD_TIMER_IRQ_NUMBER
