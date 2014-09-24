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

#include "device.h"

#include "reorder.h"
#include "crc.h"

const char* can_serial_errors[] = {
  "Success",
  "CAN-Serial conversion error",
  "Failed to send to CAN-Serial device",
  "Failed to receive from CAN-Serial device",
  "CAN-Serial checksum error",
};

const char* can_device_name = "CAN-Serial";

config_param_t can_serial_device_default_params[] = {
  {CAN_SERIAL_DEVICE_PARAMETER_NAME,
    config_param_type_string,
    "/dev/ttyS0",
    "",
    "Path to the special file of the connected CAN-Serial device"},
  {CAN_SERIAL_DEVICE_PARAMETER_BAUD_RATE,
    config_param_type_int,
    "38400",
    "[50, 230400]",
    "The requested baud rate of the CAN-Serial device in [baud]"},
  {CAN_SERIAL_DEVICE_PARAMETER_DATA_BITS,
    config_param_type_int,
    "8",
    "[5, 8]",
    "The requested number of data bits of the CAN-Serial device"},
  {CAN_SERIAL_DEVICE_PARAMETER_STOP_BITS,
    config_param_type_int,
    "1",
    "[1, 2]",
    "The requested number of stop bits of the CAN-Serial device"},
  {CAN_SERIAL_DEVICE_PARAMETER_PARITY,
    config_param_type_enum,
    "none",
    "none|odd|even",
    "The requested parity setting of the CAN-Serial device"},
  {CAN_SERIAL_DEVICE_PARAMETER_FLOW_CTRL,
    config_param_type_enum,
    "off",
    "off|xon_xoff|rts_cts",
    "The requested flow control setting of the CAN-Serial device"},
  {CAN_SERIAL_DEVICE_PARAMETER_TIMEOUT,
    config_param_type_float,
    "0.01",
    "",
    "The CAN-Serial communication timeout in [s]"},
};

const config_default_t can_device_default_config = {
  can_serial_device_default_params,
  sizeof(can_serial_device_default_params)/sizeof(config_param_t),
};

void can_serial_device_init(can_serial_device_t* dev, const char* name);
void can_serial_device_destroy(can_serial_device_t* dev);

int can_device_open(can_device_t* dev) {
  error_clear(&dev->error);
  
  if (!dev->num_references) {
    dev->comm_dev = malloc(sizeof(can_serial_device_t));
    can_serial_device_init(dev->comm_dev,
      config_get_string(&dev->config, CAN_SERIAL_DEVICE_PARAMETER_NAME));
    
    dev->num_sent = 0;
    dev->num_received = 0;

    serial_device_t* serial_dev = 
      &((can_serial_device_t*)dev->comm_dev)->serial_dev;
    if (serial_device_open(serial_dev) ||
      serial_device_setup(serial_dev,
        config_get_int(&dev->config, CAN_SERIAL_DEVICE_PARAMETER_BAUD_RATE),
        config_get_int(&dev->config, CAN_SERIAL_DEVICE_PARAMETER_DATA_BITS),
        config_get_int(&dev->config, CAN_SERIAL_DEVICE_PARAMETER_STOP_BITS),
        config_get_int(&dev->config, CAN_SERIAL_DEVICE_PARAMETER_PARITY),
        config_get_int(&dev->config, CAN_SERIAL_DEVICE_PARAMETER_FLOW_CTRL),
        config_get_float(&dev->config, CAN_SERIAL_DEVICE_PARAMETER_TIMEOUT))) {
      error_blame(&dev->error, &serial_dev->error, CAN_DEVICE_ERROR_OPEN);

      can_serial_device_destroy(dev->comm_dev);
    
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
      serial_device_t* serial_dev = 
        &((can_serial_device_t*)dev->comm_dev)->serial_dev;
        
      if (!serial_device_close(serial_dev)) {
        can_serial_device_destroy(dev->comm_dev);
        
        free(dev->comm_dev);
        dev->comm_dev = 0;
      }
      else
        error_blame(&dev->error, &serial_dev->error, CAN_DEVICE_ERROR_CLOSE);
    }
  }
  else
    error_setf(&dev->error, CAN_DEVICE_ERROR_CLOSE,
      "Non-zero reference count");
  
  return dev->error.code;
}

int can_device_send_message(can_device_t* dev, const can_message_t* message) {
  unsigned char data[64];
  
  error_clear(&dev->error);

  int result;
  if (((result = can_serial_device_from_message(dev->comm_dev,
        &dev->connection_set, message, data)) < 0) ||
      (can_serial_device_send(dev->comm_dev, data, result) < 0))
    error_blame(&dev->error, &((can_serial_device_t*)dev->comm_dev)->error,
      CAN_DEVICE_ERROR_SEND);
  else
    ++dev->num_sent;

  return dev->error.code;
}

int can_device_receive_message(can_device_t* dev, can_message_t* message) {
  unsigned char data[64];

  error_clear(&dev->error);
  
  if ((can_serial_device_receive(dev->comm_dev, data) < 0) ||
    can_serial_device_to_message(dev->comm_dev, &dev->connection_set,
      data, message))
    error_blame(&dev->error, &((can_serial_device_t*)dev->comm_dev)->error,
      CAN_DEVICE_ERROR_RECEIVE);
  else
    ++dev->num_received;
  
  return dev->error.code;
}

int can_serial_device_from_message(can_serial_device_t* dev, const
    can_connection_set_t* connection_set, const can_message_t* message,
    unsigned char* data) {
  error_clear(&dev->error);
  
  ssize_t result = can_connection_set_find_cob_id(connection_set,
    message->id);
  
  if (result >= 0) {
    if (message->protocol == can_message_protocol_sdo) {
      can_sdo_css_t css = can_sdo_get_message_css(message);
      
      switch (css) {
        case CAN_CMD_SDO_WRITE_SEND_1_BYTE:
          data[0] = CAN_SERIAL_DEVICE_OPCODE_WRITE;
          data[1] = 0x02;
          data[2] = message->content[2];
          data[3] = message->content[1];
          data[4] = message->id;
          data[5] = message->content[3];
          data[6] = message->content[5];
          data[7] = message->content[4];
          data[8] = 0x00;
          data[9] = 0x00;
          return 10;
        case CAN_CMD_SDO_WRITE_SEND_2_BYTE:
          data[0] = CAN_SERIAL_DEVICE_OPCODE_WRITE;
          data[1] = 0x02;
          data[2] = message->content[2];
          data[3] = message->content[1];
          data[4] = message->id;
          data[5] = message->content[3];
          data[6] = message->content[5];
          data[7] = message->content[4];
          data[8] = 0x00;
          data[9] = 0x00;
          return 10;
        case CAN_CMD_SDO_WRITE_SEND_4_BYTE:
          data[0] = CAN_SERIAL_DEVICE_OPCODE_WRITE;
          data[1] = 0x03;
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
        case CAN_CMD_SDO_WRITE_SEND_N_BYTE_INIT:
          data[0] = CAN_SERIAL_DEVICE_OPCODE_WRITE_SEG_INIT;
          data[1] = 0x03;
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
        case CAN_CMD_SDO_WRITE_SEND_N_BYTE_SEGMENT:
          data[0] = CAN_SERIAL_DEVICE_OPCODE_WRITE_SEG;
          data[1] = message->length;
          data[2] = (message->length > 1) ? message->content[2] : 0x00;
          data[3] = (message->length > 0) ? message->content[1] : 0x00;
          data[7] = (message->length > 3) ? message->content[4] : 0x00;
          data[5] = (message->length > 2) ? message->content[3] : 0x00;
          data[6] = (message->length > 5) ? message->content[6] : 0x00;
          data[7] = (message->length > 4) ? message->content[5] : 0x00;
          data[8] = (message->length > 7) ? message->content[8] : 0x00;
          data[9] = (message->length > 6) ? message->content[7] : 0x00;
          data[10] = 0x00;
          data[11] = 0x00;
          return 10;
        case CAN_CMD_SDO_READ_SEND:
          data[0] = CAN_SERIAL_DEVICE_OPCODE_READ;
          data[1] = 0x01;
          data[2] = message->content[2];
          data[3] = message->content[1];
          data[4] = message->id;
          data[5] = message->content[3];
          data[6] = 0x00;
          data[7] = 0x00;
          return 8;
      }
      
      error_setf(&dev->error, CAN_SERIAL_DEVICE_ERROR_CONVERT,
        "Invalid SDO CCS: 0x%02x", message->content[0]);
    }
    else
      error_setf(&dev->error, CAN_SERIAL_DEVICE_ERROR_CONVERT,
        "Invalid SDO command: 0x%02x", message->content[0]);
  }
  else {
    data[0] = CAN_SERIAL_DEVICE_OPCODE_SEND_CAN_FRAME;
    data[1] = 5;
    data[2] = (message->id >> 8);
    data[3] = message->id;
    data[4] = (message->data_length >> 8);
    data[5] = message->data_length;
    memcpy(&data[6], message->data, sizeof(message->data));
    return 6+sizeof(message->data);
  }
    
  return -dev->error.code;
}

int can_serial_device_to_message(can_serial_device_t* dev, const
    can_connection_set_t* connection_set, unsigned char* data,
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
      case CAN_CMD_SDO_WRITE_SEND_N_BYTE_INIT:
        message->content[0] = CAN_CMD_SDO_WRITE_RECEIVE;
        break;
      case CAN_CMD_SDO_WRITE_SEND_N_BYTE_SEGMENT:
        message->content[0] = CAN_CMD_SDO_WRITE_RECEIVE;
        break;
      case CAN_CMD_SDO_READ_SEND:
        message->content[0] = CAN_CMD_SDO_READ_RECEIVE_UNDEFINED;
        break;
      default:
        error_setf(&dev->error, CAN_SERIAL_DEVICE_ERROR_CONVERT,
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

int can_serial_device_send(can_serial_device_t* dev, unsigned char* data,
    size_t num) {
  unsigned char buffer;
  unsigned char crc_value[2];
  int result = 0;

  error_clear(&dev->error);
  
  can_serial_crc(data, num, crc_value);
  data[num-2] = crc_value[0];
  data[num-1] = crc_value[1];

  can_serial_reorder_bytes(data, num);

  if (serial_device_write(&dev->serial_dev, data, 1) < 0) {
    error_blame(&dev->error, &dev->serial_dev.error,
      CAN_SERIAL_DEVICE_ERROR_SEND);
    return -dev->error.code;
  }

  result = serial_device_read(&dev->serial_dev, &buffer, 1);
  if (result > 0) {
    if (buffer == CAN_SERIAL_DEVICE_ACK_FAILED) {
      error_setf(&dev->error, CAN_SERIAL_DEVICE_ERROR_SEND,
        "Send acknowledge failed");
      return -dev->error.code;
    }
    else if (buffer != CAN_SERIAL_DEVICE_ACK_OKAY) {
      error_setf(&dev->error, CAN_SERIAL_DEVICE_ERROR_SEND,
        "Unexpected response: 0x%02x", buffer);
      return -dev->error.code;
    }
  }
  else {
    error_blame(&dev->error, &dev->serial_dev.error,
      CAN_SERIAL_DEVICE_ERROR_SEND);
    return -dev->error.code;
  }

  if (serial_device_write(&dev->serial_dev, &data[1], num-1) < 0) {
    error_blame(&dev->error, &dev->serial_dev.error,
      CAN_SERIAL_DEVICE_ERROR_SEND);
    return -dev->error.code;
  }

  result = serial_device_read(&dev->serial_dev, &buffer, 1);
  if (result > 0) {
    if (buffer == CAN_SERIAL_DEVICE_ACK_FAILED) {
      error_setf(&dev->error, CAN_SERIAL_DEVICE_ERROR_SEND,
        "Send acknowledge failed");
      return -dev->error.code;
    }
    else if (buffer != CAN_SERIAL_DEVICE_ACK_OKAY) {
      error_setf(&dev->error, CAN_SERIAL_DEVICE_ERROR_SEND,
        "Unexpected response: 0x%02x", buffer);
      return -dev->error.code;
    }
  }
  else {
    error_blame(&dev->error, &dev->serial_dev.error,
      CAN_SERIAL_DEVICE_ERROR_SEND);
    return -dev->error.code;
  }

  return num;
}

int can_serial_device_receive(can_serial_device_t* dev, unsigned char* data) {
  unsigned char buffer, crc_value[2];
  int i, result = 0, num_exp = 0;

  error_clear(&dev->error);
  
  result = serial_device_read(&dev->serial_dev, &buffer, 1);
  if (result > 0) {
    if (buffer != CAN_SERIAL_DEVICE_OPCODE_RESPONSE) {
      error_setf(&dev->error, CAN_SERIAL_DEVICE_ERROR_RECEIVE,
        "Unexpected response: 0x%02x", buffer);
      return -dev->error.code;
    }
    else
      data[0] = CAN_SERIAL_DEVICE_OPCODE_RESPONSE;
  }
  else {
    error_blame(&dev->error, &dev->serial_dev.error,
      CAN_SERIAL_DEVICE_ERROR_RECEIVE);
    return -dev->error.code;
  }

  buffer = CAN_SERIAL_DEVICE_ACK_OKAY;
  if (serial_device_write(&dev->serial_dev, &buffer, 1) < 0) {
    error_blame(&dev->error, &dev->serial_dev.error,
      CAN_SERIAL_DEVICE_ERROR_RECEIVE);
    return -dev->error.code;
  }

  result = serial_device_read(&dev->serial_dev, &buffer, 1);
  if (result > 0)
    data[1] = buffer;
  else {
    error_blame(&dev->error, &dev->serial_dev.error,
      CAN_SERIAL_DEVICE_ERROR_RECEIVE);
    return -dev->error.code;
  }

  num_exp = (data[1]+2)*sizeof(unsigned short);
  for (i = 0; i < num_exp; ++i) {
    if (serial_device_read(&dev->serial_dev, &buffer, 1) > 0)
      data[i+2] = buffer;
    else {
      error_blame(&dev->error, &dev->serial_dev.error,
        CAN_SERIAL_DEVICE_ERROR_RECEIVE);
      return -dev->error.code;
    }
  }
  result = i+2;

  can_serial_reorder_bytes(data, result);
  
  can_serial_crc(data, result, crc_value);
  if ((crc_value[0] == 0x00) && (crc_value[1] == 0x00)) {
    buffer = CAN_SERIAL_DEVICE_ACK_OKAY;
    if (serial_device_write(&dev->serial_dev, &buffer, 1) < 1) {
      error_blame(&dev->error, &dev->serial_dev.error,
        CAN_SERIAL_DEVICE_ERROR_RECEIVE);
      return -dev->error.code;
    }
  }
  else {
    buffer = CAN_SERIAL_DEVICE_ACK_FAILED;
    if (serial_device_write(&dev->serial_dev, &buffer, 1) < 1) {
      error_blame(&dev->error, &dev->serial_dev.error,
        CAN_SERIAL_DEVICE_ERROR_RECEIVE);
      return -dev->error.code;
    }
    
    error_set(&dev->error, CAN_SERIAL_DEVICE_ERROR_CRC);
    return -dev->error.code;
  }

  can_serial_reorder_words(data, result);

  return result;
}

void can_serial_device_init(can_serial_device_t* dev, const char* name) {
  serial_device_init(&dev->serial_dev, name);
  error_init(&dev->error, can_serial_errors);
}

void can_serial_device_destroy(can_serial_device_t* dev) {
  serial_device_destroy(&dev->serial_dev);
  error_destroy(&dev->error);
}
