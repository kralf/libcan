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

#ifndef CAN_MESSAGE_H
#define CAN_MESSAGE_H

/** \file message.h
  * \ingroup can
  * \brief Standard CAN message implementation
  * \author Ralf Kaestner
  * 
  * The standardized CAN message (or frame) consists in an 11-bit identifier,
  * a remote transmission request (RTR) bit, a 4-bit data length field, and
  * 0-8 bytes of data.
  */

#include <stdio.h>

/** \brief Structure defining a CAN message
  */
typedef struct can_message_t {
  unsigned short id;      //!< The CAN message identifier.
  unsigned char rtr;      //!< The CAN message remote transmission request bit.

  unsigned char data[8];  //!< The data of the CAN message.
  size_t data_length;     //!< The length of the CAN message data.
} can_message_t;

/** \brief Initialize CAN message
  * \param[in] message The CAN message to be initialized.
  * \param[in] id The identifier of the CAN message to be initialized.
  * \param[in] rtr The remote transmission request (RTR) bit of the CAN
  *   message to be initialized.
  * \param[in] data The data field of the CAN message to be initialized.
  * \param[in] data_length The length of the data field in [byte]. According
  *   to the CAN bus standard, the maximum permissible data length is 8 bytes.
  *   If the provided length exceeds this limit, the data will be truncated.
  */
void can_message_init(
  can_message_t* message,
  unsigned short id,
  unsigned char rtr,
  const unsigned char* data,
  size_t data_length);

/** \brief Print CAN message
  * \param[in] stream The output stream that will be used for printing the
  *   CAN message.
  * \param[in] message The CAN message that will be printed.
  */
void can_message_print(
  FILE* stream,
  const can_message_t* message);

#endif
