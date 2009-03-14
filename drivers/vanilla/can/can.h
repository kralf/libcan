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

#ifndef _CAN_H
#define _CAN_H

/** \file
  * \brief Generic CAN communication
  * Common commands used to communicate via the CAN protocol.
  * These methods are implemented by all CAN communication backends.
  */

/** \brief Structure defining a CAN message
  */
typedef struct {
  int id;                    //!< The CAN message identifier.
  unsigned char content[8];  //!< The actual CAN message content.
} can_message_t;

/** \brief Initialize CAN communication by opening devices
  * \param[in] dev_name The name of the character device to be used for CAN
  *   communication.
  */
void can_init(
  const char* dev_name);

/** \brief Close CAN communication by closing devices
  */
void can_close(void);

/** \brief Send a CAN message
  * \param[in] message The CAN message to be sent.
  */
void can_send_message(
  can_message_t* message);

/** \brief Asynchronously read a CAN message
  * This function instantly returns to the caller.
  */
void can_read_message(void);

/** \brief Asynchronous CAN read message handler
  * \param[in] message The received CAN message.
  */
void can_read_message_handler(
  const can_message_t* message);

#endif
