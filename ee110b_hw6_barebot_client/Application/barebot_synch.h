/****************************************************************************/
/*                                                                          */
/*                             barebot_synch.h                              */
/*                          Synchronization Objects                         */
/*                                Include File                              */
/*                                                                          */
/****************************************************************************/

/*
   This file provides constants that help the Central and Server identify
   and communicate with each other.  


   Revision History:
      3/15/24 Adam Krivka       initial revision
*/

#ifndef __BAREBOT_SYNCH_H__
#define __BAREBOT_SYNCH_H__

#include "icall.h"

#define INIT_ALL_EVENTS 0xffffffff

/* ui done initializing event */
static Event_Object uiInitDone;
Event_Handle uiInitDoneHandle;

#endif
