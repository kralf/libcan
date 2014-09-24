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

#include "reorder.h"

size_t can_serial_reorder_bytes(unsigned char* data, size_t num) {
  unsigned char tmp;
  int i;

  for (i = 2; i < num; i += 2) {
    tmp = data[i];

    data[i] = data[i+1];
    data[i+1] = tmp;
  }

  return i;
}

size_t can_serial_reorder_words(unsigned char* data, size_t num) {
  unsigned char tmp_lb, tmp_hb;
  int i;

  for (i = 2; i < (num-2); i += 4) {
    tmp_hb = data[i];
    tmp_lb = data[i+1];

    data[i] = data[i+2];
    data[i+1] = data[i+3];

    data[i+2] = tmp_hb;
    data[i+3] = tmp_lb;
  }

  return i;
}
