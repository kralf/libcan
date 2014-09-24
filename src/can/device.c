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

#include "string/string.h"

#include "device.h"

#include "can.h"

const char* can_device_errors[] = {
  "Success",
  "CAN device configuration error",
  "Failed to open CAN device",
  "Failed to set CAN device parameters",
  "Failed to close CAN device",
  "CAN device protocol error"
  "CAN device connection error"
  "Failed to send message from CAN device",
  "Failed to receive message on CAN device",
  "CAN conversion error"
};

void can_device_init(can_device_t* dev) {
  dev->comm_dev = 0;
  
  dev->num_references = 0;
  dev->num_sent = 0;
  dev->num_received = 0;
  
  config_init_default(&dev->config, &can_device_default_config);
  can_connection_set_init_copy(&dev->connection_set,
    &can_default_connection_set);
  
  error_init(&dev->error, can_device_errors);
}

int can_device_init_config(can_device_t* dev, const config_t* config) {
  can_device_init(dev);
  
  if (config_set(&dev->config, config))
    error_blame(&dev->error, &dev->config.error, CAN_DEVICE_ERROR_CONFIG);

  return dev->error.code;
}

int can_device_init_config_parse(can_device_t* dev, config_parser_t* parser,
    const char* option_group, int argc, char **argv, config_parser_exit_t
    exit) {
  char* summary = 0;
  char* description = 0;
  
  can_device_init(dev);
  
   option_group = option_group ? option_group : CAN_CONFIG_PARSER_OPTION_GROUP;
  string_printf(&summary, "%s options", can_device_name);
  string_printf(&description,
    "These options control the settings for the %s communication interface. "
    "The type of interface depends on the momentarily selected alternative "
    "of the underlying CANopen library. Use the update-alternatives command "
    "to inspect or change this alternative.", can_device_name);
  config_parser_add_option_group(parser, option_group,
    &can_device_default_config, summary, description);

  string_destroy(&summary);
  string_destroy(&description);
  
  if (config_parser_parse(parser, argc, argv, exit))
    error_blame(&dev->error, &parser->error, CAN_DEVICE_ERROR_CONFIG);
  else if (config_set(&dev->config, &config_parser_get_option_group(
      parser, option_group)->options))
    error_blame(&dev->error, &dev->config.error, CAN_DEVICE_ERROR_CONFIG);

  return dev->error.code;
}

void can_device_destroy(can_device_t* dev) {
  config_destroy(&dev->config);
  error_destroy(&dev->error);
}

int can_device_send_cob(can_device_t* dev, can_service_t service, const
    can_cob_t* cob) {
  can_message_t message;
  
  if (!can_device_cob_to_message(dev, service, can_direction_receive, cob,
      &message))
    can_device_send_message(dev, &message);
    
  return dev->error.code;
}

int can_device_receive_cob(can_device_t* dev, can_service_t* service,
    can_cob_t* cob) {
  can_message_t message;
  can_direction_t direction;
  
  if (!can_device_receive_message(dev, &message) &&
      !can_device_message_to_cob(dev, &message, service, &direction, cob) &&
      (direction != can_direction_send))
    error_setf(&dev->error, CAN_DEVICE_ERROR_CONNECTION,
      "Received unexpected COB-ID: %03x", message.id);
  
  return dev->error.code;
}

int can_device_cob_to_message(can_device_t* dev, can_service_t service,
    can_direction_t direction, const can_cob_t* cob, can_message_t* message) {
  error_clear(&dev->error);
  
  ssize_t result = can_connection_set_find_service(&dev->connection_set,
    service, direction);
  if (result < 0) {
    error_setf(&dev->error, CAN_DEVICE_ERROR_CONVERT,
      "No such service: %s", can_services[service]);
    return dev->error.code;
  }
  
  can_connection_t* connection = &dev->connection_set.connections[result];
  if (cob->node_id+1 < connection->num_cob_ids) {
    error_setf(&dev->error, CAN_DEVICE_ERROR_CONVERT,
      "Node-ID out of range: %d", cob->node_id);
    return dev->error.code;
  }  
  if (connection->protocol != cob->protocol) {
    error_setf(&dev->error, CAN_DEVICE_ERROR_CONVERT,
      "%s protocol mismatch: %s", can_services[service],
      can_protocols[cob->protocol]);
    return dev->error.code;
  }
  
  can_message_init(message, connection->cob_id+cob->node_id,
    cob->rtr, cob->data, cob->data_length);
  
  return dev->error.code;
}

int can_device_message_to_cob(can_device_t* dev, const can_message_t* message,
    can_service_t* service, can_direction_t* direction, can_cob_t* cob) {
  error_clear(&dev->error);
  
  ssize_t result = can_connection_set_find_cob_id(
    &dev->connection_set, message->id);
  if (result < 0) {
    error_setf(&dev->error, CAN_DEVICE_ERROR_CONVERT,
      "Unspecified COB-ID: %03x", message->id);
    return dev->error.code;
  }

  can_connection_t* connection = &dev->connection_set.connections[result];
  
  *service = connection->service;
  *direction = connection->direction;
  can_cob_init(cob, connection->protocol, message->id-connection->cob_id,
    message->rtr, message->data, message->data_length);
  
  return dev->error.code;
}
