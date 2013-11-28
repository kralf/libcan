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

#include "can.h"

const char* can_errors[] = {
  "Success",
  "CAN configuration error",
  "Failed to open CAN device",
  "Failed to set CAN device parameters",
  "Failed to close CAN device",
  "Failed to send CAN message",
  "Failed to receive CAN message",
};

void can_device_init(can_device_t* dev) {
  dev->comm_dev = 0;
  
  dev->num_references = 0;
  dev->num_sent = 0;
  dev->num_received = 0;
  
  config_init_default(&dev->config, &can_default_config);
  error_init(&dev->error, can_errors);
}

int can_device_init_config(can_device_t* dev, const config_t* config) {
  can_device_init(dev);
  
  if (config_set(&dev->config, config))
    error_blame(&dev->error, &dev->config.error, CAN_ERROR_CONFIG);

  return dev->error.code;
}

int can_device_init_config_parse(can_device_t* dev, config_parser_t* parser,
    const char* option_group, int argc, char **argv, config_parser_exit_t
    exit) {
  char* summary = 0;
  char* description = 0;
  
  string_printf(&summary, "%s options", can_device_name);
  string_printf(&description,
    "These options control the settings for the %s communication interface. "
    "The type of interface depends on the momentarily selected alternative "
    "of the underlying CANopen library. Use the update-alternatives command "
    "to inspect or change this alternative.", can_device_name);
  config_t* config = &config_parser_add_option_group(parser,
    option_group ? option_group : CAN_CONFIG_PARSER_OPTION_GROUP,
    &can_default_config, summary, description)->options;

  string_destroy(&summary);
  string_destroy(&description);
  
  int result;
  if ((result = config_parser_parse(parser, argc, argv, exit))) {
    error_blame(&dev->error, &parser->error, CAN_ERROR_CONFIG);
    return dev->error.code;
  }

  return can_device_init_config(dev, config);
}

void can_device_destroy(can_device_t* dev) {
  config_destroy(&dev->config);
  error_destroy(&dev->error);
}
