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

can_device_p can_init(can_parameter_t parameters[], ssize_t num_parameters) {
  can_device_p dev = malloc(sizeof(can_device_t));
  memset(dev, 0, sizeof(can_device_t));

  if (num_parameters) {
    dev->parameters = malloc(num_parameters*sizeof(can_parameter_t));
    memcpy(dev->parameters, parameters, num_parameters*
      sizeof(can_parameter_t));
    dev->num_parameters = num_parameters;
  }

  return dev;
}

void can_destroy(can_device_p dev) {
  if (dev->parameters)
    free(dev->parameters);

  free(dev);
}
