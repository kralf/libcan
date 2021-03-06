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

#ifndef CAN_SERIAL_H
#define CAN_SERIAL_H

/**
  *  \file can_serial.h
  *  \brief CAN communication over EPOS RS232
  *  \author Marc Rauer, Ralf Kaestner
  * 
  *  This layer provides low-level mechanisms for CANopen communication via
  *  RS232 serial connections to EPOS controllers.
  */

#include <serial/serial.h>

#include "can.h"

/** \name Parameters
  * \brief Predefined CAN-Serial parameters
  */
//@{
#define CAN_SERIAL_PARAMETER_DEVICE             "serial-dev"
#define CAN_SERIAL_PARAMETER_BAUD_RATE          "serial-baud-rate"
#define CAN_SERIAL_PARAMETER_DATA_BITS          "serial-data-bits"
#define CAN_SERIAL_PARAMETER_STOP_BITS          "serial-stop-bits"
#define CAN_SERIAL_PARAMETER_PARITY             "serial-parity"
#define CAN_SERIAL_PARAMETER_FLOW_CTRL          "serial-flow-ctrl"
#define CAN_SERIAL_PARAMETER_TIMEOUT            "serial-timeout"
//@}

/** \name Operation Codes
  * \brief Predefined CAN-Serial operation codes
  */
//@{
#define CAN_SERIAL_OPCODE_RESPONSE              0x00
#define CAN_SERIAL_OPCODE_READ                  0x10
#define CAN_SERIAL_OPCODE_WRITE                 0x11
#define CAN_SERIAL_OPCODE_READ_SEG_INIT         0x12
#define CAN_SERIAL_OPCODE_WRITE_SEG_INIT        0x13
#define CAN_SERIAL_OPCODE_READ_SEG              0x14
#define CAN_SERIAL_OPCODE_WRITE_SEG             0x15
//@}

/** \name Acknowledges
  * \brief Predefined CAN-Serial acknowledges
  */
//@{
#define CAN_SERIAL_ACK_OKAY                     0x4F
#define CAN_SERIAL_ACK_FAILED                   0x46
//@}

/** \name Error Codes
  * \brief Predefined CAN-Serial error codes
  */
//@{
#define CAN_SERIAL_ERROR_NONE                   0
//!< Success
#define CAN_SERIAL_ERROR_CONVERT                1
//!< CAN-Serial conversion error
#define CAN_SERIAL_ERROR_SEND                   2
//!< Failed to send to CAN-Serial device
#define CAN_SERIAL_ERROR_RECEIVE                3
//!< Failed to receive from CAN-Serial device
#define CAN_SERIAL_ERROR_CRC                    4
//!< CAN-Serial checksum error
//@}

/** \brief Predefined CAN-Serial error descriptions
  */
extern const char* can_serial_errors[];

/** \brief CAN-Serial device structure
  */
typedef struct can_serial_device_t {
  serial_device_t serial_dev;   //!< Serial device.
  
  error_t error;                //!< The most recent device error.
} can_serial_device_t;

/** \brief Convert a CANopen SDO message into serial data
  * \param[in] dev The sending CAN device for which to convert the message.
  * \param[in] message The CANopen SDO message to be converted.
  * \param[out] data An array to store the converted serial data frame.
  * \return The number of bytes in the serial data frame to be sent or the
  *   negative error code.
  * 
  * This conversion method translates CANopen SDO messages into the EPOS
  * RS232 serial protocol.
  */
int can_serial_device_from_epos(
  can_serial_device_t* dev,
  const can_message_t* message,
  unsigned char* data);

/** \brief Convert serial data to a CANopen SDO message
  * \param[in] dev The receiving CAN device for which to convert the message.
  * \param[in] data The serial data frame to be converted.
  * \param[in,out] message The converted CANopen SDO message.
  * \return The resulting error code.
  * 
  * This conversion method translates the EPOS RS232 serial protocol into
  * CANopen SDO messages.
  */
int can_serial_device_to_epos(
  can_serial_device_t* dev,
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

/** \brief Change the order of bytes in serial data frames
  * \param[in,out] data An array of bytes representing the serial data frame
  *   for which to change the order.
  * \param[in] num The number of bytes in the array.
  * \return The number of reordered bytes within the serial data frame.
  * 
  * The first two characters will be ignored, the following characters
  * will be reordered. This is necessary according to the EPOS Communication
  * Guide.
  */
size_t can_serial_change_byte_order(
  unsigned char* data,
  size_t num);

/** \brief Change the order of words in serial data frames
  * \param[in,out] data An array of words representing the serial data for
  *   which to change order.
  * \param[in] num The number of bytes in the word array.
  * \return The number of reordered bytes within the serial data frame.
  * 
  * The first two characters will be ignored, the following characters
  * will be reordered in groups of two. This is necessary according to the
  * EPOS Communication Guide.
  */
size_t can_serial_change_word_order(
  unsigned char* data,
  size_t num);

/** \brief Calculate a 16-bit CRC checksum using CRC-CCITT algorithm
  * \param[in] data An array of bytes representing the serial data frame.
  * \param[in] num The number of bytes in the data frame.
  * \param[out] crc_value An array of two bytes to store the CRC-word.
  * \return The number of words built from the array.
  * 
  * Calculation has to include all bytes in the data frame. Internally,
  * the array is transformed to an array of words in order to calculate the
  * CRC. The CRC word is then tranformed back to an array of characters.
  */
size_t can_serial_calc_crc(
  unsigned char* data,
  size_t num,
  unsigned char* crc_value);

/** \brief Implementation of the CRC-CCITT algorithm
  * \param[in] data An array of words containing the serial data frame.
  * \param[in] num The number of words in the data frame.
  * \return The calculated CRC-value.
  */
unsigned short can_serial_crc_alg(
  unsigned short* data,
  size_t num);

#endif
