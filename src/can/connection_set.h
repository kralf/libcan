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

#ifndef CAN_CONNECTION_SET_H
#define CAN_CONNECTION_SET_H

/** \file connection_set.h
  * \ingroup can
  * \brief CANopen connection set
  * \author Ralf Kaestner
  * 
  * This header implements and defines the available connections to
  * CANopen services in the form of a connection set.
  */

#include "connection.h"

/** \brief CANopen connection set structure
  */
typedef struct can_connection_set_t {
  can_connection_t* connections;  //!< The connections in the set.
  size_t num_connections;         //!< The number of connections in the set.
} can_connection_set_t;

/** \brief Predefined CANopen default connection set
  *
  * The default service connections are specified by the CANopen predefined
  * connection set.
  */
extern const can_connection_set_t can_default_connection_set;

/** \brief Initialize CAN connection set
  * \param[in] connection_set The CAN connection set to be initialized.
  * \param[in] connections The CAN connections used to initialize the
  *   connection set.
  * \param[in] num_connections The number of CAN connections used to
  *   initialize the connection set.
  */
void can_connection_set_init(
  can_connection_set_t* connection_set,
  const can_connection_t* connections,
  size_t num_connections);

/** \brief Initialize CAN connection set by copying
  * \param[in] connection_set The CAN connection set to be initialized.
  * \param[in] src_connection_set The source connection set used to
  *   initialize the connection set.
  */
void can_connection_set_init_copy(
  can_connection_set_t* connection_set,
  const can_connection_set_t* src_connection_set);

/** \brief Destroy CAN connection set
  * \param[in] connection_set The CAN connection set to be destroyed.
  */
void can_connection_set_destroy(
  can_connection_set_t* connection_set);

/** \brief Add connection to a CAN connection set
  * \param[in] connection_set The CAN connection to which the connection
  *   shall be added.
  * \param[in] connection The connection to be added to the CAN connection
  *   set.
  * \return The new number of connections in the connection set.
  */
size_t can_connection_set_add(
  can_connection_set_t* connection_set,
  const can_connection_t* connection);

/** \brief Find connection in the CAN connection set by service
  * \param[in] connection_set The CAN connection set to be searched for the
  *   connection.
  * \param[in] service The CANopen service to retrieve the connection for.
  * \param[in] direction The direction of the service connection to be
  *   retrieved.
  * \return The index of the connection matching the provided service and
  *   direction, or -1  if no such connection exists in the connection set.
  */
ssize_t can_connection_set_find_service(
  const can_connection_set_t* connection_set,
  can_service_t service,
  can_direction_t direction);

/** \brief Find connection in the CAN connection set by COB-ID
  * \param[in] connection_set The CAN connection set to be searched for the
  *   connection.
  * \param[in] cob_id The communication object Identifier (COB-ID) identifying
  *   the connection to be found.
  * \return The index of the connection matching the provided COB-ID or -1 
  *   if no such connection exists in the connection set.
  */
ssize_t can_connection_set_find_cob_id(
  const can_connection_set_t* connection_set,
  unsigned short cob_id);

/** \brief Print CAN connection set
  * \param[in] stream The output stream that will be used for printing the
  *   CAN connection set.
  * \param[in] connection_set The CAN connection set that will be printed.
  */
void can_connection_set_print(
  FILE* stream,
  const can_connection_set_t* connection_set);

#endif
