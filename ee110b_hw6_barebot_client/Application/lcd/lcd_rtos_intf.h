/****************************************************************************/
/*                                                                          */
/*                              lcd_rtos_intf.h                             */
/*                            LCD RTOS interface                            */
/*                                                                          */
/****************************************************************************/

/* 14-pin 16x4 Character LCD RTOS Interface. Functions declared are:
        LCDInit() - initialize the LCD using RTOS hardware and software 
                    interrupts
        Display() - display a string on the LCD at the specified row and 
                    column
        ClearDisplay() - clear the LCD display

   Revision History:
       3/6/24  Adam Krivka      initial revision
*/

void    LCDInit();
int     Display(UArg r, UArg c, char* str, UArg len);
void    ClearDisplay();
