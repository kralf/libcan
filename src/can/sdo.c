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

#include "sdo.h"

void can_sdo_send_init_cob(can_cob_t* cob, unsigned char node_id,
    can_sdo_ccs_t ccs, can_sdo_transfer_t transfer, unsigned short index,
    unsigned char subindex, const unsigned char* data, size_t data_length) {
  unsigned char cob_data[sizeof(cob->data)];
  
  if (data_length+4 > sizeof(cob->data))
    data_length = sizeof(cob->data)-4;
  
  cob_data[0] = (ccs << 5);
  if (transfer == can_sdo_transfer_expedited) {
    cob_data[0] |= 0x03;
    cob_data[0] |= ((sizeof(cob->data)-4-data_length) << 2);
  }
  cob_data[1] = (index >> 8);
  cob_data[2] = index;
  cob_data[3] = subindex;
  memcpy(&cob_data[4], data, data_length);
  
  can_cob_init(cob, can_protocol_sdo, node_id, 0, cob_data, sizeof(cob_data));
}

can_sdo_ccs_t can_sdo_get_cob_ccs(const can_cob_t* cob) {
  if (cob->protocol == can_protocol_sdo)
    return (cob->data[0] >> 5);
  else
    return -1;
}

can_sdo_transfer_t can_sdo_get_cob_transfer(const can_cob_t* cob) {
  if (cob->protocol == can_protocol_sdo)
    return ((cob->data[0] >> 1) & 0x01);
  else
    return -1;
}

unsigned short can_sdo_get_cob_index(const can_cob_t* cob) {
  if (cob->protocol == can_protocol_sdo)
    return (cob->data[1] << 8)+cob->data[2];
  else
    return 0;
}

unsigned char can_sdo_get_cob_subindex(const can_cob_t* cob) {
  if (cob->protocol == can_protocol_sdo)
    return cob->data[3];
  else
    return 0;
}
