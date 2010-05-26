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

#include <tulibs/config.h>

/** Predefined CAN constants
  */
#define CAN_CONFIG_ARG_PREFIX               "can"

/** Predefined CAN error codes
  */
#define CAN_ERROR_NONE                      0
#define CAN_ERROR_OPEN                      1
#define CAN_ERROR_SETUP                     2
#define CAN_ERROR_CLOSE                     3
#define CAN_ERROR_SEND                      4
#define CAN_ERROR_RECEIVE                   5

/** \brief Predefined CAN error descriptions
  */
extern const char* can_errors[];

/** \brief Structure defining a CAN message
  */
typedef struct can_message_t {
  int id;                         //!< The CAN message identifier.

  unsigned char content[8];       //!< The actual CAN message content.
  ssize_t length;                 //!< The length of the CAN message.
} can_message_t, *can_message_p;

/** \brief Structure defining a CAN device
  */
typedef struct can_device_t {
  void* comm_dev;                 //!< The CAN communication device.

  config_t config;                //!< The CAN configuration parameters.

  ssize_t num_references;         //!< Number of references to this device.
  ssize_t num_sent;               //!< The number of CAN messages sent.
  ssize_t num_received;           //!< The number of CAN messages read.
} can_device_t, *can_device_p;

/** \brief Predefined CAN default configuration
  */
extern config_t can_default_config;

/** \brief Initialize CAN device
  * \param[in] dev The CAN device to be initialized.
  * \param[in] config The optional CAN device configuration parameters.
  *   Can be null.
  */
void can_init(
  can_device_p dev,
  config_p config);

/** \brief Initialize CAN device from command line arguments
  * \param[in] dev The CAN device to be initialized.
  * \param[in] argc The number of supplied command line arguments.
  * \param[in] argv The list of supplied command line arguments.
  * \param[in] prefix An optional argument prefix.
  */
void can_init_arg(
  can_device_p dev,
  int argc,
  char **argv,
  const char* prefix);

/** \brief Destroy an existing CAN device
  * \param[in] dev The CAN device to be destroyed.
  */
void can_destroy(
  can_device_p dev);

/** \brief Open CAN communication
  * \note This method is implemented by the CAN communication backend.
  * \param[in] dev The initialized CAN device to be opened.
  * \return The resulting error code.
  */
int can_open(
  can_device_p dev);

/** \brief Close CAN communication
  * \note This method is implemented by the CAN communication backend.
  * \param[in] dev The opened CAN device to be closed.
  * \return The resulting error code.
  */
int can_close(
  can_device_p dev);

/** \brief Send a CAN message
  * \note This method is implemented by the CAN communication backend.
  * \param[in] dev The CAN device to be used for sending the message.
  * \param[in] message The CAN message to be sent.
  * \return The resulting error code.
  */
int can_send_message(
  can_device_p dev,
  can_message_p message);

/** \brief Synchronously receive a CAN message
  * \note This method is implemented by the CAN communication backend.
  * \param[in] dev The CAN device to be used for receiving the message.
  * \param[in,out] message The sent CAN message that will be transformed
  *   into the CAN message received.
  * \return The resulting error code.
  */
int can_receive_message(
  can_device_p dev,
  can_message_p message);

#endif
