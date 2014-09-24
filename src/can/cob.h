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

#ifndef CAN_COB_H
#define CAN_COB_H

/** \file object.h
  * \ingroup can
  * \brief CANopen communication object implementation
  * \author Ralf Kaestner
  * 
  * The CANopen communication object (COB) consists in a 4-bit function code,
  * a 7-bit CANopen node identifier, a remote transmission request (RTR) bit,
  * a 4-bit data length field, and 0-8 bytes of data. The 4-bit function code
  * and the 7-bit node identifier taken together represent the communication
  * object identifier (COB-ID) and thus pose a special interpretation of the
  * standard CAN message identifier. In this implementation, however, the
  * function code is not made an explicit property of the communication object
  * as it will be determined by the connection set of the CANopen device
  * responsible for sending or receiving it.
  */

#include <stdio.h>

/** \brief Structure defining a CANopen communication object
  */
typedef struct can_cob_t {
  unsigned char node_id;    //!< The CANopen node identifier.
  unsigned char rtr;        //!< The remote transmission request bit.

  unsigned char data[8];    //!< The data of the communication object.
  size_t data_length;       //!< The length of the data.
} can_cob_t;

/** \brief Initialize CANopen communication object
  * \param[in] cob The CANopen communication object to be initialized.
  * \param[in] node_id The node identifier of the CANopen communication
  *   object to be initialized.
  * \param[in] rtr The remote transmission request (RTR) bit of the CANopen
  *   communication object to be initialized.
  * \param[in] data The data field of the CANopen communication object to
  *   be initialized.
  * \param[in] data_length The length of the data field in [byte]. According
  *   to the CANopen standard and the underlying CAN bus standard, the maximum
  *   permissible data length is 8 bytes. If the provided length exceeds this
  *   limit, the data will be truncated.
  */
void can_cob_init(
  can_cob_t* cob,
  unsigned char node_id,
  unsigned char rtr,
  const unsigned char* data,
  size_t data_length);

/** \brief Print CANopen communication object
  * \param[in] stream The output stream that will be used for printing the
  *   CANopen communication object.
  * \param[in] cob The CANopen communication object that will be printed.
  */
void can_cob_print(
  FILE* stream,
  const can_cob_t* cob);

#endif
