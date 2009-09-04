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
  "success",
  "error opening CAN device",
  "error setting CAN device parameters",
  "error closing CAN device",
  "error sending CAN message",
  "error receiving CAN message",
};

void can_init(can_device_p dev, config_p config) {
  dev->comm_dev = 0;

  config_init_default(&dev->config, &can_default_config);
  if (config)
    config_set(&dev->config, config);

  dev->num_references = 0;
  dev->num_sent = 0;
  dev->num_received = 0;
}

void can_init_arg(can_device_p dev, int argc, char **argv, const char* 
  prefix) {
  config_t config;
  config_init_arg(&config, argc, argv, (prefix) ? prefix : 
    CAN_CONFIG_ARG_PREFIX);

  can_init(dev, &config);

  config_destroy(&config);
}

void can_destroy(can_device_p dev) {
  config_destroy(&dev->config);
}
