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

#include <string.h>

#include "can.h"

const char* can_errors[] = {
  "Success",
  "Configuration error",
  "Error opening CAN device",
  "Error setting CAN device parameters",
  "Error closing CAN device",
  "Error sending CAN message",
  "Error receiving CAN message",
};

void can_init(can_device_p dev) {
  can_init_config(dev, &can_default_config);  
}

int can_init_config(can_device_p dev, config_p config) {
  dev->comm_dev = 0;
  
  dev->num_references = 0;
  dev->num_sent = 0;
  dev->num_received = 0;
  
  config_init_copy(&dev->config, &can_default_config);
  if ((config != &can_default_config) && config_set(&dev->config, config))
    return CAN_ERROR_CONFIG;

  return CAN_ERROR_NONE;
}

int can_init_config_parse(can_device_p dev, config_parser_p parser,
    const char* option_group, int argc, char **argv, config_parser_exit_t
    exit) {
  char summary[sizeof(((config_parser_option_group_p)0)->summary)];
  char description[sizeof(((config_parser_option_group_p)0)->description)];
  
  sprintf(summary, "%s options", can_device_name);
  sprintf(description,
    "These options control the settings for the %s communication interface. "
    "The type of interface depends on the momentarily selected alternative "
    "of the underlying CANopen library. Use the update-alternatives command "
    "to inspect or change this alternative.", can_device_name);
  config_p config = &config_parser_add_option_group(parser,
    option_group ? option_group : CAN_CONFIG_PARSER_OPTION_GROUP,
    &can_default_config, summary, description)->options;
  
  if (config_parser_parse(parser, argc, argv, exit))
    return CAN_ERROR_CONFIG;
  
  return can_init_config(dev, config);
}

void can_destroy(can_device_p dev) {
  config_destroy(&dev->config);
}
