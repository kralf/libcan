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

#ifndef CAN_EMCY_H
#define CAN_EMCY_H

/** \file emcy.h
  * \ingroup can
  * \brief CANopen Emergency Object (EMCY) protocol
  * \author Ralf Kaestner
  * 
  * The EMCY protocol specifies the transmission of emergency telegrams
  * upon error events.
  */

#include "cob.h"

/** \brief Retrieve error code of an EMCY communication object
  * \param[in] cob The EMCY communication object to retrieve the error
  *   code for.
  * \return The error code of the provided EMCY communication object or
  *   zero if the provided communication object is not an EMCY communication
  *   object.
  */
unsigned short can_emcy_get_cob_error_code(
  const can_cob_t* cob);

/** \brief Retrieve error register of an EMCY communication object
  * \param[in] cob The EMCY communication object to retrieve the error
  *   register for.
  * \return The error register of the provided EMCY communication object or
  *   zero if the provided communication object is not an EMCY communication
  *   object.
  */
unsigned char can_emcy_get_cob_error_register(
  const can_cob_t* cob);

/** \brief Retrieve vendor code of an EMCY communication object
  * \param[in] cob The EMCY communication object to retrieve the vendor
  *   code for.
  * \return The vendor code of the provided EMCY communication object,
  *   an array of 5 bytes or null if the provided communication object is
  *   not an EMCY communication object.
  */
const unsigned char* can_emcy_get_cob_vendor_code(
  const can_cob_t* object);

#endif
