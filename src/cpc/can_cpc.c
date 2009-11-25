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
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <math.h>

#include <libcpc/cpc.h>
#include <libcpc/cpclib.h>

#include "can_cpc.h"

const char* can_cpc_errors[] = {
  "success",
  "error opening CAN-CPC device",
  "error closing CAN-CPC device",
  "error setting CAN-CPC device parameters",
  "CAN-CPC device select timeout",
  "error sending to CAN-CPC device",
  "error receiving from CAN-CPC device",
};

param_t can_cpc_default_parameters[] = {
  {CAN_CPC_PARAMETER_DEVICE, "/dev/cpc_usb0"},
  {CAN_CPC_PARAMETER_BITRATE, "1000"},
  {CAN_CPC_PARAMETER_QUANTA_PER_BIT, "8"},
  {CAN_CPC_PARAMETER_SAMPLING_POINT, "0.75"},
  {CAN_CPC_PARAMETER_TIMEOUT, "0.01"},
};

config_t can_default_config = {
  can_cpc_default_parameters,
  sizeof(can_cpc_default_parameters)/sizeof(param_t),
};

void can_cpc_handle(int handle, const CPC_MSG_T* msg, void* custom);

int can_open(can_device_p dev) {
  if (!dev->comm_dev)
    dev->comm_dev = malloc(sizeof(can_cpc_device_t));

  if (!dev->num_references) {
    dev->num_sent = 0;
    dev->num_received = 0;

    if (can_cpc_open(dev->comm_dev,
        config_get_string(&dev->config, CAN_CPC_PARAMETER_DEVICE)) ||
      can_cpc_setup(dev->comm_dev,
        config_get_int(&dev->config, CAN_CPC_PARAMETER_BITRATE),
        config_get_int(&dev->config, CAN_CPC_PARAMETER_QUANTA_PER_BIT),
        config_get_float(&dev->config, CAN_CPC_PARAMETER_SAMPLING_POINT),
        config_get_float(&dev->config, CAN_CPC_PARAMETER_TIMEOUT))) {
      free(dev->comm_dev);
      dev->comm_dev = 0;

      return CAN_ERROR_OPEN;
    }
  }
  ++dev->num_references;

  return CAN_ERROR_NONE;
}

int can_close(can_device_p dev) {
  if (dev->num_references) {
    --dev->num_references;

    if (!dev->num_references) {
      if (can_cpc_close(dev->comm_dev))
        return CAN_ERROR_CLOSE;

      free(dev->comm_dev);
      dev->comm_dev = 0;
    }

    return CAN_ERROR_NONE;
  }
  else
    return CAN_ERROR_CLOSE;
}

int can_send_message(can_device_p dev, can_message_p message) {
  if (!can_cpc_send(dev->comm_dev, message)) {
    ++dev->num_sent;
    return CAN_ERROR_NONE;
  }
  else
    return CAN_ERROR_SEND;
}

int can_receive_message(can_device_p dev, can_message_p message) {
  if (!can_cpc_receive(dev->comm_dev, message)) {
    ++dev->num_received;
    return CAN_ERROR_NONE;
  }
  else
    return CAN_ERROR_RECEIVE;
}

int can_cpc_open(can_cpc_device_p dev, const char* name) {
  dev->handle = CPC_OpenChannel((char*)name);

  if (dev->handle >= 0) {
    dev->fd = -1;
    strcpy(dev->name, name);
    dev->num_sent = 0;
    dev->num_received = 0;

    CPC_AddHandlerEx(dev->handle, can_cpc_handle, dev);
  }
  else
    return CAN_CPC_ERROR_OPEN;

  return CAN_CPC_ERROR_NONE;
}

int can_cpc_close(can_cpc_device_p dev) {
  if (!CPC_CANExit(dev->handle, 0) &&
    !CPC_CloseChannel(dev->handle)) {
    dev->name[0] = 0;
    dev->fd = -1;
  }
  else
    return CAN_CPC_ERROR_CLOSE;

  return CAN_CPC_ERROR_NONE;
}

int can_cpc_setup(can_cpc_device_p dev, int bitrate, int quanta_per_bit,
  double sampling_point, double timeout) {
  CPC_INIT_PARAMS_T* parameters;

  double t = 1.0/(8*bitrate*1e3);
  int brp = round(4*t*CAN_CPC_CLOCK_FREQUENCY/quanta_per_bit);
  int tseg1 = round(quanta_per_bit*sampling_point);
  int tseg2 = quanta_per_bit-tseg1;

  parameters = CPC_GetInitParamsPtr(dev->handle);
  parameters->canparams.cc_type = SJA1000;
  parameters->canparams.cc_params.sja1000.btr0 =
    ((CAN_CPC_SYNC_JUMP_WIDTH-1) << 6)+(brp-1);
  parameters->canparams.cc_params.sja1000.btr1 =
    (CAN_CPC_TRIPLE_SAMPLING << 7)+((tseg2-1) << 4)+(tseg1-2);
  parameters->canparams.cc_params.sja1000.outp_contr = 0xda;
  parameters->canparams.cc_params.sja1000.acc_code0 = 0xff;
  parameters->canparams.cc_params.sja1000.acc_code1 = 0xff;
  parameters->canparams.cc_params.sja1000.acc_code2 = 0xff;
  parameters->canparams.cc_params.sja1000.acc_code3 = 0xff;
  parameters->canparams.cc_params.sja1000.acc_mask0 = 0xff;
  parameters->canparams.cc_params.sja1000.acc_mask1 = 0xff;
  parameters->canparams.cc_params.sja1000.acc_mask2 = 0xff;
  parameters->canparams.cc_params.sja1000.acc_mask3 = 0xff;
  parameters->canparams.cc_params.sja1000.mode = 0;
  if (CPC_CANInit(dev->handle, 0))
    return CAN_CPC_ERROR_SETUP;

  dev->fd = CPC_GetFdByHandle(dev->handle);

  dev->bitrate = bitrate;
  dev->quanta_per_bit = quanta_per_bit;
  dev->sampling_point = sampling_point;
  dev->timeout = timeout;

  if (CPC_Control(dev->handle, CONTR_CAN_Message | CONTR_CONT_ON))
    return CAN_CPC_ERROR_SETUP;

  return CAN_CPC_ERROR_NONE;
}

int can_cpc_send(can_cpc_device_p dev, can_message_p message) {
  CPC_CAN_MSG_T msg = {0x00L, 0, {0, 0, 0, 0, 0, 0, 0, 0}};
  struct timeval time;
  fd_set set;
  int i, error;

  msg.id = message->id;
  msg.length = message->length;
  memcpy(msg.msg, message->content, message->length);

  time.tv_sec = 0;
  time.tv_usec = dev->timeout*1e6;

  FD_ZERO(&set);
  FD_SET(dev->fd, &set);

  error = select(dev->fd+1, NULL, &set, NULL, &time);
  if (error == 0)
    return CAN_CPC_ERROR_TIMEOUT;

  while ((error = CPC_SendMsg(dev->handle, 0, &msg)) ==
    CPC_ERR_CAN_NO_TRANSMIT_BUF)
    usleep(10);
  if (!error)
    ++dev->num_sent;
  else
    return CAN_CPC_ERROR_SEND;

  return CAN_CPC_ERROR_NONE;
}

int can_cpc_receive(can_cpc_device_p dev, can_message_p message) {
  struct timeval time;
  fd_set set;
  int error;

  time.tv_sec = 0;
  time.tv_usec = dev->timeout*1e6;

  FD_ZERO(&set);
  FD_SET(dev->fd, &set);

  error = select(dev->fd+1, &set, NULL, NULL, &time);
  if (error == 0)
    return CAN_CPC_ERROR_TIMEOUT;

  while (CPC_Handle(dev->handle))
    usleep(10);
  *message = dev->msg_received;

  return CAN_CPC_ERROR_NONE;
}

void can_cpc_handle(int handle, const CPC_MSG_T* msg, void* custom) {
  can_cpc_device_p dev = custom;

  dev->msg_received.id = msg->msg.canmsg.id;

  memcpy(dev->msg_received.content, msg->msg.canmsg.msg,
    msg->msg.canmsg.length);
  dev->msg_received.length = msg->msg.canmsg.length;

  ++dev->num_received;
}
