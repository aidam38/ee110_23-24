/****************************************************************************/
/*                                                                          */
/*                                keypad_rtos.h                             */
/*                    Keypad RTOS header for wrapper code                   */
/*                                                                          */
/****************************************************************************/

/* 

   Revision History:
       3/6/24  Adam Krivka      initial revision
*/


/* assembly imports */
void KeypadInit();
void KeypadScanAndDebounce();
void KeypadRTOSHwiHandler();


/* hardware configuration include */
#include "../hw.h"


/* hardware configuration redefines */
#define TIMER_EXCEPTION_NUMBER KEYPAD_TIMER_EXCEPTION_NUMBER
#define TIMER_IRQ_NUMBER KEYPAD_TIMER_IRQ_NUMBER


/* interrupt constants */
#define HWI_PRIORITY 0
#define SWI_PRIORITY 2

