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

/** \file can.h
  * \brief Generic CAN communication
  * 
  * Common commands used to communicate via the CANopen protocol.
  * These methods are implemented by all CAN communication back-ends.
  */

#include <config/parser.h>

#include <error/error.h>

/** \brief Predefined CAN configuration parser option group
  */
#define CAN_CONFIG_PARSER_OPTION_GROUP            "can"

/** \name Node Identifiers
  * \brief Predefined node identifiers as specified by the CANopen standard
  */
//@{
#define CAN_NODE_ID_MAX                           0x007F
#define CAN_NODE_ID_BROADCAST                     0x0000
//@}

/** \name SDO Communication Object Identifiers
  * \brief Predefined SDO object identifiers as specified by CANopen
  */
//@{
#define CAN_COB_ID_SDO_SEND                       0x0600
#define CAN_COB_ID_SDO_RECEIVE                    0x0580
#define CAN_COB_ID_SDO_EMERGENCY                  0x0080
//@}

/** \name SDO Commands
  * \brief Predefined SDO commands as specified by the CANopen standard
  */
//@{
#define CAN_CMD_SDO_WRITE_SEND_1_BYTE             0x2F
#define CAN_CMD_SDO_WRITE_SEND_2_BYTE             0x2B
#define CAN_CMD_SDO_WRITE_SEND_4_BYTE             0x23
#define CAN_CMD_SDO_WRITE_SEND_UNDEFINED          0x22
#define CAN_CMD_SDO_WRITE_SEND_N_BYTE_INIT        0x21
#define CAN_CMD_SDO_WRITE_SEND_N_BYTE_SEGMENT     0x00
#define CAN_CMD_SDO_WRITE_RECEIVE                 0x60

#define CAN_CMD_SDO_READ_RECEIVE_1_BYTE           0x4F
#define CAN_CMD_SDO_READ_RECEIVE_2_BYTE           0x4B
#define CAN_CMD_SDO_READ_RECEIVE_4_BYTE           0x43
#define CAN_CMD_SDO_READ_RECEIVE_UNDEFINED        0x42
#define CAN_CMD_SDO_READ_RECEIVE_N_BYTE_INIT      0x41
#define CAN_CMD_SDO_READ_RECEIVE_N_BYTE_SEGMENT   0x60
#define CAN_CMD_SDO_READ_SEND                     0x40

#define CAN_CMD_SDO_ABORT                         0xC0
//@}

/** \name Error Codes
  * \brief Predefined CAN error codes
  */
//@{
#define CAN_ERROR_NONE                            0
//!< Success
#define CAN_ERROR_CONFIG                          1
//!< CAN configuration error
#define CAN_ERROR_OPEN                            2
//!< Failed to open CAN device
#define CAN_ERROR_SETUP                           3
//!< Failed to set CAN device parameters
#define CAN_ERROR_CLOSE                           4
//!< Failed to close CAN device
#define CAN_ERROR_SEND                            5
//!< Failed to send CAN message
#define CAN_ERROR_RECEIVE                         6
//!< Failed to receive CAN message
//@}

/** \brief Predefined CAN error descriptions
  */
extern const char* can_errors[];

/** \brief Structure defining a CANopen SDO message
  */
typedef struct can_message_t {
  int id;                     //!< The CAN message identifier.

  unsigned char content[8];   //!< The actual CAN message content.
  size_t length;              //!< The length of the CAN message.
} can_message_t;

/** \brief Structure defining a CAN device
  */
typedef struct can_device_t {
  void* comm_dev;             //!< The opaque CAN communication device.

  config_t config;            //!< The CAN configuration parameters.

  ssize_t num_references;     //!< Number of references to this device.
  ssize_t num_sent;           //!< The number of CAN messages sent.
  ssize_t num_received;       //!< The number of CAN messages read.
    
  error_t error;              //!< The most recent CAN device error.
} can_device_t;

/** \brief Predefined CAN device name
  */
extern const char* can_device_name;

/** \brief Predefined CAN default configuration
  */
extern const config_default_t can_default_config;

/** \brief Initialize CAN device
  * \note The device will be initialized using default configuration
  *   parameters.
  * \param[in] dev The CAN device to be initialized.
  * \return The resulting configuration error code.
  */
void can_device_init(
  can_device_t* dev);

/** \brief Initialize CAN device from configuration
  * \param[in] dev The CAN device to be initialized.
  * \param[in] config The CAN device configuration parameters.
  * \return The resulting error code.
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
void can_destroy(
  can_device_t* dev);

/** \brief Open CAN communication
  * \note This method is implemented by the CAN communication backend.
  * \param[in] dev The initialized CAN device to be opened.
  * \return The resulting error code.
  */
int can_device_open(
  can_device_t* dev);

/** \brief Close CAN communication
  * \note This method is implemented by the CAN communication backend.
  * \param[in] dev The opened CAN device to be closed.
  * \return The resulting error code.
  */
int can_device_close(
  can_device_t* dev);

/** \brief Send a CANopen SDO message
  * \note This method is implemented by the CAN communication backend.
  * \param[in] dev The CAN device to be used for sending the message.
  * \param[in] message The CANopen SDO message to be sent.
  * \return The resulting error code.
  */
int can_device_send_message(
  can_device_t* dev,
  const can_message_t* message);

/** \brief Synchronously receive a CANopen SDO message
  * \note This method is implemented by the CAN communication backend.
  * \param[in] dev The CAN device to be used for receiving the message.
  * \param[in,out] message The sent CAN message that will be transformed
  *   into the CANopen SDO message received.
  * \return The resulting error code.
  */
int can_device_receive_message(
  can_device_t* dev,
  can_message_t* message);

#endif
