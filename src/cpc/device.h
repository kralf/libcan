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

#ifndef CAN_CPC_DEVICE_H
#define CAN_CPC_DEVICE_H

/** \file cpc/device.h
  * \ingroup can_cpc
  * \brief CAN-CPC communication device
  * \author Ralf Kaestner
  *
  * This file provides an interface to the CAN communication device back-end
  * implemented for CAN-CPC hardware.
  */

#include "can.h"

/** \name Parameters
  * \brief Predefined CAN-CPC device parameters
  */
//@{
#define CAN_CPC_DEVICE_PARAMETER_NAME             "cpc-dev"
#define CAN_CPC_DEVICE_PARAMETER_BIT_RATE         "cpc-bit-rate"
#define CAN_CPC_DEVICE_PARAMETER_QUANTA_PER_BIT   "cpc-quanta-per-bit"
#define CAN_CPC_DEVICE_PARAMETER_SAMPLING_POINT   "cpc-sampling-point"
#define CAN_CPC_DEVICE_PARAMETER_TIMEOUT          "cpc-timeout"
//@}

/** \name Constants
  * \brief Predefined CAN-CPC device constants
  */
//@{
#define CAN_CPC_DEVICE_CLOCK_FREQUENCY            16e6
#define CAN_CPC_DEVICE_SYNC_JUMP_WIDTH            1
#define CAN_CPC_DEVICE_TRIPLE_SAMPLING            0
//@}

/** \name Error Codes
  * \brief Predefined CAN-CPC device error codes
  */
//@{
#define CAN_CPC_DEVICE_ERROR_NONE                 0
//!< Success
#define CAN_CPC_DEVICE_ERROR_OPEN                 1
//!< Failed to open CAN-CPC device
#define CAN_CPC_DEVICE_ERROR_CLOSE                2
//!< Failed to close CAN-CPC device
#define CAN_CPC_DEVICE_ERROR_SETUP                3
//!< Failed to set CAN-CPC device parameters
#define CAN_CPC_DEVICE_ERROR_TIMEOUT              4
//!< CAN-CPC device timeout
#define CAN_CPC_DEVICE_ERROR_SEND                 5
//!< Failed to send to CAN-CPC device
#define CAN_CPC_DEVICE_ERROR_RECEIVE              6
//!< Failed to receive from CAN-CPC device
//@}

/** \brief Predefined CAN-CPC device error descriptions
  */
extern const char* can_cpc_device_errors[];

/** \brief CAN-CPC device structure
  */
typedef struct can_cpc_device_t {
  int handle;                   //!< Device handle.
  int fd;                       //!< File descriptor.
  char* name;                   //!< Device name.

  int bitrate;                  //!< Device bitrate in [kbit/s].
  int quanta_per_bit;           //!< Number of quanta per bit.
  double sampling_point;        //!< Sampling point in the range [0, 1].
  double timeout;               //!< Device select timeout in [s].

  can_message_t msg_received;   //!< The most recent message received.
  
  error_t error;                //!< The most recent device error.
} can_cpc_device_t;

/** \brief Open the CAN-CPC device with the specified name
  * \param[in] dev The CAN-CPC device to be opened.
  * \param[in] name The name of the CAN-CPC to be opened.
  * \return The resulting error code.
  */
int can_cpc_device_open(
  can_cpc_device_t* dev,
  const char* name);

/** \brief Close an open CAN-CPC device
  * \param[in] dev The open CAN-CPC device to be closed.
  * \return The resulting error code.
  */
int can_cpc_device_close(
  can_cpc_device_t* dev);

/** \brief Setup an already opened CAN-CPC device
  * \param[in] dev The open serial CAN-CPC to be set up.
  * \param[in] bitrate The device bitrate to be set in [kbit/s].
  * \param[in] quanta_per_bit The device's number of quanta per bit.
  * \param[in] sampling_point The sampling point in the range [0, 1].
  * \param[in] timeout The device select timeout to be set in [s].
  * \return The resulting error code.
  */
int can_cpc_device_setup(
  can_cpc_device_t* dev,
  int bitrate,
  int quanta_per_bit,
  double sampling_point,
  double timeout);

/** \brief Send CAN message over an open CAN-CPC device
  * \param[in] dev The open CAN-CPC device to send the message over.
  * \param[in] message The CAN message to be sent over the device.
  * \return The resulting error code.
  */
int can_cpc_device_send_message(
  can_cpc_device_t* dev,
  const can_message_t* message);

/** \brief Receive CAN message on an open CAN-CPC device
  * \param[in] dev The open CAN-CPC device to receive the message on.
  * \param[out] message The CAN message received on the device.
  * \return The resulting error code.
  */
int can_cpc_device_receive_message(
  can_cpc_device_t* dev,
  can_message_t* message);

#endif
