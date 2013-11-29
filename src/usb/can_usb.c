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

#include <ftdi/ftdi.h>

#include "can_usb.h"

const char* can_usb_errors[] = {
  "Success",
  "No such CAN-USB device",
  "CAN-USB conversion error",
  "Failed to send to CAN-USB device",
  "Failed to receive from CAN-USB device",
  "CAN-USB checksum error",
};

const char* can_device_name = "CAN-USB";

config_param_t can_usb_default_params[] = {
  {CAN_USB_PARAMETER_DEVICE,
    config_param_type_string,
    "/dev/bus/usb/001/001",
    "",
    "Path to the special file of the connected CAN-USB FTDI device"},
  {CAN_USB_PARAMETER_INTERFACE,
    config_param_type_enum,
    "any",
    "any|a|b|c|d",
    "The requested FTDI serial interface of the CAN-USB device"},
  {CAN_USB_PARAMETER_BAUD_RATE,
    config_param_type_int,
    "1000000",
    "[183, 3000000]",
    "The requested serial baud rate of the CAN-USB device in [baud]"},
  {CAN_USB_PARAMETER_DATA_BITS,
    config_param_type_int,
    "8",
    "[7, 8]",
    "The requested number of serial data bits of the CAN-USB device"},
  {CAN_USB_PARAMETER_STOP_BITS,
    config_param_type_int,
    "1",
    "[1, 15]",
    "The requested number of serial stop bits of the CAN-USB device"},
  {CAN_USB_PARAMETER_PARITY,
    config_param_type_enum,
    "none",
    "none|odd|even|mark|space",
    "The requested serial parity setting of the CAN-USB device"},
  {CAN_USB_PARAMETER_FLOW_CTRL,
    config_param_type_enum,
    "off",
    "off|xon_xoff|rts_cts|dtr_dsr",
    "The requested serial flow control setting of the CAN-USB device"},
  {CAN_USB_PARAMETER_BREAK,
    config_param_type_enum,
    "off",
    "off|on",
    "The requested serial break setting of the CAN-USB device"},
  {CAN_USB_PARAMETER_TIMEOUT,
    config_param_type_float,
    "0.1",
    "[0.0, inf)",
    "The CAN-USB serial communication timeout in [s]"},
  {CAN_USB_PARAMETER_LATENCY,
    config_param_type_float,
    "0.001",
    "[0.001, 0.255]",
    "The CAN-USB serial communication latency in [s]"},
};

const config_default_t can_default_config = {
  can_usb_default_params,
  sizeof(can_usb_default_params)/sizeof(config_param_t),
};

int can_usb_device_init(can_usb_device_t* dev, const char* name);
void can_usb_device_destroy(can_usb_device_t* dev);

int can_device_open(can_device_t* dev) {
  error_clear(&dev->error);
    
  if (!dev->num_references) {
    dev->comm_dev = malloc(sizeof(can_usb_device_t));

    dev->num_sent = 0;
    dev->num_received = 0;

    can_usb_device_t* usb_dev = (can_usb_device_t*)dev->comm_dev;
    if (can_usb_device_init(dev->comm_dev,
        config_get_string(&dev->config, CAN_USB_PARAMETER_DEVICE))) {
      error_blame(&dev->error, &usb_dev->error, CAN_ERROR_OPEN);
    
      can_usb_device_destroy(dev->comm_dev);
      
      free(dev->comm_dev);
      dev->comm_dev = 0;
      
      return dev->error.code;
    }
    
    if (ftdi_device_open(usb_dev->ftdi_dev,
        config_get_int(&dev->config, CAN_USB_PARAMETER_INTERFACE)) ||
      ftdi_device_setup(usb_dev->ftdi_dev,
        config_get_int(&dev->config, CAN_USB_PARAMETER_BAUD_RATE),
        config_get_int(&dev->config, CAN_USB_PARAMETER_DATA_BITS),
        config_get_int(&dev->config, CAN_USB_PARAMETER_STOP_BITS),
        config_get_int(&dev->config, CAN_USB_PARAMETER_PARITY),
        config_get_int(&dev->config, CAN_USB_PARAMETER_FLOW_CTRL),
        config_get_int(&dev->config, CAN_USB_PARAMETER_BREAK),
        config_get_float(&dev->config, CAN_USB_PARAMETER_TIMEOUT),
        config_get_float(&dev->config, CAN_USB_PARAMETER_LATENCY))) {
      error_blame(&dev->error, &usb_dev->ftdi_dev->error, CAN_ERROR_OPEN);
      
      can_usb_device_destroy(dev->comm_dev);
      
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
      ftdi_device_t* ftdi_dev = ((can_usb_device_t*)dev->comm_dev)->ftdi_dev;
        
      if (!ftdi_device_close(ftdi_dev)) {
        can_usb_device_destroy(dev->comm_dev);
        
        free(dev->comm_dev);
        dev->comm_dev = 0;
      }
      else
        error_blame(&dev->error, &ftdi_dev->error, CAN_ERROR_CLOSE);
    }
  }
  else
    error_setf(&dev->error, CAN_ERROR_CLOSE, "Non-zero reference count");
  
  return dev->error.code;
}

int can_device_send_message(can_device_t* dev, const can_message_t* message) {
  unsigned char data[64];
  
  error_clear(&dev->error);

  int result;
  if (((result = can_usb_device_from_epos(dev->comm_dev,
        message, data)) < 0) ||
      (can_usb_device_send(dev->comm_dev, data, result) < 0))
    error_blame(&dev->error, &((can_usb_device_t*)dev->comm_dev)->error,
      CAN_ERROR_SEND);
  else
    ++dev->num_sent;

  return dev->error.code;
}

int can_device_receive_message(can_device_t* dev, can_message_t* message) {
  unsigned char data[64];

  error_clear(&dev->error);
  
  if ((can_usb_device_receive(dev->comm_dev, data) < 0) ||
      can_usb_device_to_epos(dev->comm_dev, data, message))
    error_blame(&dev->error, &((can_usb_device_t*)dev->comm_dev)->error,
      CAN_ERROR_RECEIVE);
  else
    ++dev->num_received;
  
  return dev->error.code;
}

int can_usb_device_from_epos(can_usb_device_t* dev, const can_message_t*
    message, unsigned char* data) {
  error_clear(&dev->error);
  
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

  error_setf(&dev->error, CAN_USB_ERROR_CONVERT,
    "Invalid SDO command: 0x%02x", message->content[0]);
  return -dev->error.code;
}

int can_usb_device_to_epos(can_usb_device_t* dev, unsigned char* data,
    can_message_t* message) {
  error_clear(&dev->error);
  
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
        error_setf(&dev->error, CAN_USB_ERROR_CONVERT,
          "Invalid SDO command: 0x%02x", message->content[0]);
        return dev->error.code;
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

  return dev->error.code;
}

int can_usb_device_send(can_usb_device_t* dev, unsigned char* data,
    size_t num) {
  unsigned char sync[] = {CAN_USB_SYNC_DLE, CAN_USB_SYNC_STX};
  unsigned char crc_value[2];
  int i;

  error_clear(&dev->error);
  
  can_usb_calc_crc(data, num, crc_value);
  data[num-2] = crc_value[0];
  data[num-1] = crc_value[1];

  can_usb_change_byte_order(data, num);

  if (ftdi_device_write(dev->ftdi_dev, sync, sizeof(sync)) < 0) {
    error_blame(&dev->error, &dev->ftdi_dev->error, CAN_USB_ERROR_SEND);
    return -dev->error.code;
  }
  
  for (i = 0; i < num; ++i) {
    if (ftdi_device_write(dev->ftdi_dev, &data[i], 1) < 1) {
      error_blame(&dev->error, &dev->ftdi_dev->error, CAN_USB_ERROR_SEND);
      return -dev->error.code;
    }
    if ((data[i] == CAN_USB_SYNC_DLE) &&
        (ftdi_device_write(dev->ftdi_dev, &data[i], 1) < 1)) {
      error_blame(&dev->error, &dev->ftdi_dev->error, CAN_USB_ERROR_SEND);
      return -dev->error.code;
    }
  }
  
  return num;
}

int can_usb_device_receive(can_usb_device_t* dev, unsigned char* data) {
  unsigned char sync[2], header[2], buffer, crc_value[2];
  int i, result = 0, num_exp = 0;

  error_clear(&dev->error);
  
  result = ftdi_device_read(dev->ftdi_dev, sync, sizeof(sync));
  if (result > 0) {
    if ((sync[0] != CAN_USB_SYNC_DLE) || (sync[1] != CAN_USB_SYNC_STX)) {
      error_setf(&dev->error, CAN_USB_ERROR_RECEIVE,
        "Unexpected response: 0x%02x 0x%02x", sync[0], sync[1]);
      return -dev->error.code;
    }
  }
  else {
    error_blame(&dev->error, &dev->ftdi_dev->error, CAN_USB_ERROR_RECEIVE);
    return -dev->error.code;
  }
  
  result = ftdi_device_read(dev->ftdi_dev, header, sizeof(header));
  if (result > 0) {
    data[0] = header[0];
    data[1] = header[1];
  }
  else {
    error_blame(&dev->error, &dev->ftdi_dev->error, CAN_USB_ERROR_RECEIVE);
    return -dev->error.code;
  }
  
  num_exp = (data[1]+1)*sizeof(unsigned short);
  for (i = 0; i < num_exp; ++i) {
    if (ftdi_device_read(dev->ftdi_dev, &buffer, 1) > 0)
      data[i+2] = buffer;
    else {
      error_blame(&dev->error, &dev->ftdi_dev->error, CAN_USB_ERROR_RECEIVE);
      return -dev->error.code;
    }
    
    if (buffer == CAN_USB_SYNC_DLE) {
      unsigned char sync_dle;
      if ((ftdi_device_read(dev->ftdi_dev, &sync_dle, 1) < 1) &&
          (sync_dle != CAN_USB_SYNC_DLE)) {
        error_setf(&dev->error, CAN_USB_ERROR_RECEIVE,
          "Unexpected response: 0x%02x 0x%02x", buffer, sync_dle);
        return -dev->error.code;
      }
    }
  }
  result = i+2;

  can_usb_change_byte_order(data, result);

  can_usb_calc_crc(data, result, crc_value);
  if ((crc_value[0] != 0x00) || (crc_value[1] != 0x00)) {
    error_set(&dev->error, CAN_USB_ERROR_CRC);
    return -dev->error.code;
  }

  can_usb_change_word_order(data, result);

  return result;
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

int can_usb_device_init(can_usb_device_t* dev, const char* name) {
  ftdi_context_init(ftdi_default_context);

  dev->ftdi_dev = ftdi_context_match_name(ftdi_default_context, name);
  error_init(&dev->error, can_usb_errors);
  
  if (!dev->ftdi_dev) {
    ftdi_context_release(ftdi_default_context);
    error_setf(&dev->error, CAN_USB_ERROR_DEVICE, name);
  }
  
  return dev->error.code;
}

void can_usb_device_destroy(can_usb_device_t* dev) {
  if (dev->ftdi_dev) {
    dev->ftdi_dev = 0;
    ftdi_context_release(ftdi_default_context);
  }
  
  error_destroy(&dev->error);
}
