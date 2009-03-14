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


/**
  * \file can_serial.c
  * \author Marc Rauer, Ralf Kaestner
  * \note
  *   message-structure within libserial:
  *
  *   message[0] OpCode	CG p.15
  *   message[1] length of data-1!
  *
  *   message[2] high-byte Index (data1)
  *   message[3] low-byte Index (data1)
  *
  *   message[4] node-id (data2)
  *   message[5] subindex (data2)
  *
  *   Optional databytes:
  *   message[6] high-byte additional data3
  *   message[7] low-byte additional data3
  *   message[8] high-byte additional data4
  *   message[9] low-byte additional data4
  *
  *   message[10] high-byte CRC
  *   message[11] low-byte CRC
  */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>

#undef ASL_DEBUG
#undef DEBUG

#include <pdebug.h>
#include <epos.h>
#include <serial.h>

#include "can_serial.h"

static serial_device_t dev;
static can_message_t message;

can_serial_error_t can_serial_error[CAN_SERIAL_MAX_ERROR] = {
  { 0x00000000, "No error." },
  { 0x06020000, "Object does not exist in the object dicionary." },
  { 0x06090011, "Sub-index does not exist." },
  { 0x05040001, "Client/server command specifier not valid or unknown." },
  { 0x05030000, "Toggle bit not alternated." },
  { 0x05040000, "SDO protocol timed out." },
  { 0x05040005, "Out of memory" },
  { 0x06010000, "Unsupported access to an object." },
  { 0x06010001, "Attempt to read a write only object."  },
  { 0x06010002, "Attempt to write a read only object."  },
  { 0x06040041, "Object cannot be mapped to the PDO." },
  { 0x06040042, "The number and length of the objects to be mapped would exceed PDO length." },
  { 0x06040043, "General parameter incompatibility reason." },
  { 0x06040047, "General internal incompatibility reason." },
  { 0x06060000, "Access failed due to an hardware error." },
  { 0x06070010, "Data type does not match, length of service parameter does not match." },
  { 0x06070012, "Data type does not match, length of service parameter too high." },
  { 0x06070013, "Data type does not match, length of service parameter too low." },
  { 0x06090030, "Value range of parameter exceeded (only for write access)." },
  { 0x06090031, "Value of parameter written too high." },
  { 0x06090032, "Value of parameter written too low." },
  { 0x06090036, "Maximum value is less than minimum value." },
  { 0x08000000, "General error." },
  { 0x08000020, "Data cannot be transferred or stored to the application." },
  { 0x08000021, "Data cannot be transferred or stored to the application because of local control." },
  { 0x08000022, "Data cannot be transferred or stored to the application because of the present device state."},
  { 0x0F00FFC0, "The device is in wrong NMT state." },
  { 0x0F00FFBF, "The RS232 command is illegal." },
  { 0x0F00FFBE, "The password is not correct." },
  { 0x0F00FFBC, "The device is not in service mode." },
  { 0x0F00FFB9, "Error Node-ID." }
};

void can_init(const char* dev_name) {
  if (!can_serial_open(dev_name))
    bzero(&message, sizeof(can_message_t));
}

void can_close() {
	can_serial_close();
}

void can_send_message(can_message_t* message) {
	unsigned char data_send[64];
	unsigned char data_recv[64];
	ssize_t num;

	num = can_serial_from_epos(message->id, message->content, data_send);

  can_serial_send(data_send, num);
  num = can_serial_receive(data_recv);

	can_serial_to_epos(data_send, data_recv);
}

void can_read_message() {
	// Nothing to do here
}

int can_serial_from_epos(int can_id, unsigned char* msg, unsigned char* data) {
	ssize_t num_sent = 0;

	switch (msg[0]) {
		case EPOS_WRITE_1_BYTE:
      data[0] = 0x11;	/* opcode write 4 or less bytes */
      data[1] = 0x02;	/* len-1 = 2 -> 3 data-words */
      data[2] = msg[2];	/* high-byte index */
      data[3] = msg[1]; /* low-byte index */
      data[4] = (can_id & 0x000000FF); /* node-id */
      data[5] = msg[3];	/* subindex */
      data[6] = msg[5];	/* high-byte of low-word data */
      data[7] = msg[4];	/* low-byte of low-word data */
      data[8] = 0x00;	/* reserved for adding CRC later */
      data[9] = 0x00;	/* reserved for adding CRC later */
      num_sent = 10; /* 8 data-bytes + 2 CRC */
      break;
		case EPOS_WRITE_2_BYTE:
      data[0] = 0x11;	/* opcode write 4 or less bytes */
      data[1] = 0x02;	/* len-1 = 2 -> 3 data-words */
      data[2] = msg[2];	/* high-byte index */
      data[3] = msg[1]; /* low-byte index */
      data[4] = (can_id & 0x000000FF); /* node-id */
      data[5] = msg[3];	/* subindex */
      data[6] = msg[5];	/* high-byte of low-word data */
      data[7] = msg[4];	/* low-byte of low-word data */
      data[8] = 0x00;	/* reserved for adding CRC later */
      data[9] = 0x00;	/* reserved for adding CRC later */
      num_sent = 10; /* 8 data-bytes + 2 CRC */
      break;
		case EPOS_WRITE_4_BYTE:
      data[0] = 0x11; /* opcode write 4 or less bytes */
      data[1] = 0x03; /* len-1 = 3 -> 4 data-words */
      data[2] = msg[2]; /* high-byte index */
      data[3] = msg[1]; /* low-byte index */
      data[4] = (can_id & 0x000000FF);	/* node-id */
      data[5] = msg[3]; /* subindex */
      /* Change of order of data-words necessary */
      data[6] = msg[5]; /* high-byte of high-word data */
      data[7] = msg[4]; /* low-byte of high-word data */
      data[8] = msg[7]; /* high-byte of low-word data */
      data[9] = msg[6]; /* low-byte of low-word data */
      data[10] = 0x00; /* reserved for adding CRC later */
      data[11] = 0x00; /* reserved for adding CRC later */
      num_sent = 12; /* 10 data-bytes + 2 CRC */
      break;
		case EPOS_READ:
      data[0] = 0x10; /* opcode read 4 or less bytes */
      data[1] = 0x01; /* len-1 = 1 -> 2 data-words */
      data[2] = msg[2]; /* high-byte index */
      data[3] = msg[1]; /* low-byte index */
      data[4] = (can_id & 0x000000FF);	/* node-id */
      data[5] = msg[3];	/* subindex */
      data[6] = 0x00;	/* reserved for adding CRC later */
      data[7] = 0x00;	/* reserved for adding CRC later */
      num_sent = 8; /* 6 data-bytes + 2 CRC */
      break;
		default:
      PDEBUG_SNIP("can_serial_from_epos: msg[0] = 0x%02X\n", msg[0]);
      num_sent = -1;
      break;
	}

	return num_sent;
}

void can_serial_to_epos(unsigned char* data_send, unsigned char* data_recv) {
	long int error = 0;
	int i;

	message.id = (data_send[5] & 0x000000FF)+0x580;

	if ((data_recv[2] == 0x00) && (data_recv[3] == 0x00) &&
    (data_recv[4] == 0x00) && (data_recv[5] == 0x00)) {
		message.content[0] = 0x00; /* no error */
		message.content[1] = data_send[2]; /* high-byte index */
		message.content[2] = data_send[3]; /* low-byte index */
		message.content[3] = data_send[4]; /* subindex */
		message.content[7] = data_recv[6]; /* high-byte of high-word data */
		message.content[6] = data_recv[7]; /* low-byte of high-word data */
		message.content[5] = data_recv[8]; /* high-byte of low-word data */
		message.content[4] = data_recv[9]; /* low-byte of low-word data */
	}
	else {
    // Serial error occured see "EPOS Communication Guide" page 18
		error = (long int)((data_recv[4] << 24)+(data_recv[5] << 16)+
      (data_recv[2] << 8)+data_recv[3]);

		for (i = 0; i < CAN_SERIAL_MAX_ERROR; i++) {
			if (can_serial_error[i].code == error) {
				fprintf(stderr, "can_serial_to_epos: %s\n", can_serial_error[i].msg);
				epos_read.node[message.id-0x581].error.comm.code =
          can_serial_error[i].code;
				epos_read.node[message.id-0x581].error.comm.msg =
          can_serial_error[i].msg;
			}
		}
		return;
	}

	can_read_message_handler(&message);
}

int can_serial_open(const char* dev_name) {
  if (!serial_open(&dev, dev_name))
    return serial_setup(&dev, CAN_SERIAL_BAUDRATE, CAN_SERIAL_DATABITS,
    CAN_SERIAL_STOPBITS, CAN_SERIAL_PARITY, CAN_SERIAL_TIMEOUT);
  else
    return -1;
}

int can_serial_close() {
  return serial_close(&dev);
}

ssize_t can_serial_send(unsigned char* data, ssize_t num) {
	int i;
	unsigned char buffer;
	unsigned char crc_value[2];
	int result = 0;

  /* 1. Calculate crc and add to data */
	PDEBUG_SNIP("Calculate crc... ");
	can_serial_calc_crc(data, num, crc_value);
	PDEBUG_SNIP("done.\n");

	PDEBUG_SNIP("Write crc to data frame... ");
	data[num-2] = crc_value[0];
	data[num-1] = crc_value[1];
	PDEBUG_SNIP("done.\n");

  /* 2. Change order of bytes to send */
	PDEBUG_SNIP("Change data byte order... ");
	can_serial_change_byte_order(data, num);
	PDEBUG_SNIP("done.\n");

  /* 3. Send Op-code and wait for readyAck */
  PDEBUG_SNIP("Send opcode 0x%02X... ", data[0]);
  if (serial_write(&dev, data, 1) != 1)
    return -1;
  PDEBUG_SNIP("done.\n");

  PDEBUG_SNIP("Wait for ready ack... ");
  result = serial_read(&dev, &buffer, 1);
  if ((result == 1) && (buffer == CAN_SERIAL_OKAY))
    PDEBUG_SNIP("done, received 'OKAY'.\n");
  else if ((result == 1) && (buffer == CAN_SERIAL_FAILED)) {
    PDEBUG_SNIP("done, received 'FAILED'.\n");
    return -1;
  }
  else if (result == 1) {
    PDEBUG_SNIP("ERROR: unkown response: 0x%02X\n", buffer);
    return -1;
  }
  else if (result == 0) {
    PDEBUG_SNIP("ERROR: no response\n");
    return -1;
	}
  
	PDEBUG_SNIP("Send rest of data frame... ");
  if (serial_write(&dev, &data[1], num-1) != num-1)
		return -1;
	PDEBUG_SNIP("done.\n");

  PDEBUG_SNIP("Wait for end ack... ");
  result = serial_read(&dev, &buffer, 1);
  if ((result == 1) && (buffer == CAN_SERIAL_OKAY))
    PDEBUG_SNIP("done, received 'OKAY'.\n");
  else if ((result == 1) && (buffer == CAN_SERIAL_FAILED)) {
    PDEBUG_SNIP("done, received 'FAILED'.\n");
    return -1;
  }
  else if (result == 1) {
    PDEBUG_SNIP("ERROR: unkown response: 0x%02X\n", buffer);
    return -1;
  }
  else if (result == 0) {
    PDEBUG_SNIP("ERROR: no response\n");
    return -1;
  }

	return num;
}

ssize_t can_serial_receive(unsigned char* data) {
	int i, result;
	unsigned char buffer, crc_value[2];
	ssize_t num_recv = 0, num_expected = 0;

  /* 1. Wait for response-opcode */
  PDEBUG_SNIP("Wait for response opcode... ");
  result = serial_read(&dev, &buffer, 1);
  if ((result == 1) && (buffer == CAN_SERIAL_RESPONSE)) {
    PDEBUG_SNIP("done.\n");
    data[0] = CAN_SERIAL_RESPONSE;
  }
  else if (result == 1) {
    PDEBUG_SNIP("ERROR: unkown response: 0x%02X\n", buffer);
    return -1;
  }
  else if (result == 0) {
    PDEBUG_SNIP("ERROR: no response\n");
    return -1;
  }

  /* 2. Send readyAck and wait for dataframe length (len-1) */
	PDEBUG_SNIP("Send ready ack... ");
  buffer = CAN_SERIAL_OKAY;
	if (serial_write(&dev, &buffer, 1) != 1)
		return -1;
	PDEBUG_SNIP("done.\n");

  PDEBUG_SNIP("Wait for data frame length... ");
  result = serial_read(&dev, &buffer, 1);
  if (result == 1) {
    PDEBUG_SNIP("done.\n");
    data[1] = buffer;
  }
  else if (result == 0) {
    PDEBUG_SNIP("ERROR: no response\n");
    return -1;
  }

  /* 3. Read rest of dataframe (datawords + crcword) */
	num_expected = (data[1]+2)*sizeof(unsigned short);
	PDEBUG_SNIP("Wait for %zd more bytes... ", num_expected);
	for (i = 0; i < num_expected; ++i) {
		if (serial_read(&dev, &buffer, 1) == 1)
		  data[i+2] = buffer;
    else
      break;
	}
	if (i != num_expected) {
		PDEBUG_SNIP("ERROR: only %d bytes received\n", i);
		return -1;
	}
	PDEBUG_SNIP("done.\n");
	num_recv = i+2;

  /* 4. change order of received data-bytes */
	PDEBUG_SNIP("Change byte order of received data... ");
	can_serial_change_byte_order(data, num_recv);
	PDEBUG_SNIP("done.\n");

  /* 5. check crc and send Ack to EPOS*/
	PDEBUG_SNIP("Validating checksum... ");
	can_serial_calc_crc(data, num_recv, crc_value);
	if ((crc_value[0] == 0x00) && (crc_value[1] == 0x00)) {
		PDEBUG_SNIP("done.\n");
		PDEBUG_SNIP("Send end ack 'OKAY'... ");
    buffer = CAN_SERIAL_OKAY;
		if (serial_write(&dev, &buffer, 1) != 1)
			return -1;
		PDEBUG_SNIP("done.\n");
	}
	else {
		PDEBUG_SNIP("ERROR: checksum failure\n");
		PDEBUG_SNIP("Send end ack 'FAILED'... ");
    buffer = CAN_SERIAL_FAILED;
    if (serial_write(&dev, &buffer, 1) != 1)
      return -1;
		PDEBUG_SNIP("done.\n");
	}

  /* 6. change word order in data frame */
	PDEBUG_SNIP("Change word order of received data... ");
	can_serial_change_word_order(data, num_recv);
	PDEBUG_SNIP("done.\n");

	return num_recv;
}

ssize_t can_serial_change_byte_order(unsigned char* data, ssize_t num) {
	int i;
	unsigned char tmp;

	for (i = 2; i < num; i += 2) { /* without opcode & len-1! */
		tmp = data[i]; /* save low-byte */
		data[i] = data[i+1]; /* new low-byte is old high-byte */
		data[i+1] = tmp;	 /* new high-byte is old low-byte */
	}

	return i;
}

ssize_t can_serial_change_word_order(unsigned char* data, ssize_t num) {
	int i;
	unsigned char tmp_lb, tmp_hb;

	for (i = 2; i < (num-2); i +=4 ) { /* without opcode & len-1 ! */
		tmp_hb = data[i]; /* save high-byte word #1 */
		tmp_lb = data[i+1]; /* save low-byte word #1 */

		data[i] = data[i+2]; /* new high-byte word #1 is high-byte word #2 */
		data[i+1] = data[i+3]; /* new low-byte word #1 is low-byte word #2 */

		data[i+2] = tmp_hb; /* new high-byte word #2 is old high-byte word #1*/
		data[i+3] = tmp_lb;	/* new low-byte word #2 is old low-byte word #1*/
	}

	return i;
}

ssize_t can_serial_calc_crc(unsigned char* data, ssize_t num, unsigned char*
  crc_value) {
  unsigned short* buffer = (unsigned short*)data;
  ssize_t num_words = num/2;
  unsigned short crc;

  crc = can_serial_crc_alg(buffer, num_words); /* calculate crc */
  crc_value[0] = (crc >> 8);
  crc_value[1] = crc;

  #ifdef DEBUG_CRC
    PDEBUG_SNIP("crc: 0x%02X%02X\n", crc_value[0], crc_value[1]);
  #endif

  return num_words;
}

unsigned short can_serial_crc_alg(unsigned short* data, ssize_t num) {
  /* EPOS CRC-CCITT Calculation p.8 */
  int i;
	unsigned short shift, c, carry, crc = 0;

	for (i = 0; i < num; ++i) {
		shift = 0x8000;
    c = (data[i] << 8) | (data[i] >> 8);

		do {
			#ifdef DEBUG_CRC
				PDEBUG_SNIP("c: 0x%04hX  shifter: 0x%04hX  ", c, shift);
			#endif

			carry = crc & 0x8000;
			#ifdef DEBUG_CRC
				PDEBUG_SNIP("carry: 0x%04hX  ", carry);
			#endif
			crc <<= 1;
			#ifdef DEBUG_CRC
				PDEBUG_SNIP("crc: 0x%04hX \n", crc);
			#endif
			if (c & shift) ++crc;
			if (carry) crc ^= 0x1021;
			shift >>= 1;
		}
    while (shift);
	}

	#ifdef DEBUG_CRC
		PDEBUG_SNIP("crc: 0x%04hX\n", crc);
	#endif

	return crc;
}
