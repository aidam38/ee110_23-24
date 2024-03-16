/****************************************************************************/
/*                                                                          */
/*                              lcd_util.h                                  */
/*                            LCD Utility functions                         */
/*                                Include File                              */
/*                                                                          */
/****************************************************************************/

/* This file contains declarations for utility functions defined in lcd_util.c.
    It also serves as the interface include file.

   Revision History:
       3/6/24  Adam Krivka      initial revision
*/

/* constants */
#define TEXT_BUF_SIZE   100

/* function declarations */
int Display_printf(UArg r, UArg c, UArg len, char *fmt, ...);
