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
void ButtonInit();
void ButtonDebounce();
void ButtonRTOSHwiHandler();


/* hardware configuration include */
#include "../hw.h"


/* hardware configuration redefines */
#define TIMER_EXCEPTION_NUMBER  KEYPAD_TIMER_EXCEPTION_NUMBER
#define TIMER_IRQ_NUMBER        KEYPAD_TIMER_IRQ_NUMBER


/* interrupt constants */
#define HWI_PRIORITY 2
#define SWI_PRIORITY 2

