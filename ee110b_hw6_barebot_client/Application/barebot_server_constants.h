/****************************************************************************/
/*                                                                          */
/*                      barebot_server_constants.h                          */
/*                  Barebot Server Identifying Constants                    */
/*                                Include File                              */
/*                                                                          */
/****************************************************************************/

/*
   This file provides constants that help the Central and Server identify
   and communicate with each other.  


   Revision History:
      3/15/24 Adam Krivka       initial revision
*/

#ifndef  __BAREBOT_SERVER_CONSTANTS_H__
    #define   __BAREBOT_SERVER_CONSTANTS_H__


/* server local short name */
#define BAREBOT_SERVER_LOCAL_NAME "BP"

/* (below is copy-pasted from server GATT profile) */
// Profile Parameters
// Service UUID
#define BAREBOTPROFILE_SERV_UUID 0xFFF0
// Characteristic defines
#define BAREBOTPROFILE_THOUGHTS   0
#define BAREBOTPROFILE_THOUGHTS_UUID 0xFFF1
#define BAREBOTPROFILE_THOUGHTS_LEN  100
// Characteristic defines
#define BAREBOTPROFILE_SPEED   1
#define BAREBOTPROFILE_SPEED_UUID 0xFFF2
#define BAREBOTPROFILE_SPEED_LEN  2
// Characteristic defines
#define BAREBOTPROFILE_TURN   2
#define BAREBOTPROFILE_TURN_UUID 0xFFF3
#define BAREBOTPROFILE_TURN_LEN  2
// Characteristic defines
#define BAREBOTPROFILE_SPEEDUPDATE   3
#define BAREBOTPROFILE_SPEEDUPDATE_UUID 0xFFF4
#define BAREBOTPROFILE_SPEEDUPDATE_LEN  2
// Characteristic defines
#define BAREBOTPROFILE_TURNUPDATE   4
#define BAREBOTPROFILE_TURNUPDATE_UUID 0xFFF5
#define BAREBOTPROFILE_TURNUPDATE_LEN  2

#endif
