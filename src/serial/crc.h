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

#ifndef CAN_SERIAL_CRC_H
#define CAN_SERIAL_CRC_H

/** \file serial/crc.h
  * \ingroup can_serial
  * \brief CAN-Serial checksum calculation helpers
  * \author Marc Rauer, Ralf Kaestner
  *
  * This file provides an interface for calculating the checksums required
  * to transfer CANopen messages over RS232 serial connections to maxon EPOS
  * controllers.
  */

#include <stdlib.h>

/** \brief Calculate a 16-bit CRC checksum using CRC-CCITT algorithm
  * \param[in] data An array of bytes representing the serial data frame.
  * \param[in] num The number of bytes in the data frame.
  * \param[out] crc_value An array of two bytes to store the CRC-word.
  * \return The number of words built from the array.
  * 
  * Calculation has to include all bytes in the data frame. Internally,
  * the array is transformed to an array of words in order to calculate the
  * CRC. The CRC word is then tranformed back to an array of characters.
  */
size_t can_serial_crc(
  unsigned char* data,
  size_t num,
  unsigned char* crc_value);

/** \brief Implementation of the CRC-CCITT algorithm
  * \param[in] data An array of words containing the serial data frame.
  * \param[in] num The number of words in the data frame.
  * \return The calculated CRC-value.
  */
unsigned short can_serial_crc_ccitt(
  unsigned short* data,
  size_t num);

#endif
