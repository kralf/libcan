#include <sys/types.h>
#include <sys/dir.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/file.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>

#undef ASL_DEBUG

#include <pdebug.h>

#include "can_cpc.h"

/* Different macros for watching on read, write or both on a CAN channel. */
/* Also stdin is watched for a user input.                                */

#define SELECT    FD_ZERO(&readfds); \
                  FD_SET(0, &readfds); \
                  FD_SET(cpcfd, &readfds); \
                  tv.tv_sec = 0; \
                  tv.tv_usec = 500000; \
                  nfds = select(cpcfd+1, &readfds, NULL, NULL, &tv);

#define SELECT_WR FD_ZERO(&writefds); \
                  FD_SET(0, &readfds); \
                  FD_SET(cpcfd, &writefds); \
                  tv.tv_sec = 0; \
                  tv.tv_usec = 500000; \
                  nfds = select(cpcfd+1, NULL, &writefds, NULL, &tv);

static can_message_t message;

unsigned char btr0, btr1;
int handle;
char interface[32];
int zchn, nfds, cpcfd;
CPC_INIT_PARAMS_T *CPCInitParamsPtr;
fd_set readfds, writefds;
struct timeval tv;

void cpc_read_message_handler(int handle, const CPC_MSG_T *cpcmsg);

/* Init CAN Hardware */
void can_init(const char* dev) {
  bzero(&message, sizeof(can_message_t));

  /* ... add your code here ... */
  btr0=0x00;
  btr1=0x14;

  /* Using Interface cpc_usb0 */
  strcpy(interface, dev);
  // strcpy(interface, "/dev/cpc_usb0");

  /*Open the CAN*/
  if ((handle = CPC_OpenChannel(interface)) < 0) {
      fprintf(stderr, "ERROR: %s\n", CPC_DecodeErrorMsg(handle));
      exit(1);
  }
  PDEBUG("%s is CAN interface -> handle %d\n", interface, handle);

  /* ############################# Init Parameters ######################*/

  /*Define Handlers*/
  CPC_AddHandler(handle, cpc_read_message_handler);
  /*CPC_AddHandler(handle, get_steering_msg_handler);
  CPC_AddHandler(handle, get_yaw_msg_handler);*/


  /* This sets up the parameters used to initialize the CAN controller */
  PDEBUG("Initializing CAN-Controller ... ");

  CPCInitParamsPtr = CPC_GetInitParamsPtr(handle);
  CPCInitParamsPtr->canparams.cc_type                      = SJA1000;
  CPCInitParamsPtr->canparams.cc_params.sja1000.btr0       = btr0;
  CPCInitParamsPtr->canparams.cc_params.sja1000.btr1       = btr1;
  CPCInitParamsPtr->canparams.cc_params.sja1000.outp_contr = 0xda;
  CPCInitParamsPtr->canparams.cc_params.sja1000.acc_code0  = 0xff;
  CPCInitParamsPtr->canparams.cc_params.sja1000.acc_code1  = 0xff;
  CPCInitParamsPtr->canparams.cc_params.sja1000.acc_code2  = 0xff;
  CPCInitParamsPtr->canparams.cc_params.sja1000.acc_code3  = 0xff;
  CPCInitParamsPtr->canparams.cc_params.sja1000.acc_mask0  = 0xff;
  CPCInitParamsPtr->canparams.cc_params.sja1000.acc_mask1  = 0xff;
  CPCInitParamsPtr->canparams.cc_params.sja1000.acc_mask2  = 0xff;
  CPCInitParamsPtr->canparams.cc_params.sja1000.acc_mask3  = 0xff;
  CPCInitParamsPtr->canparams.cc_params.sja1000.mode       = 0;

  CPC_CANInit(handle, 0);
  PDEBUG("Done!\n\n");

  cpcfd = CPC_GetFdByHandle(handle);

  PDEBUG("Switch ON transimssion of CAN messages from CPC to PC\n");

  /* switch on transmission of CAN messages from CPC to PC */
  CPC_Control(handle, CONTR_CAN_Message | CONTR_CONT_ON);
  PDEBUG("Initialization finished...\n");
}

void can_close() {
  // Nothing to do here
}

void can_send_message(can_message_t* message) {
  static CPC_CAN_MSG_T cmsg = {0x00L, 0, {0, 0, 0, 0, 0, 0, 0, 0}};
  int i, retval = 0;

  cmsg.id=message->id;                          //put the can id of the device in cmsg
  cmsg.length=8;                                //put the lenght of the message in cmsg
  for(i = 0; i < cmsg.length; i++) {
    cmsg.msg[i] = message->content[i];          //put the message in cmsg
  }

  SELECT_WR                                     //enable write en read
  if (FD_ISSET(cpcfd, &writefds)) {             //if chanel open and writable (???)
    //send the message of lenght 8 to the device of id can_id
    while ((retval = CPC_SendMsg(handle, 0, &cmsg)) == CPC_ERR_CAN_NO_TRANSMIT_BUF)
      usleep(10);

    if (retval == 0) {
      //wait for the reply
      PDEBUG("Sent CAN message, now waiting for reply...\n");
      can_read_message();
      PDEBUG("Received CAN reply\n");
    }
    else
      PDEBUG_ERR("%s\n", CPC_DecodeErrorMsg(retval));
  }
}

void can_read_message() {
  /* task for reading can */
  SELECT                                        //enable read (???)

  if (nfds > 0) {
  if (FD_ISSET(cpcfd, &readfds)) {              //check, if messages have been received
      do{;}                                     //wait
      while(CPC_Handle(handle));                //until OK is send
    }
  }
  else if(nfds == -1)                           //if error
    perror(interface);                          //print error
}

void cpc_read_message_handler(int handle, const CPC_MSG_T *cpcmsg)
{
  int i;

  message.id = cpcmsg->msg.canmsg.id;
  for(i = 0; i < cpcmsg->msg.canmsg.length; i++) {
    message.content[i] = cpcmsg->msg.canmsg.msg[i];
  }

  can_read_message_handler(&message);
}
