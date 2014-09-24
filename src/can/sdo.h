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

#ifndef CAN_SDO_H
#define CAN_SDO_H

/** \file sdo.h
  * \ingroup can
  * \brief CANopen Service Data Object (SDO) protocol
  * \author Ralf Kaestner
  * 
  * The SDO protocol is a simple CANopen protocol which is used for accessing
  * values from an object dictionary of a remote CANopen device.
  */

#include "cob.h"

/** \name SDO Protocol Communication Object Identifiers
  * \brief Predefined SDO communication object identifiers as specified
  *   by the CANopen standard
  */
//@{
#define CAN_SDO_COB_ID_SEND                       0x0580
#define CAN_SDO_COB_ID_RECEIVE                    0x0600
//@}

/** \brief SDO Client Command Specifiers (CCS)
  * 
  * In CANopen terminology, communication is viewed from the SDO server,
  * so that a read from an object dictionary results in an SDO upload and
  * a write to a dictionary entry is an SDO download.
  */
typedef enum {
  can_sdo_ccs_segment_download,        //!< SDO segment download.
  can_sdo_ccs_init_download,           //!< Initiate SDO segment download.
  can_sdo_ccs_init_upload,             //!< Initiate SDO segment upload.
  can_sdo_ccs_segment_upload,          //!< SDO segment upload.
  can_sdo_ccs_abort,                   //!< Abort SDO communication.
} can_sdo_ccs_t;

/** \brief SDO transfer types
  */
typedef enum {
  can_sdo_transfer_expedited,          //!< SDO expedited transfer.
  can_sdo_transfer_segmented,          //!< SDO segmented transfer.
} can_sdo_transfer_t;

/** \brief Initialize CANopen SDO communication object
  * \param[in] cob The CANopen SDO communication object to be initialized.
  * \param[in] node_id The node identifier of the CANopen communication object.
  * \param[in] ccs The Client Command Specifier (CCS) of the SDO communication
  *   object.
  * \param[in] transfer The transfer type of the SDO communication object.
  * \param[in] index The object dictionary index of the value to be accessed.
  * \param[in] subindex The object dictionary subindex of the value to be
  *   accessed.
  * \param[in] data The data field of the SDO communication object.
  * \param[in] data_length The length of the data field in [byte]. According
  *   to the CANopen standard, the maximum permissible data length of an SDO
  *   transfer is 4 bytes. If the provided length exceeds this limit, the data
  *   will be truncated.
  * 
  * This initializer calls can_cob_init() with the data field constructed
  * according to the CANopen SDO protocol.
  */
void can_sdo_init_cob(
  can_cob_t* cob,
  unsigned char node_id,
  can_sdo_ccs_t ccs,
  can_sdo_transfer_t transfer,
  unsigned short index,
  unsigned char subindex,
  const unsigned char* data,
  size_t data_length);

/** \brief Retrieve client command specifier (CCS) of an SDO communication
 *    object
  * \param[in] cob The SDO communication object to retrieve the client
  *   command specifier (CCS) for.
  * \return The client command specifier (CCS) of the provided SDO
  *   communication object or -1 if the provided communication object is
  *   not an SDO communication object.
  */
can_sdo_ccs_t can_sdo_get_cob_ccs(
  const can_cob_t* cob);

/** \brief Retrieve transfer type of an SDO communication object
  * \param[in] cob The SDO communication object to retrieve the transfer
  *   type for.
  * \return The transfer type of the provided SDO communication object or
  *   -1 if the the provided communication object is not an SDO communication
  *   object.
  */
can_sdo_transfer_t can_sdo_get_cob_transfer(
  const can_cob_t* cob);

/** \brief Retrieve object dictionary index of an SDO communication object
  * \param[in] cob The SDO communication object to retrieve the object
  *   dictionary index for.
  * \return The object dictionary index of the provided SDO communication
  *   object or zero if the provided communication object is not an SDO
  *   communication object.
  */
unsigned short can_sdo_get_cob_index(
  const can_cob_t* cob);

/** \brief Retrieve object dictionary subindex of an SDO communication object
  * \param[in] cob The SDO communication object to retrieve the object
  *   dictionary subindex for.
  * \return The object dictionary subindex of the provided SDO communication
  *   object or zero if the provided communication object is not an SDO
  *   communication object.
  */
unsigned char can_sdo_get_cob_subindex(
  const can_cob_t* cob);

#endif
