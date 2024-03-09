/****************************************************************************/
/*                                                                          */
/*                            blinker_peripheral.h                          */
/*                             Blinker Peripheral                           */
/*                               Bluetooth Demo                             */
/*                                Include File                              */
/*                                                                          */
/****************************************************************************/

/*
   This file contains the constants, structures, and function prototypes for
   the Bluetooth blinker peripheral defined in blinker_peripheral.c.  These
   definitions are for the blinker peripheral code and should not be needed
   by other code in the project.


   Revision History:
      3/10/22  Glen George       initial revision
*/



#ifndef  __BLINKER_PERIPHERAL_H__
    #define  __BLINKER_PERIPHERAL_H__



/* library include files */
#include  <stdint.h>
#include  <stdbool.h>

/* local include files */
#include  "blinker_peripheral_intf.h"




/* constants */

/* task configuration */
#define  BP_TASK_PRIORITY          2

#ifndef BP_TASK_STACK_SIZE
    #define  BP_TASK_STACK_SIZE    1024
#endif


/* application events */
#define  BP_CHAR_CHANGE_EVT         1
#define  BP_ADV_EVT                 3

/* only system events are the ICALL message and queue events */
#define  BP_ALL_EVENTS            ( ICALL_MSG_EVENT_ID  |  UTIL_QUEUE_EVENT_ID )

/* suggest maximum data length values */
#define  BP_SUGGESTED_PDU_SIZE      251
#define  BP_SUGGESTED_TX_TIME       2120




/* macros */

/* spin if the function is not successful (return is not SUCCESS) */
#define BP_VERIFY(expr)   \
    if ((expr) != SUCCESS)  \
        BlinkerPeripheral_spin();




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
static void      BlinkerPeripheral_init(void);
static void      BlinkerPeripheral_taskFxn(UArg, UArg);

/* local functions - message and event processing */
static uint8_t   BlinkerPeripheral_processStackMsg(ICall_Hdr *);
static void      BlinkerPeripheral_processGapMessage(gapEventHdr_t *);
static void      BlinkerPeripheral_processAppMsg(bpEvt_t *);
static bool      BlinkerPeripheral_processAdvEvent(bpEvtData_t);
static bool      BlinkerPeripheral_processCharValueChangeEvt(bpEvtData_t);

/* local functions - callbacks */
static void      BlinkerPeripheral_advCallback(uint32_t, void *, uintptr_t);
static void      BlinkerPeripheral_charValueChangeCB(uint8_t);

/* local funtions - utility */
static status_t  BlinkerPeripheral_enqueueMsg(uint8_t, bpEvtData_t);
static void      BlinkerPeripheral_spin(void);



#endif
