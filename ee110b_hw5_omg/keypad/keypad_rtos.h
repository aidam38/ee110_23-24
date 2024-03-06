#include "../hw.h"

/* assembly imports */
void KeypadInit();
void KeypadScanAndDebounce();


/* interrupt constants */
#define HWI_PRIORITY 1
#define SWI_PRIORITY 1

/* hardware redefines */
#define TIMER_EXCEPTION_NUMBER KEYPAD_TIMER_EXCEPTION_NUMBER
