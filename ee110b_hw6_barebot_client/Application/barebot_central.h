/****************************************************************************/
/*                                                                          */
/*                            barebot_central.h                             */
/*                             Barebot Central                              */
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



#ifndef  __BAREBOT_CENTRAL_H__
    #define  __BAREBOT_CENTRAL_H__



/* library include files */
#include  <stdint.h>
#include  <stdbool.h>

/* local include files */
#include "barebot_central_intf.h"




/* constants */
const uint8_t BAREBOT_SERVER_ADDR[6] = {0xcb, 0x45, 0x2c, 0x98, 0x97, 0x77};


/* task configuration */
#define  BC_TASK_PRIORITY          2

#ifndef BC_TASK_STACK_SIZE
    #define  BC_TASK_STACK_SIZE    1024
#endif


/* application events */
#define  BC_EVT_KEY_PRESSED         1
#define  BC_EVT_ADV_REPORT          4

/* only system events are the ICALL message and queue events */
#define  BC_ALL_EVENTS            ( ICALL_MSG_EVENT_ID  |  UTIL_QUEUE_EVENT_ID )

/* suggest maximum data length values */
#define  BC_SUGGESTED_PDU_SIZE      251
#define  BC_SUGGESTED_TX_TIME       2120




/* macros */

/* spin if the function is not successful (return is not SUCCESS) */
#define BC_VERIFY(expr)   \
    if ((expr) != SUCCESS)  \
        BarebotCentral_spin();




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

/* local functions - the central task */
static void      BarebotCentral_init(void);
static void      BarebotCentral_taskFxn(UArg, UArg);

/* local functions - message and event processing */
static uint8_t   BarebotCentral_processStackMsg(ICall_Hdr *);
static void      BarebotCentral_processGapMessage(gapEventHdr_t *);
static void      BarebotCentral_processGattMessage(gattMsgEvent_t *);
static void      BarebotCentral_processAppMsg(bpEvt_t *);

/* local functions - callbacks */
static void      BarebotCentral_scanCb(uint32_t, void *, uintptr_t);

/* local funtions - utility */
static bool BarebotCentral_findDeviceName(uint8_t *, uint16_t, char *, uint8_t);
static status_t  BarebotCentral_enqueueMsg(uint8_t, bpEvtData_t);
static void      BarebotCentral_spin(void);



#endif
