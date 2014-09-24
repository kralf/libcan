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

#include "message.h"

void can_message_init(can_message_t* message, unsigned short id,
    unsigned char rtr, const unsigned char* data, size_t data_length) {
  message->id = id;
  message->rtr = rtr;
  
  if (data_length > sizeof(message->data))
    data_length = sizeof(message->data);  
  memcpy(message->data, data, data_length);
  if (data_length < sizeof(message->data))
    memset(&message->data[data_length], 0, sizeof(message->data)-data_length);
  message->data_length = data_length;
}

void can_message_print(FILE* stream, const can_message_t* message) {
  fprintf(stream, "ID %03x RTR %1d: ", message->id, message->rtr);
  
  if (message->data_length) {
    size_t i;
    for (i = 0; i < message->data_length; ++i) {
      if (!i)
        fprintf(stream, "Data");
      fprintf(stream, " %02x", message->data[i]);
    }
  }
  else
    fprintf(stream, "No data");
}
