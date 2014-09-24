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

#include "crc.h"

size_t can_serial_crc(unsigned char* data, size_t num, unsigned char*
    crc_value) {
  unsigned short* word_data = (unsigned short*)data;
  size_t num_words = num/2;
  unsigned short crc;

  crc = can_serial_crc_ccitt(word_data, num_words);
  crc_value[0] = (crc >> 8);
  crc_value[1] = crc;

  return num_words;
}

unsigned short can_serial_crc_ccitt(unsigned short* data, size_t num) {
  unsigned short shift, c, carry, crc = 0;
  int i;

  for (i = 0; i < num; ++i) {
    shift = 0x8000;
    c = (data[i] << 8) | (data[i] >> 8);

    do {
      carry = crc & 0x8000;
      crc <<= 1;

      if (c & shift)
        ++crc;
      if (carry)
        crc ^= 0x1021;

      shift >>= 1;
    }
    while (shift);
  }

  return crc;
}
