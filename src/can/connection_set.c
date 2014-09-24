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

#include "connection_set.h"

can_connection_t can_default_connections[] = {
  {can_service_nmt, can_direction_receive, 0x000, 1},
  {can_service_sync, can_direction_receive, 0x080, 1},
  {can_service_emcy, can_direction_send, 0x080, 128},
  {can_service_time, can_direction_receive, 0x100, 1},
  {can_service_pdo1, can_direction_send, 0x180, 128},
  {can_service_pdo1, can_direction_receive, 0x200, 128},
  {can_service_pdo2, can_direction_send, 0x280, 128},
  {can_service_pdo2, can_direction_receive, 0x300, 128},
  {can_service_pdo3, can_direction_send, 0x380, 128},
  {can_service_pdo3, can_direction_receive, 0x400, 128},
  {can_service_pdo4, can_direction_send, 0x480, 128},
  {can_service_pdo4, can_direction_receive, 0x500, 128},
  {can_service_sdo, can_direction_send, 0x580, 128},
  {can_service_sdo, can_direction_receive, 0x600, 128},
  {can_service_nmt_ec, can_direction_send, 0x700, 128},
  {can_service_lss, can_direction_send, 0x7e4, 1},
  {can_service_lss, can_direction_receive, 0x7e5, 1},
};

const can_connection_set_t can_default_connection_set = {
  can_default_connections,
  sizeof(can_default_connections)/sizeof(can_connection_t)
};

void can_connection_set_init(can_connection_set_t* connection_set, const
    can_connection_t* connections, size_t num_connections) {
  connection_set->connections = malloc(num_connections*
    sizeof(can_connection_t));
  memcpy(connection_set->connections, connections, num_connections*
    sizeof(can_connection_t));
  connection_set->num_connections = num_connections;
}

void can_connection_set_init_copy(can_connection_set_t* connection_set,
    const can_connection_set_t* src_connection_set) {
  can_connection_set_init(connection_set, src_connection_set->connections,
    src_connection_set->num_connections);
}

void can_connection_set_destroy(can_connection_set_t* connection_set) {
  if (connection_set->num_connections) {
    free(connection_set->connections);
    connection_set->connections = 0;
    connection_set->num_connections = 0;
  }
}

size_t can_connection_set_add(can_connection_set_t* connection_set,
    const can_connection_t* connection) {
  connection_set->connections = realloc(connection_set->connections,
    (connection_set->num_connections+1)*sizeof(can_connection_t));
  memcpy(&connection_set->connections[connection_set->num_connections],
    connection, sizeof(can_connection_t));
  ++connection_set->num_connections;
  
  return connection_set->num_connections;
}

ssize_t can_connection_set_find_service(const can_connection_set_t*
    connection_set, can_service_t service, can_direction_t direction) {
  size_t i;
  
  for (i = 0; i < connection_set->num_connections; ++i)
    if ((service == connection_set->connections[i].service) &&
        (direction == connection_set->connections[i].direction))
      return i;
    
  return -1;
}

ssize_t can_connection_set_find_cob_id(const can_connection_set_t*
    connection_set, unsigned short cob_id) {
  size_t i;
  
  for (i = 0; i < connection_set->num_connections; ++i)
    if ((cob_id >= connection_set->connections[i].cob_id) &&
      (cob_id < connection_set->connections[i].cob_id+
        connection_set->connections[i].num_cob_ids))
      return i;
    
  return -1;
}

void can_connection_set_print(FILE* stream, const can_connection_set_t*
    connection_set) {
  size_t i;
  
  for (i = 0; i < connection_set->num_connections; ++i) {
    if (i)
      fprintf(stream, "\n");
    can_connection_print(stream, &connection_set->connections[i]);
  }
}
