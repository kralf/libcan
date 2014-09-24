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

#include "connection.h"

void can_connection_init(can_connection_t* connection, can_service_t service,
    can_direction_t direction, unsigned short cob_id, size_t num_cob_ids) {
  connection->service = service;
  connection->direction = direction;
  
  connection->cob_id = cob_id;
  connection->num_cob_ids = num_cob_ids;
}

void can_connection_print(FILE* stream, const can_connection_t* connection) {
  fprintf(stream, "%s %s: %03x",
    can_services[connection->service],
    (connection->direction == can_direction_send) ? "SEND" : "RECV",
    connection->cob_id);
  
  if (connection->num_cob_ids > 1)
    fprintf(stream, " - %03x", (unsigned short)(connection->cob_id+
      connection->num_cob_ids));
}
