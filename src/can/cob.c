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

#include "cob.h"

void can_cob_init(can_cob_t* cob, unsigned char node_id, unsigned char rtr,
    const unsigned char* data, size_t data_length) {
  cob->node_id = node_id;
  cob->rtr = rtr;
  
  if (data_length > sizeof(cob->data))
    data_length = sizeof(cob->data);  
  memcpy(cob->data, data, data_length);
  if (data_length < sizeof(cob->data))
    memset(&cob->data[data_length], 0, sizeof(cob->data)-data_length);
  cob->data_length = data_length;
}

void can_cob_print(FILE* stream, const can_cob_t* cob) {
  fprintf(stream, "NODE-ID %03d RTR %1d: ",
    cob->node_id,
    cob->rtr);
  
  if (cob->data_length) {
    size_t i;
    for (i = 0; i < cob->data_length; ++i) {
      if (!i)
        fprintf(stream, "Data");
      fprintf(stream, " %02x", cob->data[i]);
    }
  }
  else
    fprintf(stream, "No data");
}
