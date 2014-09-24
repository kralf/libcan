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

#ifndef CAN_NMT_EC_H
#define CAN_NMT_EC_H

/** \file nmt_ec.h
  * \ingroup can
  * \brief CANopen Network Management Error Control (NMT-EC) protocol
  * \author Ralf Kaestner
  * 
  * The NMT-EC protocol is used to detect remote device bootups and error
  * conditions.
  */

#include "cob.h"

/** \brief Retrieve state of an NMT-EC communication object
  * \param[in] cob The NMT-EC communication object to retrieve the state
  *   for.
  * \return The state of the provided NMT-EC communication object or zero
  *   if the provided communication object is not an NMT-EC communication
  *   object.
  */
unsigned char can_nmt_ec_get_cob_state(
  const can_cob_t* cob);

#endif
