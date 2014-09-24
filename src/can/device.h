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

#ifndef CAN_DEVICE_H
#define CAN_DEVICE_H

/** \file device.h
  * \ingroup can
  * \brief Generic CANopen communication device
  * \author Ralf Kaestner
  * 
  * The generic CANopen device provides the interface for sending and
  * receiving CANopen messages over an opaque communication device. This
  * interface must be implemented by the different communication device
  * back-ends.
  */

#include <config/parser.h>

#include <error/error.h>

#include "connection_set.h"
#include "cob.h"
#include "message.h"

/** \name Error Codes
  * \brief Predefined CAN device error codes
  */
//@{
#define CAN_DEVICE_ERROR_NONE                     0
//!< Success
#define CAN_DEVICE_ERROR_CONFIG                   1
//!< CAN device configuration error
#define CAN_DEVICE_ERROR_OPEN                     2
//!< Failed to open CAN device
#define CAN_DEVICE_ERROR_SETUP                    3
//!< Failed to set CAN device parameters
#define CAN_DEVICE_ERROR_CLOSE                    4
//!< Failed to close CAN device
#define CAN_DEVICE_ERROR_PROTOCOL                 5
//!< CAN device protocol error
#define CAN_DEVICE_ERROR_CONNECTION               6
//!< CAN device connection error
#define CAN_DEVICE_ERROR_SEND                     7
//!< Failed to send message from CAN device
#define CAN_DEVICE_ERROR_RECEIVE                  8
//!< Failed to receive message on CAN device
#define CAN_DEVICE_ERROR_CONVERT                  9
//!< CAN conversion error 
//@}

/** \brief Predefined CAN device error descriptions
  */
extern const char* can_device_errors[];

/** \brief Structure defining a CAN device
  */
typedef struct can_device_t {
  void* comm_dev;             //!< The opaque CAN communication device.

  config_t config;            //!< The CAN device configuration parameters.
  can_connection_set_t
    connection_set;           //!< The CAN device connection set.

  ssize_t num_references;     //!< Number of references to this CAN device.
  ssize_t num_sent;           //!< The number of CAN messages sent.
  ssize_t num_received;       //!< The number of CAN messages read.
    
  error_t error;              //!< The most recent CAN device error.
} can_device_t;

/** \brief Predefined CAN device name
  */
extern const char* can_device_name;

/** \brief Predefined CAN device default configuration
  */
extern const config_default_t can_device_default_config;

/** \brief Initialize CAN device
  * \param[in] dev The CAN device to be initialized.
  * \return The resulting configuration error code.
  * 
  * The device will be initialized using default configuration parameters
  * and the default CANopen connnection set.
  */
void can_device_init(
  can_device_t* dev);

/** \brief Initialize CAN device from configuration
  * \param[in] dev The CAN device to be initialized.
  * \param[in] config The CAN device configuration parameters.
  * \return The resulting error code.
  * 
  * The device will be initialized using the provided configuration parameters
  * and the default CANopen connnection set.
  */
int can_device_init_config(
  can_device_t* dev,
  const config_t* config);

/** \brief Initialize CAN device by parsing command line arguments
  * \param[in] dev The CAN device to be initialized.
  * \param[in] parser The initialized configuration parser which will
  *   be used to parse the command line arguments into the CAN device
  *   configuration.
  * \param[in] option_group An optional name of the parser option group
  *   containing the CAN device configuration parameters. If null, the
  *   default name is chosen.
  * \param[in] argc The number of supplied command line arguments.
  * \param[in] argv The list of supplied command line arguments.
  * \param[in] exit The exit policy of the parser in case of an error
  *   or help request.
  * \return The resulting error code.
  * 
  * The device will be initialized using the provided command line arguments
  * and the default CANopen connnection set.
  */
int can_device_init_config_parse(
  can_device_t* dev,
  config_parser_t* parser,
  const char* option_group,
  int argc,
  char **argv,
  config_parser_exit_t exit);

/** \brief Destroy an existing CAN device
  * \param[in] dev The CAN device to be destroyed.
  */
void can_device_destroy(
  can_device_t* dev);

/** \brief Open CAN device communication
  * \note This method is implemented by the CAN communication backend.
  * \param[in] dev The initialized CAN device to be opened.
  * \return The resulting error code.
  */
int can_device_open(
  can_device_t* dev);

/** \brief Close CAN device communication
  * \note This method is implemented by the CAN communication backend.
  * \param[in] dev The opened CAN device to be closed.
  * \return The resulting error code.
  */
int can_device_close(
  can_device_t* dev);

/** \brief Send a CANopen communication object
  * \param[in] dev The CAN device to be used for sending the CANopen
  *   communication object.
  * \param[in] service The CANopen service to receive the sent communication
  *   object.
  * \param[in] cob The CANopen communication object to be sent.
  * \return The resulting error code.
  * 
  * This method attempts to transform the CANopen communication object to
  * be sent into a standard CAN message through can_device_cob_to_message()
  * and then passes this message to can_device_send_message().
  */
int can_device_send_cob(
  can_device_t* dev,
  can_service_t service,
  const can_cob_t* cob);

/** \brief Send a CAN message
  * \note This method is implemented by the CAN communication backend.
  * \param[in] dev The CAN device to be used for sending the message.
  * \param[in] message The CAN message to be sent.
  * \return The resulting error code.
  */
int can_device_send_message(
  can_device_t* dev,
  const can_message_t* message);

/** \brief Synchronously receive a CANopen communication object
  * \param[in] dev The CAN device to be used for receiving the CANopen
  *   communication object.
  * \param[in,out] service The CANopen service which sent the received
  *   communication object.
  * \param[in,out] cob The received CANopen communication object.
  * \return The resulting error code.
  * 
  * This method attempts to receive a standard CAN message using
  * can_device_receive_message() and then converts this message into a
  * CANopen communication object through can_device_message_to_cob().
  */
int can_device_receive_cob(
  can_device_t* dev,
  can_service_t* service,
  can_cob_t* cob);

/** \brief Synchronously receive a CAN message
  * \note This method is implemented by the CAN communication backend.
  * \param[in] dev The CAN device to be used for receiving the message.
  * \param[in,out] message The received CAN message.
  * \return The resulting error code.
  */
int can_device_receive_message(
  can_device_t* dev,
  can_message_t* message);

/** \brief Convert CANopen communication object into a standard CAN message
  * \param[in] dev The CAN device whose connection set determines the
  *   conversion of the CANopen communication object.
  * \param[in] service The CANopen service associated with the communication
  *   object.
  * \param[in] direction The direction of communication with the CANopen
  *   service.
  * \param[in] cob The CANopen communication object to be converted.
  * \param[in,out] message The CAN message resulting from the conversion.
  * \return The resulting error code.
  */
int can_device_cob_to_message(
  can_device_t* dev,
  can_service_t service,
  can_direction_t direction,
  const can_cob_t* cob,
  can_message_t* message);

/** \brief Convert CANopen communication object into a standard CAN message
  * \param[in] dev The CAN device whose connection set determines the
  *   conversion of the CAN message.
  * \param[in] message The CAN message to be converted.
  * \param[in,out] service The CANopen service associated with the
  *   communication object as specified by the connection set of the device.
  * \param[in,out] direction The direction of communication with the CANopen
  *   service as specified by the connection set of the device.
  * \param[in,out] cob The CANopen communication object resulting from the
  *   conversion.
  * \return The resulting error code.
  */
int can_device_message_to_cob(
  can_device_t* dev,
  const can_message_t* message,
  can_service_t* service,
  can_direction_t* direction,
  can_cob_t* cob);

#endif
