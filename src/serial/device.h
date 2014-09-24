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

#ifndef CAN_SERIAL_DEVICE_H
#define CAN_SERIAL_DEVICE_H

/** \file serial/device.h
  * \ingroup can_serial
  * \brief CAN-Serial communication device
  * \author Marc Rauer, Ralf Kaestner
  *
  * This file provides an interface to the CANopen communication device
  * back-end implemented for RS232 serial connections to maxon EPOS
  * controllers.
  */

#include <serial/serial.h>

#include "can.h"

/** \name Parameters
  * \brief Predefined CAN-Serial device parameters
  */
//@{
#define CAN_SERIAL_DEVICE_PARAMETER_NAME               "serial-dev"
#define CAN_SERIAL_DEVICE_PARAMETER_BAUD_RATE          "serial-baud-rate"
#define CAN_SERIAL_DEVICE_PARAMETER_DATA_BITS          "serial-data-bits"
#define CAN_SERIAL_DEVICE_PARAMETER_STOP_BITS          "serial-stop-bits"
#define CAN_SERIAL_DEVICE_PARAMETER_PARITY             "serial-parity"
#define CAN_SERIAL_DEVICE_PARAMETER_FLOW_CTRL          "serial-flow-ctrl"
#define CAN_SERIAL_DEVICE_PARAMETER_TIMEOUT            "serial-timeout"
//@}

/** \name Operation Codes
  * \brief Predefined CAN-Serial device operation codes
  */
//@{
#define CAN_SERIAL_DEVICE_OPCODE_RESPONSE              0x00
#define CAN_SERIAL_DEVICE_OPCODE_READ_OBJECT           0x10
#define CAN_SERIAL_DEVICE_OPCODE_WRITE_OBJECT          0x11
#define CAN_SERIAL_DEVICE_OPCODE_INIT_SEG_READ         0x12
#define CAN_SERIAL_DEVICE_OPCODE_INIT_SEG_WRITE        0x13
#define CAN_SERIAL_DEVICE_OPCODE_SEG_READ              0x14
#define CAN_SERIAL_DEVICE_OPCODE_SEG_WRITE             0x15
#define CAN_SERIAL_DEVICE_OPCODE_SEND_CAN_FRAME        0x20
#define CAN_SERIAL_DEVICE_OPCODE_REQUEST_CAN_FRAME     0x21
//@}

/** \name Acknowledges
  * \brief Predefined CAN-Serial device acknowledges
  */
//@{
#define CAN_SERIAL_DEVICE_ACK_OKAY                     0x4F
#define CAN_SERIAL_DEVICE_ACK_FAILED                   0x46
//@}

/** \name Error Codes
  * \brief Predefined CAN-Serial device error codes
  */
//@{
#define CAN_SERIAL_DEVICE_ERROR_NONE                   0
//!< Success
#define CAN_SERIAL_DEVICE_ERROR_CONVERT                1
//!< CAN-Serial conversion error
#define CAN_SERIAL_DEVICE_ERROR_SEND                   2
//!< Failed to send to CAN-Serial device
#define CAN_SERIAL_DEVICE_ERROR_RECEIVE                3
//!< Failed to receive from CAN-Serial device
#define CAN_SERIAL_DEVICE_ERROR_CRC                    4
//!< CAN-Serial checksum error
//@}

/** \brief Predefined CAN-Serial device error descriptions
  */
extern const char* can_serial_device_errors[];

/** \brief CAN-Serial device structure
  */
typedef struct can_serial_device_t {
  serial_device_t serial_dev;   //!< Serial device.
  
  error_t error;                //!< The most recent device error.
} can_serial_device_t;

/** \brief Convert CAN message into serial data
  * \param[in] dev The sending CAN-Serial device for which to convert the
  *   CAN message.
  * \param[in] connection_set The connection set of the sending CAN device
  *   for which to convert the CAN message.
  * \param[in] message The CAN message to be converted.
  * \param[out] data An array to store the converted serial data frame.
  * \return The number of bytes in the serial data frame to be sent or the
  *   negative error code.
  * 
  * This conversion method translates CAN messages into the EPOS RS232
  * serial protocol.
  */
int can_serial_device_from_message(
  can_serial_device_t* dev,
  const can_connection_set_t* connection_set,
  const can_message_t* message,
  unsigned char* data);

/** \brief Convert serial data to CAN message
  * \param[in] dev The receiving CAN-Serial device for which to convert
  *   the data.
  * \param[in] connection_set The connection set of the receiving CAN device
  *   for which to convert the data.
  * \param[in] data The serial data frame to be converted.
  * \param[in,out] message The converted CAN message.
  * \return The resulting error code.
  * 
  * This conversion method translates the EPOS RS232 serial protocol into
  * CANopen messages.
  */
int can_serial_device_to_message(
  can_serial_device_t* dev,
  const can_connection_set_t* connection_set,
  unsigned char* data,
  can_message_t* message);

/** \brief Send serial data to a CAN device
  * \param[in] dev The open CAN-Serial device to send data to.
  * \param[in] data An array containing the serial data frame to be sent
  *   via an EPOS RS232 connection.
  * \param[in] num The size of the serial data frame to be sent.
  * \return The number of bytes sent to the CAN-Serial device or the
  *   negative error code.
  */
int can_serial_device_send(
  can_serial_device_t* dev,
  unsigned char* data,
  size_t num);

/** \brief Receive serial data from a CAN device
  * \param[in] dev The open CAN-Serial device to reveice data from.
  * \param[out] data An array representing the serial data frame received
  *   via an EPOS RS232 connection.
  * \return The number of bytes received from the CAN-Serial device or the
  *   negative error code.
  */
int can_serial_device_receive(
  can_serial_device_t* dev,
  unsigned char* data);

#endif
