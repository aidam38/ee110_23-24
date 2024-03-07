/****************************************************************************/
/*                                                                          */
/*                                  hw.h                                    */
/*                      Hardware configuration symbols                      */
/*                                                                          */
/****************************************************************************/

/* Duplicate of hw.inc for symbols that need to be used in C files. Needs to 
    be kept in sync with hw.inc because it's not possible to include .inc files 
    in C. */


#define KEYPAD_TIMER_EXCEPTION_NUMBER 31 // GPT0A_EXCEPTION_NUMBER
#define KEYPAD_TIMER_IRQ_NUMBER 15 // GPT0A_EXCEPTION_NUMBER
