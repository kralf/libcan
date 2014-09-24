/***************************************************************************
 *   Copyright (C) 2008 by Ralf Kaestner                                   *
 *   ralf.kaestner@gmail.com                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#define _ISOC99_SOURCE

#include <sys/types.h>
#include <sys/dir.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/file.h>
#include <sys/time.h>
#include <signal.h>
#include <termios.h>
#include <math.h>

#include <libcpc/cpc.h>
#include <libcpc/cpclib.h>

#include <string/string.h>

#include <timer/timer.h>

#include "device.h"

const char* can_cpc_device_errors[] = {
  "Success",
  "Failed to open CAN-CPC device",
  "Failed to close CAN-CPC device",
  "Failed to set CAN-CPC device parameters",
  "CAN-CPC device timeout",
  "Failed to send to CAN-CPC device",
  "Failed to receive from CAN-CPC device",
};

const char* can_device_name = "CAN-CPC";

config_param_t can_cpc_device_default_parameters[] = {
  {CAN_CPC_DEVICE_PARAMETER_NAME,
    config_param_type_string,
    "/dev/cpc_usb0",
    "",
    "Path to the special file of the connected CAN-CPC device"},
  {CAN_CPC_DEVICE_PARAMETER_BIT_RATE,
    config_param_type_int,
    "1000",
    "[10, 1000]",
    "The requested bit rate of the CAN bus in [kbit/s]"},
  {CAN_CPC_DEVICE_PARAMETER_QUANTA_PER_BIT,
    config_param_type_int,
    "8",
    "[8, 16]",
    "The requested number of time quanta per bit of the CAN bus"},
  {CAN_CPC_DEVICE_PARAMETER_SAMPLING_POINT,
    config_param_type_float,
    "0.75",
    "[0.75, 0.875]",
    "The requested synchronization sample point of the CAN bus, "
    "expressed as a ratio of the second phase buffer segment's "
    "start time quantum and the number of time quanta per bit"},
  {CAN_CPC_DEVICE_PARAMETER_TIMEOUT,
    config_param_type_float,
    "0.01",
    "",
    "The CAN bus communication timeout in [s]"},
};

const config_default_t can_device_default_config = {
  can_cpc_device_default_parameters,
  sizeof(can_cpc_device_default_parameters)/sizeof(config_param_t),
};

void can_cpc_device_init(can_cpc_device_t* dev);
void can_cpc_device_destroy(can_cpc_device_t* dev);
void can_cpc_device_handle(int handle, const CPC_MSG_T* msg, void* custom);

int can_device_open(can_device_t* dev) {
  error_clear(&dev->error);
  
  if (!dev->num_references) {
    dev->comm_dev = malloc(sizeof(can_cpc_device_t));
    can_cpc_device_init(dev->comm_dev);

    dev->num_sent = 0;
    dev->num_received = 0;
    
    if (can_cpc_device_open(dev->comm_dev,
        config_get_string(&dev->config, CAN_CPC_DEVICE_PARAMETER_NAME)) ||
      can_cpc_device_setup(dev->comm_dev,
        config_get_int(&dev->config, CAN_CPC_DEVICE_PARAMETER_BIT_RATE),
        config_get_int(&dev->config, CAN_CPC_DEVICE_PARAMETER_QUANTA_PER_BIT),
        config_get_float(&dev->config, CAN_CPC_DEVICE_PARAMETER_SAMPLING_POINT),
        config_get_float(&dev->config, CAN_CPC_DEVICE_PARAMETER_TIMEOUT))) {
      error_blame(&dev->error, &((can_cpc_device_t*)dev->comm_dev)->error,
        CAN_DEVICE_ERROR_OPEN);
      
      can_cpc_device_destroy(dev->comm_dev);
    
      free(dev->comm_dev);
      dev->comm_dev = 0;
      
      return dev->error.code;
    }
  }
  ++dev->num_references;

  return dev->error.code;
}

int can_device_close(can_device_t* dev) {
  error_clear(&dev->error);
  
  if (dev->num_references) {
    --dev->num_references;

    if (!dev->num_references) {
      if (!can_cpc_device_close(dev->comm_dev)) {
        can_cpc_device_destroy(dev->comm_dev);
        
        free(dev->comm_dev);
        dev->comm_dev = 0;
      }
      else
        error_blame(&dev->error, &((can_cpc_device_t*)dev->comm_dev)->error,
          CAN_DEVICE_ERROR_CLOSE);
    }
  }
  else
    error_setf(&dev->error, CAN_DEVICE_ERROR_CLOSE,
      "Non-zero reference count");

  return dev->error.code;  
}

int can_device_send_message(can_device_t* dev, const can_message_t* message) {
  error_clear(&dev->error);
  
  if (dev->comm_dev) {
    if (can_cpc_device_send_message(dev->comm_dev, message))
      error_blame(&dev->error, &((can_cpc_device_t*)dev->comm_dev)->error,
        CAN_DEVICE_ERROR_SEND);
    else
      ++dev->num_sent;
  }
  else
    error_setf(&dev->error, CAN_DEVICE_ERROR_SEND,
      "Communication device unavailable");
  
  return dev->error.code;
}

int can_device_receive_message(can_device_t* dev, can_message_t* message) {
  error_clear(&dev->error);

  if (dev->comm_dev) {
    if (can_cpc_device_receive_message(dev->comm_dev, message))
      error_blame(&dev->error, &((can_cpc_device_t*)dev->comm_dev)->error,
        CAN_DEVICE_ERROR_RECEIVE);
    else
      ++dev->num_received;
  }
  else
    error_setf(&dev->error, CAN_DEVICE_ERROR_RECEIVE,
      "Communication device unavailable");

  return dev->error.code;  
}

void can_cpc_device_init(can_cpc_device_t* dev) {
  dev->handle = 0;
  dev->fd = 0;
  dev->name = 0;

  dev->bitrate = 0;
  dev->quanta_per_bit = 0;
  dev->sampling_point = 0.0;
  dev->timeout = 0.0;

  memset(&dev->msg_received, 0, sizeof(can_message_t));
  
  error_init(&dev->error, can_cpc_device_errors);
}

void can_cpc_device_destroy(can_cpc_device_t* dev) {
  string_destroy(&dev->name);
  error_destroy(&dev->error);
}

int can_cpc_device_open(can_cpc_device_t* dev, const char* name) {
  int result;
  
  error_clear(&dev->error);

  if ((result = CPC_OpenChannel((char*)name)) >= 0) {
    dev->handle = result;
    dev->fd = 0;
    string_copy(&dev->name, name);
    
    CPC_AddHandlerEx(dev->handle, can_cpc_device_handle, dev);
  }
  else
    error_setf(&dev->error, CAN_CPC_DEVICE_ERROR_OPEN,
      CPC_DecodeErrorMsg(result));

  return dev->error.code;
}

int can_cpc_device_close(can_cpc_device_t* dev) {
  int result;
  
  error_clear(&dev->error);
  
  if (!(result = CPC_CANExit(dev->handle, 0)) &&
    !(result = CPC_CloseChannel(dev->handle))) {
    dev->name[0] = 0;
    dev->fd = 0;
  }
  else
    error_setf(&dev->error, CAN_CPC_DEVICE_ERROR_CLOSE,
      CPC_DecodeErrorMsg(result));

  return dev->error.code;
}

int can_cpc_device_setup(can_cpc_device_t* dev, int bitrate, int
  quanta_per_bit, double sampling_point, double timeout) {
  int result;
  CPC_INIT_PARAMS_T* parameters;

  error_clear(&dev->error);

  double t = 1.0/(8*bitrate*1e3);
  int brp = round(4*t*CAN_CPC_DEVICE_CLOCK_FREQUENCY/quanta_per_bit);
  int tseg1 = round(quanta_per_bit*sampling_point);
  int tseg2 = quanta_per_bit-tseg1;
  
  parameters = CPC_GetInitParamsPtr(dev->handle);
  parameters->canparams.cc_type = SJA1000;
  parameters->canparams.cc_params.sja1000.btr0 =
    ((CAN_CPC_DEVICE_SYNC_JUMP_WIDTH-1) << 6)+(brp-1);
  parameters->canparams.cc_params.sja1000.btr1 =
    (CAN_CPC_DEVICE_TRIPLE_SAMPLING << 7)+((tseg2-1) << 4)+(tseg1-2);
  parameters->canparams.cc_params.sja1000.outp_contr = 0xda;
  
  parameters->canparams.cc_params.sja1000.acc_code1 = 0xff;
  parameters->canparams.cc_params.sja1000.acc_code2 = 0xff;
  parameters->canparams.cc_params.sja1000.acc_code3 = 0xff;
  parameters->canparams.cc_params.sja1000.acc_mask0 = 0xff;
  parameters->canparams.cc_params.sja1000.acc_mask1 = 0xff;
  parameters->canparams.cc_params.sja1000.acc_mask2 = 0xff;
  parameters->canparams.cc_params.sja1000.acc_mask3 = 0xff;
  parameters->canparams.cc_params.sja1000.mode = 0;
  
  if (!(result = CPC_CANInit(dev->handle, 0))) {
    dev->fd = CPC_GetFdByHandle(dev->handle);

    dev->bitrate = bitrate;
    dev->quanta_per_bit = quanta_per_bit;
    dev->sampling_point = sampling_point;
    dev->timeout = timeout;

    if ((result = CPC_Control(dev->handle, CONTR_CAN_Message | CONTR_CONT_ON)))
      error_setf(&dev->error, CAN_CPC_DEVICE_ERROR_SETUP,
        CPC_DecodeErrorMsg(result));
  }
  else
    error_setf(&dev->error, CAN_CPC_DEVICE_ERROR_SETUP,
      CPC_DecodeErrorMsg(result));
  
  return dev->error.code;
}

int can_cpc_device_send_message(can_cpc_device_t* dev, const can_message_t*
    message) {
  CPC_CAN_MSG_T msg = {0x00L, 0, {0, 0, 0, 0, 0, 0, 0, 0}};
  struct timeval time;
  fd_set set;
  int result;

  error_clear(&dev->error);

  msg.id = message->id;  
  msg.length = message->data_length;
  memcpy(msg.msg, message->data, message->data_length);

  time.tv_sec = 0;
  time.tv_usec = dev->timeout*1e6;

  FD_ZERO(&set);
  FD_SET(dev->fd, &set);

  result = select(dev->fd+1, NULL, &set, NULL, &time);
  if (result == 0) {
    error_set(&dev->error, CAN_CPC_DEVICE_ERROR_TIMEOUT);
    return dev->error.code;
  }

  if (message->rtr) {
    while ((result = CPC_SendRTR(dev->handle, 0, &msg)) ==
        CPC_ERR_CAN_NO_TRANSMIT_BUF)
      timer_sleep(1e-5);
  }
  else {
    while ((result = CPC_SendMsg(dev->handle, 0, &msg)) ==
        CPC_ERR_CAN_NO_TRANSMIT_BUF)
      timer_sleep(1e-5);
  }
  
  if (result)
    error_setf(&dev->error, CAN_CPC_DEVICE_ERROR_SEND,
      CPC_DecodeErrorMsg(result));

  return dev->error.code;
}

int can_cpc_device_receive_message(can_cpc_device_t* dev, can_message_t*
    message) {
  struct timeval time;
  fd_set set;
  int result;

  error_clear(&dev->error);
  
  time.tv_sec = 0;
  time.tv_usec = dev->timeout*1e6;

  FD_ZERO(&set);
  FD_SET(dev->fd, &set);

  result = select(dev->fd+1, &set, NULL, NULL, &time);
  if (result == 0) {
    error_set(&dev->error, CAN_CPC_DEVICE_ERROR_TIMEOUT);
    return dev->error.code;
  }

  while (CPC_Handle(dev->handle))
    timer_sleep(1e-5);
  *message = dev->msg_received;

  return dev->error.code;
}

void can_cpc_device_handle(int handle, const CPC_MSG_T* msg, void* custom) {
  can_cpc_device_t* dev = custom;

  dev->msg_received.id = msg->msg.canmsg.id;
  dev->msg_received.rtr = (msg->type == CPC_MSG_T_RTR);

  memcpy(dev->msg_received.data, msg->msg.canmsg.msg, msg->msg.canmsg.length);
  dev->msg_received.data_length = msg->msg.canmsg.length;
}
