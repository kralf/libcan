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

#ifndef CAN_NMT_H
#define CAN_NMT_H

/** \file nmt.h
  * \ingroup can
  * \brief CANopen Network Management (NMT) protocol
  * \author Ralf Kaestner
  * 
  * The NMT protocol is used to issue state machine change commands, e.g.,
  * to start, stop, initalize, and reset the devices.
  */

#include "cob.h"

/** \brief Initialize CANopen NMT communication object
  * \param[in] cob The CANopen NMT communication object to be initialized.
  * \param[in] cs The command specifier of the NMT communication object.
  * \param[in] node_id The node identifier of the NMT communication object.
  * 
  * This initializer calls can_cob_init() with the data field constructed
  * according to the CANopen NMT protocol.
  */
void can_nmt_init_cob(
  can_cob_t* cob,
  unsigned char cs,
  unsigned char node_id);

/** \brief Retrieve command specifier of an NMT communication object
  * \param[in] cob The NMT communication object to retrieve the command
  *   specifier for.
  * \return The command specifier of the provided NMT communication object or
  *   zero if the provided communication object is not an NMT communication
  *   object.
  */
unsigned char can_nmt_get_cob_cs(
  const can_cob_t* cob);

/** \brief Retrieve node identifier of an NMT communication object
  * \param[in] cob The NMT communication object to retrieve the node
  *   identifier for.
  * \return The node identifier of the provided NMT communication object or
  *   zero if the provided communication object is not an NMT communication
  *   object.
  */
unsigned char can_nmt_get_cob_node_id(
  const can_cob_t* cob);

#endif
