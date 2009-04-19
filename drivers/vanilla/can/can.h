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

#ifndef CAN_H
#define CAN_H

/** \file
  * \brief Generic CAN communication
  * Common commands used to communicate via the CAN protocol.
  * These methods are implemented by all CAN communication backends.
  */

#include <stdlib.h>

/** \brief Predefined CAN error codes
  */
#define CAN_ERROR_NONE                      0
#define CAN_ERROR_INIT                      1
#define CAN_ERROR_CLOSE                     2
#define CAN_ERROR_SEND                      3
#define CAN_ERROR_RECEIVE                   4

/** \brief Structure defining a CAN parameter
  */
typedef struct can_parameter_t {
  char name[64];                  //!< The CAN parameter name.
  char value[64];                 //!< The CAN parameter value.
} can_parameter_t, *can_parameter_p;

/** \brief Structure defining a CAN message
  */
typedef struct can_message_t {
  int id;                         //!< The CAN message identifier.
  unsigned char content[8];       //!< The actual CAN message content.
} can_message_t, *can_message_p;

/** \brief Structure defining a CAN device
  */
typedef struct can_device_t {
  void* comm_dev;                 //!< The CAN communication device.

  can_parameter_t* parameters;    //!< The CAN configuration parameters.

  ssize_t num_sent;               //!< The number of CAN messages sent.
  ssize_t num_received;           //!< The number of CAN messages read.
} can_device_t, *can_device_p;

/** \brief Initialize CAN communication by opening devices
  * \param[in] dev The CAN device to be initialized.
  * \param[in] parameters An array of CAN device parameters.
  * \param[in] num_parameters The number of CAN device parameters.
  * \return The resulting error code.
  */
int can_init(
  can_device_p dev,
  can_parameter_t parameters[],
  ssize_t num_parameters);

/** \brief Close CAN communication by closing devices
  * \param[in] dev The CAN device to be closed.
  * \return The resulting error code.
  */
int can_close(
  can_device_p dev);

/** \brief Send a CAN message
  * \param[in] dev The CAN device to be used for sending the message.
  * \param[in] message The CAN message to be sent.
  * \return The resulting error code.
  */
int can_send_message(
  can_device_p dev,
  can_message_p message);

/** \brief Synchronously receive a CAN message
  * \param[in] dev The CAN device to be used for receiving the message.
  * \param[in,out] message The sent CAN message that will be transformed
  *   into the CAN message received.
  * \return The resulting error code.
  */
int can_receive_message(
  can_device_p dev,
  can_message_p message);

#endif
