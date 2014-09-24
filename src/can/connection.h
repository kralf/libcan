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

#ifndef CAN_CONNECTION_H
#define CAN_CONNECTION_H

/** \file connection.h
  * \ingroup can
  * \brief CANopen service connection
  * \author Ralf Kaestner
  * 
  * This header implements the connection to a specific CANopen service.
  */

#include <stdio.h>

#include "service.h"
#include "protocol.h"
#include "direction.h"

/** \brief CANopen service connection structure
  */
typedef struct can_connection_t {
  can_service_t service;     //!< The CANopen service behind the connection.
  can_direction_t direction; //!< The direction of the service connection.
  
  unsigned short cob_id;     //!< The base COB-ID for connecting to the service.
  size_t num_cob_ids;        //!< The number of COB-IDs assigned to the service.
} can_connection_t;

/** \brief Initialize CAN connection
  * \param[in] connection The CAN connection to be initialized.
  * \param[in] service The CANopen service behind the connection to be
  *   initialized.
  * \param[in] direction The communication direction of the connection to be
  *   initialized.
  * \param[in] cob_id The base communication object identifier (COB-ID) for
  *   CANopen communication objects exchanged via the connection.
  * \param[in] num_cob_ids The number of COB-IDs allocated for CANopen
  *   communication objects to be exchanged via the connection.
  */
void can_connection_init(
  can_connection_t* connection,
  can_service_t service,
  can_direction_t direction,
  unsigned short cob_id,
  size_t num_cob_ids);

/** \brief Print CAN connection
  * \param[in] stream The output stream that will be used for printing the
  *   CAN connection.
  * \param[in] connection The CAN connection that will be printed.
  */
void can_connection_print(
  FILE* stream,
  const can_connection_t* connection);

#endif
