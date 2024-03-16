/****************************************************************************/
/*                                                                          */
/*                              lcd_util.c                                  */
/*                            LCD Utility functions                         */
/*                                                                          */
/****************************************************************************/

/* This file contains utility functions for the LCD. Functions included are:
        Display_printf - print a formatted string to the LCD

   Revision History:
       3/6/24  Adam Krivka      initial revision
*/

/* includes */
#include  <xdc/runtime/System.h>

/* local includes */
#include "lcd_util.h"
#include "lcd_rtos_intf.h"

/* shared/global variables */
    /* none */



/* functions */

int Display_printf(UArg r, UArg c, UArg len, char *fmt, ...) {
    /* variables */
    char textBuf[TEXT_BUF_SIZE]; /* text buffer */
    va_list arg__va;                /* variable argument list */

    /* print to textBuf (must use variadac args version here)*/
    (void)va_start(arg__va, fmt);
    System_sprintf_va(textBuf, fmt, arg__va);
    va_end(arg__va);

    /* display to LCD and return that value */
    return Display(r, c, textBuf, len);
}
