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

#ifndef CAN_SERIAL_REORDER_H
#define CAN_SERIAL_REORDER_H

/** \file serial/reorder.h
  * \ingroup can_serial
  * \brief CAN-Serial data reordering helpers
  * \author Marc Rauer, Ralf Kaestner
  *
  * This file provides an interface for reordering the serial data frames to
  * be transferred over RS232 serial connections to maxon EPOS controllers.
  */

#include <stdlib.h>

/** \brief Change the order of bytes in serial data frames
  * \param[in,out] data An array of bytes representing the serial data frame
  *   for which to change the order.
  * \param[in] num The number of bytes in the array.
  * \return The number of reordered bytes within the serial data frame.
  * 
  * The first two characters will be ignored, the following characters
  * will be reordered. This is necessary according to the EPOS Communication
  * Guide.
  */
size_t can_serial_reorder_bytes(
  unsigned char* data,
  size_t num);

/** \brief Change the order of words in serial data frames
  * \param[in,out] data An array of words representing the serial data for
  *   which to change order.
  * \param[in] num The number of bytes in the word array.
  * \return The number of reordered bytes within the serial data frame.
  * 
  * The first two characters will be ignored, the following characters
  * will be reordered in groups of two. This is necessary according to the
  * EPOS Communication Guide.
  */
size_t can_serial_reorder_words(
  unsigned char* data,
  size_t num);

#endif
