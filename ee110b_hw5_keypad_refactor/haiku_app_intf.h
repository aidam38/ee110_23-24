/****************************************************************************/
/*                                                                          */
/*                           haiku_app_intf.h                               */
/*                       Haiku Application Interface                        */
/*                                                                          */
/****************************************************************************/

/* Haiku Application Interface. Functions declared are:
        KeyPressed() - enqueue events to application event queue
        App_createTask() - create main app task

   Revision History:
       3/6/24  Adam Krivka      initial revision
*/

#ifndef  __HAIKU_APP_INTF_H__
    #define  __HAIKU_APP_INTF_H__

/* C includes */
#include <stdint.h>


void KeyPressed(uint32_t _evt); /* enqueue events to application event queue */

void App_createTask(); /* create main app task */

#endif