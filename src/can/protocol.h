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

#ifndef CAN_PROTOCOL_H
#define CAN_PROTOCOL_H

/** \file protocol.h
  * \ingroup can
  * \brief CANopen protocols
  * \author Ralf Kaestner
  * 
  * This header specifies the available CANopen protocols.
  */

#include <stdlib.h>

/** \brief CANopen protocols
  */
typedef enum {
  can_protocol_sync,
  //!< CANopen Synchronization Object (SYNC) protocol.
  can_protocol_emcy,
  //!< CANopen Emergency Object (EMCY) protocol.
  can_protocol_time,
  //!< CANopen Time Stamp Object (TIME) protocol.
  can_protocol_nmt,
  //!< CANopen Network Management (NMT) protocol.
  can_protocol_nmt_ec,
  //!< CANopen Network Management Error Control (NMT-EC) protocol.
  can_protocol_lss,
  //!< CANopen Layer Setting Services (LSS) protocol.
  can_protocol_sdo,
  //!< CANopen Service Data Object (SDO) protocol.
  can_protocol_pdo,
  //!< CANopen Process Data Object (PDO) protocol.
} can_protocol_t;

/** \brief Predefined CANopen protocols
  */
extern const char* can_protocols[];

#endif
