/****************************************************************************/
/*                                                                          */
/*                             cc26x2r_rtos_intf.h                          */
/*                               Init Functions                             */
/*                                                                          */
/****************************************************************************/

/* CC26x2R Init function declarations. To be included and called in C files.

   Revision History:
       3/6/24  Adam Krivka      initial revision
*/

#ifndef CC26X2R_RTOS_INTF_H
    #define CC26X2R_RTOS_INTF_H

void PeriphPowerInit();              // turn on peripheral power domain
void GPIOClockInit();                // turn on GPIO clock
void GPTClockInit();                 // turn on GPT clock
void SSIClockInit();                 // turn on GPT clock

#endif
