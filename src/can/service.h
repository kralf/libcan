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

#ifndef CAN_SERVICE_H
#define CAN_SERVICE_H

/** \file service.h
  * \ingroup can
  * \brief CANopen services
  * \author Ralf Kaestner
  * 
  * This header specifies the available CANopen services.
  */

#include <stdlib.h>

#include "protocol.h"

/** \brief CANopen services
  * 
  * The CANopen services specified by this enumerable type are limited to
  * the predefined connection set of the CANopen standard. Additional PDO
  * services may be referred to by using their index as an offset to
  * can_service_pdo1.
  */
typedef enum {
  can_service_sync,
  //!< CANopen Synchronization Object (SYNC) service.
  can_service_emcy,
  //!< CANopen Emergency Object (EMCY) service.
  can_service_time,
  //!< CANopen Time Stamp Object (TIME) service.
  can_service_nmt,
  //!< CANopen Network Management (NMT) service.
  can_service_nmt_ec,
  //!< CANopen Network Management Error Control (NMT-EC) service.
  can_service_lss,
  //!< CANopen Layer Setting Services (LSS).
  can_service_sdo,
  //!< CANopen Service Data Object (SDO) service.
  can_service_pdo1,
  //!< First CANopen Process Data Object (PDO) service.
  can_service_pdo2,
  //!< Second CANopen Process Data Object (PDO) service.
  can_service_pdo3,
  //!< Third CANopen Process Data Object (PDO) service.
  can_service_pdo4,
  //!< Fourth CANopen Process Data Object (PDO) service.
} can_service_t;

/** \brief Predefined CANopen services
  */
extern const char* can_services[];

/** \brief Predefined CANopen service protocols
  */
extern const can_protocol_t can_service_protocols[];

#endif
