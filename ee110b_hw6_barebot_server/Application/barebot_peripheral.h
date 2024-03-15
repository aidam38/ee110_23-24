/****************************************************************************/
/*                                                                          */
/*                            barebot_peripheral.h                          */
/*                             Barebot Peripheral                           */
/*                               Bluetooth Demo                             */
/*                                Include File                              */
/*                                                                          */
/****************************************************************************/

/*
   This file contains the constants, structures, and function prototypes for
   the Bluetooth barebot peripheral defined in barebot_peripheral.c.  These
   definitions are for the barebot peripheral code and should not be needed
   by other code in the project.


   Revision History:
      3/10/22  Glen George       initial revision
*/



#ifndef  __BAREBOT_PERIPHERAL_H__
    #define  __BAREBOT_PERIPHERAL_H__



/* library include files */
#include  <stdint.h>
#include  <stdbool.h>

/* local include files */
#include "barebot_peripheral_intf.h"




/* constants */

/* task configuration */
#define  BS_TASK_PRIORITY          2

#ifndef BS_TASK_STACK_SIZE
    #define  BS_TASK_STACK_SIZE    1024
#endif


/* application events */
#define  BS_BUTTON_PRESSED          1
#define  BS_CHAR_CHANGE_EVT         2
#define  BS_ADV_EVT                 3

/* only system events are the ICALL message and queue events */
#define  BS_ALL_EVENTS            ( ICALL_MSG_EVENT_ID  |  UTIL_QUEUE_EVENT_ID )

/* suggest maximum data length values */
#define  BS_SUGGESTED_PDU_SIZE      251
#define  BS_SUGGESTED_TX_TIME       2120


/* error messages */
const char error1[] = "Button error";


/* macros */

/* spin if the function is not successful (return is not SUCCESS) */
#define BS_VERIFY(expr)   \
    if ((expr) != SUCCESS)  \
        BarebotPeripheral_spin();




/* structures, unions, and typedefs */

/* union for storing the data for an application event message */
/*    union to avoid unnecessary dynamic memory allocation */
typedef  union  {     /* data associated with the message */
             uint8_t   byte;    /* data is a byte (1 octet) */
             uint8_t   hword;   /* data is a half word (2 octets)*/
             uint8_t   word;    /* data is a word (4 octets) */
             void     *pData;   /* pointer to longer types of data */
         }  bpEvtData_t;


/* application event message - the event type combined with the data */
typedef  struct  {
             uint8_t      event;        /* event type */
             bpEvtData_t  data;         /* event data */
         }  bpEvt_t;


/* messages from advertising events - type and data from callback */
typedef  struct  {
             uint32_t   event;          /* event type */
             void      *pBuf;           /* event data */
         }  bpGapAdvEventData_t;




/* function declarations */

/* local functions - the peripheral task */
static void      BarebotPeripheral_init(void);
static void      BarebotPeripheral_taskFxn(UArg, UArg);

/* local functions - message and event processing */
static uint8_t   BarebotPeripheral_processStackMsg(ICall_Hdr *);
static void      BarebotPeripheral_processGapMessage(gapEventHdr_t *);
static void      BarebotPeripheral_processAppMsg(bpEvt_t *);
static void      BarebotPeripheral_handleButton(uint8_t buttonId);
static bool      BarebotPeripheral_processAdvEvent(bpEvtData_t);
static bool      BarebotPeripheral_processCharValueChangeEvt(bpEvtData_t);

/* local functions - callbacks */
static void      BarebotPeripheral_advCallback(uint32_t, void *, uintptr_t);
static void      BarebotPeripheral_charValueChangeCB(uint8_t);

/* local funtions - utility */
static status_t  BarebotPeripheral_enqueueMsg(uint8_t, bpEvtData_t);
static void      BarebotPeripheral_spin(void);



#endif
