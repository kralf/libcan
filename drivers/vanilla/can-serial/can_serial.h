/*	Header-file for serial CAN communication via EPOS
 *	v0.4
 * 	(C) Marc Rauer ETHZ	marc.rauer(at)gmx.de
 * 	Last change: 05/16/07
 */

#ifndef _CAN_SERIAL_H
#define _CAN_SERIAL_H

/**
*  \file
*  \brief
*  EPOS-communication over RS232
*  <h2>Introduction</h2>
*
*  This layer provides low-level mechanisms for communicating with EPOS
*  motion controllers over a serial connection
*/

#include <can.h>

/* defines: */
/**	Value of the acknowledge byte, if dataframe was valid.
*/
#define OKAY 		0x4F
/** Value of the acknowledge byte, if dataframe was invalid.
*/
#define FAILED		0x46
/** Value of the op-code for a response message.
*/
#define RESPONSE	0x00

/** Default baudrate using EPOS.
*/
#define BAUDRATE B57600
/** Default timeout [seconds] during reading form serial device (used by select()).
*/
#define TIMEOUTSEC 0
/** Default timeout [nanoseconds] during reading form serial device (used by select()).
*/
#define TIMEOUTNSEC 50000 /* 50ms */
/** Specifies maximum retries during failed read/write-operations.
*/
#define MAXRETRY 10
/** Current entries in the error-structure for handling specific serial errors.
*/
#define MAXERRORSERIAL 31

/** Defines a datatype of size word for handling the CRC-algorithm.
*/
typedef unsigned short word;

typedef struct EPOS_ERROR_SERIAL{
  long int code;
  char *msg;
} EPOS_ERROR_SERIAL;

/* function-prototypes: */

/** Convert the msg-array defined in libepos.c to the data-array defined in libserial.c.
*
*	\note
*	This conversion is necessary, because msg-array in libepos.c relies strongly on CAN protocol.
*
*	\return
*	Number of bytes in the dataframe to send
*/
int epos2serial(/** Node-ID */ int can_id,
                /** Array of char containig the msg-array */ unsigned char *msg,
                /** Array of char to store converted data-array*/ unsigned char *data);

/** Convert the data-array defined in libserial.c to the cpcmsg-structure defined in libepos.c. This function
*	also calls read_SDO_msg_handler() in libepos.c, which handles the incoming messages.
*
*	\note
*	This conversion is necessary, because cpcmsg-structure in libepos.c assumes that the message was received by CAN protocol.
*/
void serial2epos(/** Array of char representing the dataframe sent to EPOS */ unsigned char *data_send,
                 /** Array of char representing the dataframe received from EPOS */ unsigned char *data_recv);

/** Open a device using termios for configuration.
*
*	\warning
*	Function exits program, if there is no/wrong device!
*
* 	\return
*	File descriptor
*/
int open_device(/** Name of the device (/dev/ttyS0 for first serial port) */ const char *name);

/** Close an open device.
**
* 	\return
*	- 0: Device successfully closed
*	- -1: Error closing device
*/
int close_device(/** File descriptor */ int fd);

int send_dataframe(int fd, unsigned char *data, int no_bytes_send);

int receive_dataframe(int fd, unsigned char *data);

/** Read bytes from device.
*
*	\note
*	Uses select() for non-infinit blocking (see TIMEOUTSEC).
*
*  	\return
*   - 0: Timeout reading a byte
*   - >0: Number of bytes read
*   - -1: Unspecified error in read()
*/
int read_byte(/** File descriptor */ int fd,
              /** Array of char containing read bytes */ unsigned char *buffer);

/** Write one byte to a device.
*
*	\return
*	- 1: Byte successfully written
*	- 0: Error at writing
*/
int write_byte(/** File descriptor */ int fd,
               /** Byte to send */ unsigned char data);

/** Write a string represented by an array of char to a device.
*
*	\return
*	- >0: Number of bytes successfully written
*	- 0: Error at writing
*/
int write_string(/** File descriptor */ int fd,
                 /** Array of char to send */ unsigned char *data,
                 /** Number of chars to send */ int len);

/** Converts an array of char to an array of word by taking two char as a word.
*
*	\return
*	Number of words in the array
*/
int c2w(/** Array of char to convert */ unsigned char *data,
        /** Array of word to store converted values */ word *buffer,
        /** Number of chars to convert */ int no_chars);

/** Converts an array of word to an array of char by dividing a word into two char.
*
*	\return
*	Number of chars in the array
*/
int w2c(/** Array of word to convert */ word *buffer,
        /** Array of char to store converted values */ unsigned char *data,
        /** Number of words to convert */ int no_words);

/** Change order of databytes in the dataframe. The first two char will be ignored, the
* 	following chars will be changed by each other.
*
*	\Note
*	Necessary according EPOS Communication guide.
*	\return
*	Number of changed bytes within the dataframe
*/
int chg_byte_order(/** Array of bytes for which change order*/ unsigned char *data,
                   /** Number of bytes in the array (length)*/ int no_chars);

/** Change order of datawords (two char) in the dataframe. The first two char will be ignored, the
* 	following chars will be grouped by two and these groups changed by each other.
*
*	\Note
*	Necessary according EPOS Communication guide.
*	\return
*	Number of changed bytes within the dataframe.
*/
int chg_word_order(/** Array of char for which change order*/ unsigned char *data,
                   /** Number of bytes! in the array (length)*/ int no_chars);

/** Calculate 16-bit CRC checksum using CRC-CCITT algorithm.
*
*	\note
*	Calculation has to include all bytes in the dataframe. Internally the array of char
*       is transformed to an array of words to calculate the CRC. The CRC value as a word is
*	then tranformed back to an array of char.
*
*	\return
*	Number of words build from the array.
*/
int calc_crc(/** Array of bytes representing the dataframe */ unsigned char *data,
             /** Array of two bytes to store the CRC-word*/ unsigned char *crc_value,
             /** Number of bytes in the dataframe*/ int no_char);

/** Implementation of the CRC-CCITT algorithm.
*
*	\return
*	Calculated CRC-value.
*/
word crc_alg(/** Array of words containing the dataframe */ word *buffer,
             /** Number of words in the dataframe*/ int no);

/** Clears the IO-Buffer by reading all available bytes.
*
*	\return
*	- 0: no more bytes available
*	- -1: unspecified error
*/
int clear_iobuffer(/** File descriptor*/ int fd);

void prtmsg(char *desc, unsigned char *msg, int no);

#endif
