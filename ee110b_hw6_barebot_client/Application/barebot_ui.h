/****************************************************************************/
/*                                                                          */
/*                            barebot_central.h                             */
/*                             Barebot UI                              */
/*                               Bluetooth Demo                             */
/*                                Include File                              */
/*                                                                          */
/****************************************************************************/

/*
   This file contains the constants, structures, and function prototypes for
   the Bluetooth barebot central defined in barebot_central.c.  These
   definitions are for the barebot central code and should not be needed
   by other code in the project.


   Revision History:
      3/10/22  Glen George       initial revision
*/



#ifndef  __BAREBOT_UI_H__
    #define  __BAREBOT_UI_H__



/* library include files */
#include  <stdint.h>
#include  <stdbool.h>

/* local include files */
#include "barebot_ui_intf.h"



/* constants */

/* task configuration */
#define  BUI_TASK_PRIORITY          2

#ifndef BUI_TASK_STACK_SIZE
    #define  BUI_TASK_STACK_SIZE    1024
#endif


/* UI events */
#define  BUI_EVT_KEY_PRESSED            1
#define  BUI_EVT_CENTRAL_STATE_CHANGED  2
#define  BUI_EVT_SPEED_CHANGED          3
#define  BUI_EVT_TURN_CHANGED           4

/* UI states */
#define BUI_STATE_CONTROL           1
#define BUI_STATE_THOUGHTS          2

/* only system events are the ICALL message and queue events */
#define  BUI_ALL_EVENTS            ( ICALL_MSG_EVENT_ID  |  UTIL_QUEUE_EVENT_ID )


/* macros */

/* spin if the function is not successful (return is not SUCCESS) */
#define BUI_VERIFY(expr)   \
    if ((expr) != SUCCESS)  \
        BarebotUI_spin();




/* structures, unions, and typedefs */

/* union for storing the data for an application event message */
/*    union to avoid unnecessary dynamic memory allocation */
typedef  union  {     /* data associated with the message */
             uint8_t   byte;    /* data is a byte (1 octet) */
             uint16_t  hword;   /* data is a half word (2 octets)*/
             uint32_t  word;    /* data is a word (4 octets) */
             void     *pData;   /* pointer to longer types of data */
         }  buiEvtData_t;


/* application event message - the event type combined with the data */
typedef  struct  {
             uint8_t      event;        /* event type */
             buiEvtData_t  data;         /* event data */
         }  buiEvt_t;



/* function declarations */

/* local functions - the central task */
static void      BarebotUI_init(void);
static void      BarebotUI_taskFxn(UArg, UArg);

/* local functions - message and event processing */
static void      BarebotUI_processUIMsg(buiEvt_t *);
void             BarebotUI_handleKey(uint8_t row, uint8_t col);

/* local functions - callbacks */

/* local funtions - utility */

static status_t  BarebotUI_enqueueMsg(uint8_t, buiEvtData_t);
static void      BarebotUI_spin(void);



#endif
