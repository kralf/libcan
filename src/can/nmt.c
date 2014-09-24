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

#include "nmt.h"

void can_nmt_init_cob(can_cob_t* cob, unsigned char cs, unsigned char
    node_id) {
  unsigned char data[2] = {cs, node_id};
  can_cob_init(cob, can_protocol_nmt, 0, 0, data, sizeof(data));
}

unsigned char can_nmt_get_cob_cs(const can_cob_t* cob) {
  if (cob->protocol == can_protocol_nmt)
    return cob->data[0];
  else
    return 0;
}

unsigned char can_nmt_get_cob_node_id(const can_cob_t* cob) {
  if (cob->protocol == can_protocol_nmt)
    return cob->data[1];
  else
    return 0;
}
