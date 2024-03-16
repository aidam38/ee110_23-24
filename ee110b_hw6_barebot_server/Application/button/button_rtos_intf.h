/****************************************************************************/
/*                                                                          */
/*                             button_rtos_intf.h                           */
/*                           Button RTOS interface                          */
/*                                                                          */
/****************************************************************************/

/* 4x4 Button RTOS interface function declarations (only those necessary to
    set up and use the button code). Functions declared are:
        ButtonInit_RTOS() - initialize the button using RTOS hardware and 
                            software interrupts

   Revision History:
       3/6/24  Adam Krivka      initial revision
*/

#ifndef BUTTON_RTOS_INTF_H
    #define BUTTON_RTOS_INTF_H

void ButtonInit_RTOS();
void ButtonPressed(uint8_t buttonId);

#endif
