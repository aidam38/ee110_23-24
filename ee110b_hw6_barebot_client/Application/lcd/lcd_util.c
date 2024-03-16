/* includes */
#include  <xdc/runtime/System.h>

#include "lcd_util.h"
#include "lcd_rtos_intf.h"

/* shared variables */

char textBuf[TEXT_BUF_SIZE];

/* functions */

int Display_printf(UArg r, UArg c, UArg len, char *fmt, ...) {
    /* variables */
    va_list arg__va;

    /* print to textBuf */
    (void)va_start(arg__va, fmt);
    System_sprintf_va(textBuf, fmt, arg__va);
    va_end(arg__va);

    /* display to LCD and return that value */
    return Display(r, c, textBuf, len);
}
