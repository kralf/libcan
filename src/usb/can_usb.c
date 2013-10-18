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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <tulibs/ftdi.h>

#include "can_usb.h"

const char* can_usb_errors[] = {
  "success",
  "CAN USB conversion error",
  "CAN USB send failed",
  "CAN USB receive failed",
  "error reading from CAN USB device",
  "error writing to CAN USB device",
  "CAN USB device not responding",
  "unexpected response from CAN USB device",
  "CAN USB checksum error",
};

param_t can_usb_default_params[] = {
  {CAN_USB_PARAMETER_DEVICE, "/dev/bus/usb/001/001"},
  {CAN_USB_PARAMETER_INTERFACE, "0"},
  {CAN_USB_PARAMETER_BAUDRATE, "1000000"},
  {CAN_USB_PARAMETER_DATABITS, "8"},
  {CAN_USB_PARAMETER_STOPBITS, "1"},
  {CAN_USB_PARAMETER_PARITY, "0"},
  {CAN_USB_PARAMETER_FLOW_CTRL, "0"},
  {CAN_USB_PARAMETER_BREAK, "0"},
  {CAN_USB_PARAMETER_TIMEOUT, "0.1"},
  {CAN_USB_PARAMETER_LATENCY, "0.016"},
};

config_t can_default_config = {
  can_usb_default_params,
  sizeof(can_usb_default_params)/sizeof(param_t),
};

int can_open(can_device_p dev) {
  if (!dev->comm_dev) {
    ftdi_context_init(ftdi_default_context);
    dev->comm_dev = ftdi_match_name(ftdi_default_context,
      config_get_string(&dev->config, CAN_USB_PARAMETER_DEVICE));
    
    if (!dev->comm_dev) {
      ftdi_context_release(ftdi_default_context);
      return CAN_ERROR_OPEN;
    }
  }

  if (!dev->num_references) {
    dev->num_sent = 0;
    dev->num_received = 0;

    if (ftdi_open(dev->comm_dev,
        config_get_string(&dev->config, CAN_USB_PARAMETER_DEVICE),
        config_get_int(&dev->config, CAN_USB_PARAMETER_INTERFACE)) ||
      ftdi_setup(dev->comm_dev,
        config_get_int(&dev->config, CAN_USB_PARAMETER_BAUDRATE),
        config_get_int(&dev->config, CAN_USB_PARAMETER_DATABITS),
        config_get_int(&dev->config, CAN_USB_PARAMETER_STOPBITS),
        config_get_int(&dev->config, CAN_USB_PARAMETER_PARITY),
        config_get_int(&dev->config, CAN_USB_PARAMETER_FLOW_CTRL),
        config_get_int(&dev->config, CAN_USB_PARAMETER_BREAK),
        config_get_float(&dev->config, CAN_USB_PARAMETER_TIMEOUT),
        config_get_float(&dev->config, CAN_USB_PARAMETER_LATENCY))) {
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
      if (ftdi_close(dev->comm_dev))
        return CAN_ERROR_CLOSE;

      dev->comm_dev = 0;
      ftdi_context_release(ftdi_default_context);
    }

    return CAN_ERROR_NONE;
  }
  else
    return CAN_ERROR_CLOSE;
}

int can_send_message(can_device_p dev, can_message_p message) {
  unsigned char data[64];
  int num;

  num = can_usb_from_epos(dev, message, data);
  if ((num > 0) && (can_usb_send(dev, data, num) > 0)) {
    ++dev->num_sent;
    return CAN_ERROR_NONE;
  }
  else
    return CAN_ERROR_SEND;
}

int can_receive_message(can_device_p dev, can_message_p message) {
  unsigned char data[64];
  int num;

  num = can_usb_receive(dev, data);
  if ((num > 0) && !can_usb_to_epos(dev, data, message)) {
    ++dev->num_received;
    return CAN_ERROR_NONE;
  }
  else
    return CAN_ERROR_RECEIVE;
}

int can_usb_from_epos(can_device_p dev, can_message_p message,
    unsigned char* data) {
  switch (message->content[0]) {
    case CAN_CMD_SDO_WRITE_SEND_1_BYTE:
      data[0] = CAN_USB_OPCODE_WRITE;
      data[1] = 0x04;
      data[2] = message->content[2];
      data[3] = message->content[1];
      data[4] = message->id;
      data[5] = message->content[3];
      data[6] = message->content[5];
      data[7] = message->content[4];
      data[8] = 0x00;
      data[9] = 0x00;
      data[10] = 0x00;
      data[11] = 0x00;
      return 12;
    case CAN_CMD_SDO_WRITE_SEND_2_BYTE:
      data[0] = CAN_USB_OPCODE_WRITE;
      data[1] = 0x04;
      data[2] = message->content[2];
      data[3] = message->content[1];
      data[4] = message->id;
      data[5] = message->content[3];
      data[6] = message->content[5];
      data[7] = message->content[4];
      data[8] = 0x00;
      data[9] = 0x00;
      data[10] = 0x00;
      data[11] = 0x00;
      return 12;
    case CAN_CMD_SDO_WRITE_SEND_4_BYTE:
      data[0] = CAN_USB_OPCODE_WRITE;
      data[1] = 0x04;
      data[2] = message->content[2];
      data[3] = message->content[1];
      data[4] = message->id;
      data[5] = message->content[3];
      data[6] = message->content[5];
      data[7] = message->content[4];
      data[8] = message->content[7];
      data[9] = message->content[6];
      data[10] = 0x00;
      data[11] = 0x00;
      return 12;
    case CAN_CMD_SDO_READ_SEND:
      data[0] = CAN_USB_OPCODE_READ;
      data[1] = 0x02;
      data[2] = message->content[2];
      data[3] = message->content[1];
      data[4] = message->id;
      data[5] = message->content[3];
      data[6] = 0x00;
      data[7] = 0x00;
      return 8;
  }

  return -CAN_USB_ERROR_CONVERT;
}

int can_usb_to_epos(can_device_p dev, unsigned char* data, can_message_p
    message) {
  if ((data[2] == 0) && (data[3] == 0) && (data[4] == 0) && (data[5] == 0)) {
    switch (message->content[0]) {
      case CAN_CMD_SDO_WRITE_SEND_1_BYTE:
        message->content[0] = CAN_CMD_SDO_WRITE_RECEIVE;
        break;
      case CAN_CMD_SDO_WRITE_SEND_2_BYTE:
        message->content[0] = CAN_CMD_SDO_WRITE_RECEIVE;
        break;
      case CAN_CMD_SDO_WRITE_SEND_4_BYTE:
        message->content[0] = CAN_CMD_SDO_WRITE_RECEIVE;
        break;
      case CAN_CMD_SDO_READ_SEND:
        message->content[0] = CAN_CMD_SDO_READ_RECEIVE_UNDEFINED;
        break;
      default:
        return -CAN_USB_ERROR_CONVERT;
    }

    message->content[1] = message->content[2];
    message->content[2] = message->content[3];
    message->content[3] = message->content[4];
    message->content[7] = data[6];
    message->content[6] = data[7];
    message->content[5] = data[8];
    message->content[4] = data[9];
  }
  else {
    message->id -= CAN_COB_ID_SDO_SEND;
    message->id += CAN_COB_ID_SDO_RECEIVE;

    message->content[0] = CAN_CMD_SDO_ABORT;
    message->content[1] = message->content[2];
    message->content[2] = message->content[3];
    message->content[3] = message->content[4];
    message->content[7] = data[2];
    message->content[6] = data[3];
    message->content[5] = data[4];
    message->content[4] = data[5];
  }
  message->length = 8;

  return CAN_USB_ERROR_NONE;
}

int can_usb_send(can_device_p dev, unsigned char* data, size_t num) {
  unsigned char sync[] = {CAN_USB_SYNC_DLE, CAN_USB_SYNC_STX};
  unsigned char crc_value[2];
  int i;

  if (!dev->comm_dev)
    return -CAN_USB_ERROR_SEND;

  can_usb_calc_crc(data, num, crc_value);
  data[num-2] = crc_value[0];
  data[num-1] = crc_value[1];

  can_usb_change_byte_order(data, num);

  if (ftdi_write(dev->comm_dev, sync, sizeof(sync)) != sizeof(sync))
    return -CAN_USB_ERROR_WRITE;
  for (i = 0; i < num; ++i) {
    if (ftdi_write(dev->comm_dev, &data[i], 1) != 1)
      return -CAN_USB_ERROR_WRITE;
    if ((data[i] == CAN_USB_SYNC_DLE) &&
        (ftdi_write(dev->comm_dev, &data[i], 1) != 1))
      return -CAN_USB_ERROR_WRITE;
  }
  
  return num;
}

int can_usb_receive(can_device_p dev, unsigned char* data) {
  unsigned char sync[2], header[2], buffer, crc_value[2];
  int i, num_recv = 0, num_exp = 0;

  if (!dev->comm_dev)
    return -CAN_USB_ERROR_RECEIVE;

  num_recv = ftdi_read(dev->comm_dev, sync, sizeof(sync));
  if (num_recv == sizeof(sync)) {
    if ((sync[0] != CAN_USB_SYNC_DLE) || (sync[1] != CAN_USB_SYNC_STX))
      return -CAN_USB_ERROR_UNEXPECTED_RESPONSE;
  }
  else if (num_recv == 0)
    return -CAN_USB_ERROR_NO_RESPONSE;
  else
    return -CAN_USB_ERROR_READ;
  
  num_recv = ftdi_read(dev->comm_dev, header, sizeof(header));
  if (num_recv == sizeof(header)) {
    data[0] = header[0];
    data[1] = header[1];
  }
  else if (num_recv == 0)
    return -CAN_USB_ERROR_NO_RESPONSE;
  else
    return -CAN_USB_ERROR_READ;
  
  num_exp = (data[1]+1)*sizeof(unsigned short);
  for (i = 0; i < num_exp; ++i) {
    if (ftdi_read(dev->comm_dev, &buffer, 1) == 1)
      data[i+2] = buffer;
    else
      break;
    
    if ((buffer == CAN_USB_SYNC_DLE) &&
        (ftdi_read(dev->comm_dev, &buffer, 1) != 1) &&
        (buffer != CAN_USB_SYNC_DLE))
      return -CAN_USB_ERROR_UNEXPECTED_RESPONSE;
  }
  if (i == 0)
    return -CAN_USB_ERROR_NO_RESPONSE;
  else if (i < num_exp)
    return -CAN_USB_ERROR_UNEXPECTED_RESPONSE;
  num_recv = i+2;

  can_usb_change_byte_order(data, num_recv);

  can_usb_calc_crc(data, num_recv, crc_value);
  if ((crc_value[0] != 0x00) || (crc_value[1] != 0x00)) {
    return -CAN_USB_ERROR_CRC;
  }

  can_usb_change_word_order(data, num_recv);

  return num_recv;
}

size_t can_usb_change_byte_order(unsigned char* data, size_t num) {
  unsigned char tmp;
  int i;

  for (i = 2; i < num; i += 2) {
    tmp = data[i];

    data[i] = data[i+1];
    data[i+1] = tmp;
  }

  return i;
}

size_t can_usb_change_word_order(unsigned char* data, size_t num) {
  unsigned char tmp_lb, tmp_hb;
  int i;

  for (i = 2; i < (num-2); i += 4) {
    tmp_hb = data[i];
    tmp_lb = data[i+1];

    data[i] = data[i+2];
    data[i+1] = data[i+3];

    data[i+2] = tmp_hb;
    data[i+3] = tmp_lb;
  }

  return i;
}

size_t can_usb_calc_crc(unsigned char* data, size_t num, unsigned char*
    crc_value) {
  unsigned short* word_data = (unsigned short*)data;
  size_t num_words = num/2;
  unsigned short crc;

  crc = can_usb_crc_alg(word_data, num_words);
  crc_value[0] = (crc >> 8);
  crc_value[1] = crc;

  return num_words;
}

unsigned short can_usb_crc_alg(unsigned short* data, size_t num) {
  unsigned short shift, c, carry, crc = 0;
  int i;

  for (i = 0; i < num; ++i) {
    shift = 0x8000;
    if (i)
      c = (data[i] << 8) | (data[i] >> 8);
    else
      c = data[i];

    do {
      carry = crc & 0x8000;
      crc <<= 1;

      if (c & shift)
        ++crc;
      if (carry)
        crc ^= 0x1021;

      shift >>= 1;
    }
    while (shift);
  }

  return crc;
}
