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

#ifndef CAN_USB_H
#define CAN_USB_H

/**
  *  \file can_usb.h
  *  \brief CAN communication over EPOS USB
  *  \author Ralf Kaestner
  * 
  *  This layer provides low-level mechanisms for CANopen communication via
  *  FTDI serial connections over USB links to EPOS controllers.
  */

#include <ftdi/ftdi.h>

#include "can.h"

/** \name Parameters
  * \brief Predefined CAN-USB parameters
  */
//@{
#define CAN_USB_PARAMETER_DEVICE           "usb-dev"
#define CAN_USB_PARAMETER_INTERFACE        "usb-serial-interface"
#define CAN_USB_PARAMETER_BAUD_RATE        "usb-serial-baud-rate"
#define CAN_USB_PARAMETER_DATA_BITS        "usb-serial-databits"
#define CAN_USB_PARAMETER_STOP_BITS        "usb-serial-stopbits"
#define CAN_USB_PARAMETER_PARITY           "usb-serial-parity"
#define CAN_USB_PARAMETER_FLOW_CTRL        "usb-serial-flow-ctrl"
#define CAN_USB_PARAMETER_BREAK            "usb-serial-break"
#define CAN_USB_PARAMETER_TIMEOUT          "usb-serial-timeout"
#define CAN_USB_PARAMETER_LATENCY          "usb-serial-latency"
//@}

/** \name Operation Codes
  * \brief Predefined CAN-USB operation codes
  */
//@{
#define CAN_USB_OPCODE_RESPONSE            0x00
#define CAN_USB_OPCODE_READ                0x10
#define CAN_USB_OPCODE_WRITE               0x11
//@}

/** \name Synchronization Characters
  * \brief Predefined CAN-USB frame synchronization characters
  */
//@{
#define CAN_USB_SYNC_DLE                   0x90
#define CAN_USB_SYNC_STX                   0x02
//@}

/** \name Error Codes
  * \brief Predefined CAN-USB error codes
  */
//@{
#define CAN_USB_ERROR_NONE                 0
//!< Success
#define CAN_USB_ERROR_DEVICE               1
//!< No such CAN-USB device
#define CAN_USB_ERROR_CONVERT              2
//!< CAN-USB conversion error
#define CAN_USB_ERROR_SEND                 3
//!< Failed to send to CAN-USB device
#define CAN_USB_ERROR_RECEIVE              4
//!< Failed to receive from CAN-USB device
#define CAN_USB_ERROR_CRC                  5
//!< CAN-USB checksum error
//@}

/** \brief Predefined CAN-USB error descriptions
  */
extern const char* can_usb_errors[];

/** \brief CAN-USB device structure
  */
typedef struct can_usb_device_t {
  ftdi_device_t* ftdi_dev;      //!< FTDI device.
  
  error_t error;                //!< The most recent device error.
} can_usb_device_t;

/** \brief Convert a CANopen SDO message into USB data
  * \param[in] dev The sending CAN device for which to convert the message.
  * \param[in] message The CANopen SDO message to be converted.
  * \param[out] data An array to store the converted USB data frame.
  * \return The number of bytes in the USB data frame to be sent or the
  *   negative error code.
  * 
  * This conversion method translates CANopen SDO messages into the EPOS
  * USB protocol.
  */
int can_usb_device_from_epos(
  can_usb_device_t* dev,
  const can_message_t* message,
  unsigned char* data);

/** \brief Convert USB data to a CANopen SDO message
  * \param[in] dev The receiving CAN device for which to convert the message.
  * \param[in] data The USB data frame to be converted.
  * \param[in,out] message The converted CANopen SDO message.
  * \return The resulting negative error code.
  * 
  * This conversion method translates the EPOS USB protocol into CANopen
  * SDO messages.
  */
int can_usb_device_to_epos(
  can_usb_device_t* dev,
  unsigned char* data,
  can_message_t* message);

/** \brief Send USB data to a CAN device
  * \param[in] dev The open CAN-USB device to send data to.
  * \param[in] data An array containing the USB data frame to be sent
  *   via an EPOS USB connection.
  * \param[in] num The size of the USB data frame to be sent.
  * \return The number of bytes sent to the CAN-USB device or the
  *   negative error code.
  */
int can_usb_device_send(
  can_usb_device_t* dev,
  unsigned char* data,
  size_t num);

/** \brief Receive USB data from a CAN device
  * \param[in] dev The open CAN-USB device to reveice data from.
  * \param[out] data An array representing the USB data frame received
  *   via an EPOS USB connection.
  * \return The number of bytes received from the CAN-USB device or the
  *   negative error code.
  */
int can_usb_device_receive(
  can_usb_device_t* dev,
  unsigned char* data);

/** \brief Change the order of bytes in USB data frames
  * \param[in,out] data An array of bytes representing the USB data frame
  *   for which to change the order.
  * \param[in] num The number of bytes in the array.
  * \return The number of reordered bytes within the USB data frame.
  * 
  * The first two characters will be ignored, the following characters
  * will be reordered. This is necessary according to the EPOS Communication
  * Guide.
  */
size_t can_usb_change_byte_order(
  unsigned char* data,
  size_t num);

/** \brief Change the order of words in USB data frames
  * \param[in,out] data An array of words representing the USB data for
  *   which to change order.
  * \param[in] num The number of bytes in the word array.
  * \return The number of reordered bytes within the USB data frame.
  * 
  * The first two characters will be ignored, the following characters
  * will be reordered in groups of two. This is necessary according to the
  * EPOS Communication guide.
  */
size_t can_usb_change_word_order(
  unsigned char* data,
  size_t num);

/** \brief Calculate a 16-bit CRC checksum using CRC-CCITT algorithm
  * \param[in] data An array of bytes representing the USB data frame.
  * \param[in] num The number of bytes in the data frame.
  * \param[out] crc_value An array of two bytes to store the CRC-word.
  * \return The number of words built from the array.
  * 
  * Calculation has to include all bytes in the data frame. Internally,
  * the array is transformed to an array of words in order to calculate the
  * CRC. The CRC word is then tranformed back to an array of characters.
  */
size_t can_usb_calc_crc(
  unsigned char* data,
  size_t num,
  unsigned char* crc_value);

/** \brief Implementation of the CRC-CCITT algorithm
  * \param[in] data An array of words containing the USB data frame.
  * \param[in] num The number of words in the data frame.
  * \return The calculated CRC-value.
  */
unsigned short can_usb_crc_alg(
  unsigned short* data,
  size_t num);

#endif
