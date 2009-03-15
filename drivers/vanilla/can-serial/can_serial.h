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
  *  \brief EPOS-communication over RS232
  *  \author Marc Rauer, Ralf Kaestner
  *  This layer provides low-level mechanisms for communicating with EPOS
  *  motion controllers over a serial connection.
  */

#include <can.h>

#define CAN_SERIAL_OKAY 		          0x4F
#define CAN_SERIAL_FAILED		          0x46
#define CAN_SERIAL_RESPONSE	          0x00

#define CAN_SERIAL_BAUDRATE           38400
#define CAN_SERIAL_DATABITS           8
#define CAN_SERIAL_STOPBITS           1
#define CAN_SERIAL_PARITY             none
#define CAN_SERIAL_TIMEOUT            0.05

#define CAN_SERIAL_MAX_ERROR          31

/** \brief Error structure
  */
typedef struct {
  long int code;  //!< The error code.
  char* msg;      //!< The error message.
} can_serial_error_t;

/** Convert the msg-array defined in libepos.c to the data-array defined
  * in libserial.c.
  *	\note	This conversion is necessary, because msg-array in libepos.c relies
  *   strongly on CAN protocol.
  * \param[in] can_id Node-ID
  * \param[in] msg Array of char containig the msg-array
  * \param[out] data Array of char to store converted data-array
  *	\return Number of bytes in the data frame to send
  */
int can_serial_from_epos(
  int can_id,
  unsigned char* msg,
  unsigned char* data);

/** Convert the data-array defined in libserial.c to the cpcmsg-structure
  * defined in libepos.c. This function also calls read_SDO_msg_handler()
  * in libepos.c, which handles the incoming messages.
  *	\note This conversion is necessary, because cpcmsg-structure in libepos.c
  *   assumes that the message was received by CAN protocol.
  * \param[in] data_send Array of char representing the data frame sent to EPOS
  * \param[out] data_recv Array of char representing the data frame received
  *   from EPOS
  */
void can_serial_to_epos(
  unsigned char* data_send,
  unsigned char* data_recv);

/** Open a device using termios for configuration.
  *	\warning Function exits program, if there is no/wrong device!
  * \param[in] dev_name Name of the device (/dev/ttyS0 for first serial port)
  * \return 0: Device successfully opened, < 0: Error opening device
  */
int can_serial_open(
  const char* dev_name);

/** Close an open device.
  * \return 0: Device successfully closed, < 0: Error closing device
  */
int can_serial_close(void);

/** Send data frame to EPOS
  * \param[in] data Array containing the data frame to be sent
  * \param[in] num Size of the data frame to be sent
  * \return Number of bytes sent
  */
ssize_t can_serial_send(
  unsigned char* data,
  ssize_t num);

/** Receive data frame to EPOS
  * \param[out] data Array representing the received data frame
  * \return Number of bytes received
  */
ssize_t can_serial_receive(
  unsigned char* data);

/** Change order of databytes in the data frame. The first two char will be
  * ignored, the following chars will be changed by each other.
  *	\note Necessary according EPOS Communication guide.
  * \param[in,out] data Array of bytes for which to change order
  * \param[in] num Number of bytes in the array (length)
  *	\return	Number of changed bytes within the data frame
  */
ssize_t can_serial_change_byte_order(
  unsigned char* data,
  ssize_t num);

/** Change order of datawords (two chars) in the data frame. The first two
  * chars will be ignored, the following chars will be grouped by two and
  * these groups changed by each other.
  *	\note	Necessary according EPOS Communication guide.
  * \param[in,out] data Array of char for which to change order
  * \param[in] num Number of bytes in the array (length)
  *	\return	Number of changed bytes within the data frame.
  */
ssize_t can_serial_change_word_order(
  unsigned char* data,
  ssize_t num);

/** Calculate 16-bit CRC checksum using CRC-CCITT algorithm.
  *	\note Calculation has to include all bytes in the data frame. Internally
  *   the array of char is transformed to an array of words to calculate the
  *   CRC. The CRC value as a word is	then tranformed back to an array of char.
  * \param[in] data Array of bytes representing the data frame
  * \param[in] num Number of bytes in the data frame
  * \param[out] crc_value Array of two bytes to store the CRC-word
  *	\return	Number of words built from the array.
  */
ssize_t can_serial_calc_crc(
  unsigned char* data,
  ssize_t num,
  unsigned char* crc_value);

/** Implementation of the CRC-CCITT algorithm.
  * \param[in] data Array of words containing the data frame
  * \param[in] num Number of words in the data frame
  *	\return	Calculated CRC-value.
  */
unsigned short can_serial_crc_alg(
  unsigned short* data,
  ssize_t num);

#endif
