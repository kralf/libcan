/*	Source-file for serial EPOS-communication
 *	V0.4
 * 	(C) Marc Rauer ETHZ	marc.rauer(at)gmx.de
 * 	Last change: 05/16/07
 */

/** \file
    \note
    message-structure within libserial:

    message[0]	OpCode	CG p.15
    message[1]	length of data - 1!

    message[2]	high-byte Index	(data1)
    message[3] 	low-byte Index	(data1)

    message[4]	node-id		(data2)
    message[5]	subindex	(data2)

    // if four databytes
    message[6]	high-byte additional data3
    message[7]	low-byte additional data3

    message[8]	high-byte additional data4
    message[9]	low-byte additional data4

    message[10]	high-byte CRC
    message[11] low-byte CRC
*/

/** \file
 *  \brief
 *  Library for communication with EPOS Controller over RS232.
 */

/* includes: */
#include <linux/serial.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <termios.h>
#include <fcntl.h>

#undef ASL_DEBUG
#undef DEBUG

#include <pdebug.h>
#include <epos.h>

#include "can_serial.h"

int fd = 0;
static can_message_t message;

EPOS_ERROR_SERIAL error_serial[MAXERRORSERIAL] = {
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


/* functions: **************************************************************/
void can_init(const char* dev)
{
	fd = open_device(dev);
	clear_iobuffer(fd);
	bzero(&message, sizeof(can_message_t));
}
/*-----------------------------------*/
void can_close()
{
	close_device(fd);
}
/*-----------------------------------*/
void can_send_message(can_message_t* message)
{
	unsigned char data_send[64];
	unsigned char data_recv[64];
	int len;

	len = epos2serial(message->id, message->content, data_send);

		send_dataframe(fd, data_send, len);

		len = receive_dataframe(fd, data_recv);

	serial2epos(data_send, data_recv);
}
/*-----------------------------------*/
void can_read_message()
{
	// Empty!!!
}
/*-----------------------------------*/
int epos2serial(int can_id, unsigned char *msg, unsigned char *data)
{
	int no_bytes_send=0;

	switch(msg[0])
	{
		case EPOS_WRITE_1_BYTE:	data[0]=0x11;	/* opcode write 4 or less bytes */
							data[1]=0x02;	/* len-1 = 2 -> 3 data-words */
							data[2]=msg[2];	/* high-byte index */
							data[3]=msg[1]; /* low-byte index */
							data[4]=(can_id & 0x000000FF);	/* node-id */
							data[5]=msg[3];	/* subindex */
							data[6]=msg[5];	/* high-byte of low-word data */
							data[7]=msg[4];	/* low-byte of low-word data */
							data[8]=0x00;	/* reserved for adding CRC later */
							data[9]=0x00;	/* reserved for adding CRC later */
							no_bytes_send = 10; /* 8 data-bytes + 2 CRC */
							break;

		case EPOS_WRITE_2_BYTE:	data[0]=0x11;	/* opcode write 4 or less bytes */
							data[1]=0x02;	/* len-1 = 2 -> 3 data-words */
							data[2]=msg[2];	/* high-byte index */
							data[3]=msg[1]; /* low-byte index */
							data[4]=(can_id & 0x000000FF);	/* node-id */
							data[5]=msg[3];	/* subindex */
							data[6]=msg[5];	/* high-byte of low-word data */
							data[7]=msg[4];	/* low-byte of low-word data */
							data[8]=0x00;	/* reserved for adding CRC later */
							data[9]=0x00;	/* reserved for adding CRC later */
							no_bytes_send = 10; /* 8 data-bytes + 2 CRC */
							break;

		case EPOS_WRITE_4_BYTE:	data[0]=0x11;		/* opcode write 4 or less bytes */
							data[1]=0x03;		/* len-1 = 3 -> 4 data-words */
							data[2]=msg[2];		/* high-byte index */
							data[3]=msg[1]; 	/* low-byte index */
							data[4]=(can_id & 0x000000FF);	/* node-id */
							data[5]=msg[3];		/* subindex */

							/* Change of order of data-words necessary */
							data[6]=msg[5];		/* high-byte of high-word data */
							data[7]=msg[4];		/* low-byte of high-word data */
							data[8]=msg[7];		/* high-byte of low-word data */
							data[9]=msg[6];		/* low-byte of low-word data */

							data[10]=0x00;		/* reserved for adding CRC later */
							data[11]=0x00;		/* reserved for adding CRC later */
							no_bytes_send = 12; /* 10 data-bytes + 2 CRC */
							break;

		case EPOS_READ:			data[0]=0x10;		/* opcode read 4 or less bytes */
							data[1]=0x01;		/* len-1 = 1 -> 2 data-words */
							data[2]=msg[2];		/* high-byte index */
							data[3]=msg[1]; 	/* low-byte index */
							data[4]=(can_id & 0x000000FF);	/* node-id */
							data[5]=msg[3];		/* subindex */
							data[6]=0x00;		/* reserved for adding CRC later */
							data[7]=0x00;		/* reserved for adding CRC later */
							no_bytes_send = 8; 	/* 6 data-bytes + 2 CRC */
							break;

		default: 			PDEBUG_SNIP("Failure in epos2serial!!! msg[0]=0x%02X\n",msg[0]);
							no_bytes_send = -1;
							break;
	}
	return no_bytes_send;
}
/*-----------------------------------*/
void serial2epos(unsigned char *data_send, unsigned char *data_recv)
{
	long int error_no = 0;
	int i;

	message.id = (data_send[5] & 0x000000FF) + 0x580;

	if(data_recv[2] == 0x00 && data_recv[3] == 0x00 && data_recv[4] == 0x00 &&
        data_recv[5] == 0x00)
	{
		message.content[0] = 0x00;			/* no error */

		message.content[1] = data_send[2];	/* high-byte index */
		message.content[2] = data_send[3]; 	/* low-byte index */

		message.content[3] = data_send[4];	/* subindex */

		message.content[7] = data_recv[6];	/* high-byte of high-word data */
		message.content[6] = data_recv[7];	/* low-byte of high-word data */
		message.content[5] = data_recv[8];	/* high-byte of low-word data */
		message.content[4] = data_recv[9];	/* low-byte of low-word data */
	}
	else // Serial error occured see EPOS Communication Guide page 18
	{
                //message.content[0] = 0x80;	        /* Configuration error */

		error_no = (long int)
                  ((data_recv[4]<<24)+(data_recv[5]<<16)+(data_recv[2]<<8)+data_recv[3]);

		for(i=0;i<MAXERRORSERIAL;i++)
		{
			if(error_serial[i].code == error_no)
			{
				printf("Serial ErrorCode 0x%08lX: %s\n", error_serial[i].code,
                                  error_serial[i].msg);
				epos_read.node[(message.id - 0x581)].error.comm.code =
                                  error_serial[i].code;
				epos_read.node[(message.id - 0x581)].error.comm.msg =
                                  error_serial[i].msg;
			}
		}
		return;
	}

	can_read_message_handler(&message);
}
/*-----------------------------------*/
int send_dataframe(int fd, unsigned char *data, int no_bytes_send)
{
	int i;
	unsigned char buffer;
	unsigned char crc_value[2];
	int ret=0;


/* 1. Calculate crc and add to data */

	PDEBUG_SNIP("Calculate crc..");
	calc_crc(data, crc_value, no_bytes_send);
	PDEBUG_SNIP("Done.\n");

	PDEBUG_SNIP("Insert crc in data-frame...");
	data[no_bytes_send-2] = crc_value[0];
	data[no_bytes_send-1] = crc_value[1];
	PDEBUG_SNIP("Done.\n");

	//prtmsg("data to send: ", data, no_bytes_send);


/* 2. Change order of bytes to send*/

	PDEBUG_SNIP("Change order of data-bytes to send...");
	chg_byte_order(data, no_bytes_send);
	PDEBUG_SNIP("Done.\n");

	//prtmsg("data ready to send",data,no_bytes_send);


/* 3. Send Op-code and wait for readyAck */

	for(i=0;i<MAXRETRY;i++)
	{
		PDEBUG_SNIP("Send opcode 0x%02X...",data[0]);

		ret = write_byte(fd, data[0]);

		if(ret != 1)
		{
			return -1;
		}
		PDEBUG_SNIP("Done.\n");

		PDEBUG_SNIP("Wait for readyAck...");

		ret = read_byte(fd,&buffer);

		if((ret == 1) && (buffer == OKAY))
		{
		  	PDEBUG_SNIP("Done. Received 'OKAY'.\n");
			break;
		}
		if(ret == 1 && buffer == FAILED)
		{
			PDEBUG_SNIP("Done. Received 'FAILED'.\nEPOS seems not to be ready!!!\n");
			//			continue;
		}
		if(ret == 0 )
		{
			PDEBUG_SNIP("ERROR: EPOS doesn't answer!!!\n");
			//			continue;
		}
		PDEBUG_SNIP("ERROR: Unkown value. Will try again!!!\n");
	}
	if(i == MAXRETRY)
	{
		PDEBUG_SNIP("Failure at sending op-code!!!\n");
		return -1;
	}


/* 4. Send rest of message and wait for endAck */
/*
		PDEBUG_SNIP("Send data-length...");
		if(write_byte(fd, data[1]) != 1)
		{
			return -1;
		}
		PDEBUG_SNIP("Done.\n");
	*/

	PDEBUG_SNIP("Send rest of data-frame...");
	if(write_string(fd, data+1, no_bytes_send-1) != no_bytes_send-1)
	{
		return -1;
	}
	PDEBUG_SNIP("Done.\n");

	for(i=0;i<MAXRETRY;i++)
	{
		PDEBUG_SNIP("Wait for endAck...");
		ret = read_byte(fd,&buffer);

		if(ret == 1 && buffer == OKAY)
		{
		  	PDEBUG_SNIP("Done. Received 'OKAY'.\n");
			break;
		}
		if(ret == 1 && buffer == FAILED)
		{
			PDEBUG_SNIP("Done. Received 'FAILED'.\nEPOS hasn't accepted data!!!\n");
			return -1;
		}
		if(ret == 0)
		{
			PDEBUG_SNIP("ERROR: EPOS doesn't answer!!!\n");
			continue;
		}
		PDEBUG_SNIP("ERROR: Unkown value. Will try again!!!\n");
	}

	if(i == MAXRETRY)
	{
		PDEBUG_SNIP("Failure at sending rest of data!!!\n");
		return -1;
	}

	return no_bytes_send;
}
/*-----------------------------------*/
int receive_dataframe(int fd, unsigned char *data)
{
	int i,n,ret;
	unsigned char buffer, crc_value[2];
	int no_bytes_recv=0,no_bytes_expect=0;



/* 1. Wait for response-opcode */

	for(i=0;i<MAXRETRY;i++)
	{
		PDEBUG_SNIP("Wait for response-opcode...");

		ret = read_byte(fd,&buffer);

		if(ret == 1 && buffer == RESPONSE)
		{
		  	PDEBUG_SNIP("Done.\n");
			data[0] = RESPONSE;
			break;
		}
		if(ret == 0)
		{
			PDEBUG_SNIP("ERROR: EPOS doesn't answer!!!\n");
			continue;
		}
		PDEBUG_SNIP("ERROR: Unkown value: 0x%02X. Will try again!!!\n",buffer);
	}
	if(i == MAXRETRY)
	{
		PDEBUG_SNIP("Failure at receiving response op-code!!!\n");
		return -1;
	}


/* 2. Send readyAck and wait for dataframe length (len-1)*/

	PDEBUG_SNIP("Send readyAck...");

	ret = write_byte(fd, OKAY);

	if(ret != 1)
	{
		return -1;
	}

	PDEBUG_SNIP("Done.\n");

	for(i=0;i<MAXRETRY;i++)
	{
		PDEBUG_SNIP("Wait for dataframe length...");

		ret = read_byte(fd,&buffer);

		if(ret == 1)
		{
		  	PDEBUG_SNIP("Done.\n");
			data[1] = buffer;
			break;
		}
		if(ret == 0 )
		{
			PDEBUG_SNIP("ERROR: EPOS doesn't answer!!!\n");
			continue;
		}
		PDEBUG_SNIP("ERROR: Unkown value. Will try again!!!\n");
	}
	if(i == MAXRETRY)
	{
		PDEBUG_SNIP("Failure at receiving response op-code!!!\n");
		return -1;
	}


/* 3. Read rest of dataframe (datawords + crcword) */

	no_bytes_expect = (data[1]+2)*2;	/* number of expected bytes */

	PDEBUG_SNIP("Wait for expected %d bytes...",no_bytes_expect);

	for(n=0;n<no_bytes_expect;n++)
	{
		ret = read_byte(fd,&buffer);
		data[n+2] = buffer;
	}

	if(n != no_bytes_expect)
	{
		PDEBUG_SNIP("ERROR: %d bytes received, %d bytes expected!!!",n,no_bytes_expect);
		return -1;
	}
	PDEBUG_SNIP("Done.\n");

	no_bytes_recv = n+2;

	//prtmsg("data received",data,no_bytes_recv);

/* 4. change order of received data-bytes */

	PDEBUG_SNIP("Change order of received data-bytes...");

	chg_byte_order(data, no_bytes_recv);

	PDEBUG_SNIP("Done.\n");

	//prtmsg("data changed",data,no_bytes_recv);



/* 5. check crc and send Ack to EPOS*/

	PDEBUG_SNIP("Check CRC...");

	calc_crc(data, crc_value, no_bytes_recv);

	//prtmsg("calc_crc",crc_value,2);

	if(crc_value[0] == 0x00 && crc_value[1] == 0x00)
	{
		PDEBUG_SNIP("Done.\n");
		PDEBUG_SNIP("Send endAck 'OKAY'...");

		ret = write_byte(fd, OKAY);

		if(ret!=1)
		{
			return -1;
		}
		PDEBUG_SNIP("Done.\n");
	}
	else
	{
		PDEBUG_SNIP("Failure: crc is wrong!!!\n");
		PDEBUG_SNIP("Send endAck 'FAILED'...");

		ret = write_byte(fd, FAILED);

		if( ret!=1)
		{
			return -1;
		}
		PDEBUG_SNIP("Done.\n");
	}

/* 6. change word order in data-frame */

	PDEBUG_SNIP("Change order of received data-words...");

	chg_word_order(data, no_bytes_recv);

	PDEBUG_SNIP("Done.\n");

/* Finished */
	return no_bytes_recv;
}
/*-----------------------------------*/
int open_device(const char *name)
{
	struct termios oldtio, newtio;

	fd=open(name, O_RDWR | O_NOCTTY);//Serial Programming HOWTO
	if(fd == -1)
	{
		perror("Error: Can't open device");
		exit(-1);
	}

	tcgetattr(fd,&oldtio); 				// save current serial port settings
	bzero(&newtio, sizeof(newtio)); 	// clear struct for new port settings

	newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD; // | HUPCL;
	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0;
	newtio.c_lflag = 0;
	newtio.c_cc[VMIN] = 1;				// blocking read until 8 chars received
	newtio.c_cc[VTIME] = 0;				// inter-character timer 0.5s

	tcflush(fd, TCIFLUSH);				// clean the modem line
	tcsetattr(fd, TCSANOW, &newtio);	// activate the settings for the port

	#ifdef DEBUG_DEV
		PDEBUG_SNIP("fd: %d\n",fd);
	#endif
	return fd;
}
/*-----------------------------------*/
int close_device(int fd)
{
	#ifdef DEBUG_DEV
	PDEBUG_SNIP("Try to close device... ");
	#endif
	if(fd > -1)
	{
		if(close(fd) == -1)
		{
			perror("ERROR: Failure at close()");
			return -1;
		}
		else
		{
			#ifdef DEBUG_DEV
			PDEBUG_SNIP("Done\n");
			#endif
			return 0;
		}
	}
	PDEBUG_SNIP("ERROR: No device to close!\n");
	return -1;
}
/*-----------------------------------*/
int read_byte(int fd, unsigned char *buffer)
{
	int ret,n;
	struct timeval tvlr;
	fd_set fdset;
	tvlr.tv_sec = TIMEOUTSEC;
	tvlr.tv_usec = TIMEOUTNSEC;

	FD_ZERO(&fdset);
	FD_SET(fd, &fdset);

	ret = select(fd + 1, &fdset, NULL, NULL, &tvlr);

	if(ret > 0)
	{
		n = read(fd, buffer, 1);
		#ifdef DEBUG
			PDEBUG_SNIP("read %d byte(s): 0x%02X\n", n, buffer[0]);
		#endif
		return n;
	}
	if(ret == 0)
	{
		printf("ERROR: time-out at reading one Byte\n");

		return 0;
	}
	perror("Failure at read_byte");
	return -1;
}
/*-----------------------------------*/
int write_byte(int fd, unsigned char data)
{
	int ret;
	#ifdef SERIAL_DUMMY
		PDEBUG_SNIP("==> serial Out:|0x%02X|\n",data);
		return 0;
	#else

	ret = write(fd,  &data , 1);

	if(ret == 1) return 1;

	perror("Failure at write_byte");
	return 0;
	#endif
}
/*-----------------------------------*/
int write_string(int fd, unsigned char *data, int len)
{
	int length;
	#ifdef SERIAL_DUMMY
		PDEBUG_SNIP("==> serial Out:|0x%s|\n",data);
		return 0;
	#else

	length= write(fd, (const void*) data , len);

	if(length < 0 || length != len)
	{
		perror("Failure at write_string");
		return 0;
	}
	return len;
	#endif
}
/*-----------------------------------*/
int c2w(unsigned char *data, word *buffer, int no_chars)
{
	int n,i=0;

	for(n=0;n<no_chars;n+=2)
	{
		buffer[i++] = (data[n+1] & 0x00FF) | ((data[n]<<8) & 0xFF00);
	}
	return i;
}
/*-----------------------------------*/
int w2c(word *buffer, unsigned char *data, int no_words)
{
	int n;

	for(n=0;n<no_words<<1;n+=2)
	{
		data[n+1] = buffer[n>>1] & 0xFF;
		data[n] = (buffer[n>>1]>>8) & 0xFF;
	}
	return n;
}
/*-----------------------------------*/
int chg_byte_order(unsigned char *data, int no_chars)
{
	int i;
	unsigned char temp;

	for(i=2;i<no_chars;i+=2)	/* without opcode & len-1 ! */
	{
		temp = data[i];			/* save low-byte */
		data[i] = data[i+1];	/* new low-byte is old high-byte */
		data[i+1] = temp;		/* new high-byte is old low-byte */
	}
	return i;
}
/*-----------------------------------*/
int chg_word_order(unsigned char *data, int no_chars)
{
	int i;
	unsigned char tmp_lb, tmp_hb;

	for(i=2;i<(no_chars-2);i+=4)	/* without opcode & len-1 ! */
	{
		tmp_hb = data[i];			/* save high-byte word #1 */
		tmp_lb = data[i+1];			/* save low-byte word #1 */

		data[i] = data[i+2];	/* new high-byte word #1 is high-byte word #2 */
		data[i+1] = data[i+3];	/* new low-byte word #1 is low-byte word #2 */

		data[i+2] = tmp_hb;		/* new high-byte word #2 is old high-byte word #1*/
		data[i+3] = tmp_lb;		/* new low-byte word #2 is old low-byte word #1*/
	}
	return i;
}
/*-----------------------------------*/
word crc_alg(word *buffer, int no) /* EPOS CRC-CCITT Calcu. p.8 */
{
	word shifter, c,carry,crc=0;

	while(no--)
	{
		shifter = 0x8000;
		c = *buffer++;
		do
		{
			#ifdef DEBUG_CRC
				PDEBUG_SNIP("c: 0x%04hX  shifter: 0x%04hX  ",c,shifter);
			#endif

			carry = crc & 0x8000;
			#ifdef DEBUG_CRC
				PDEBUG_SNIP("carry: 0x%04hX  ",carry);
			#endif
			crc <<= 1;
			#ifdef DEBUG_CRC
				PDEBUG_SNIP("crc: 0x%04hX \n",crc);
			#endif
			if(c & shifter) crc++;
			if(carry) crc ^= 0x1021;
			shifter >>= 1;
		} while(shifter);
	}
	#ifdef DEBUG_CRC
		PDEBUG_SNIP("crc: 0x%04hX\n",crc);
	#endif
	return crc;
}
/*-----------------------------------*/
int calc_crc(unsigned char *data, unsigned char *crc_value, int no_char)
{
	word buffer[32];
	word crc_word;
	int no_words;

	no_words = c2w(data, buffer, no_char);	/* change data char[] to word[] */
	crc_word = crc_alg(buffer, no_words); 	/* calculate crc */
	w2c(&crc_word, crc_value, 1);		/* change crc word[] to char[] */

	#ifdef DEBUG_CRC
		PDEBUG_SNIP("crc: 0x%02X%02X\n",crc_value[0],crc_value[1]);
	#endif
	return no_words;
}
/*-----------------------------------*/
void prtmsg(char *desc, unsigned char *msg, int no)
{
	int i;

	printf("%s: ", desc);
	for(i=0;i<no;i++)
	{
		printf("\t0x%02x", msg[i]);
	}
	printf("\n");
}
/*-----------------------------------*/
int clear_iobuffer(int fd)
{
	int i=0;
	unsigned char IOBuffer;
	struct timeval tvlIO;
	fd_set fdsetIO;
	tvlIO.tv_sec = 0;
	tvlIO.tv_usec = 100000;

	FD_ZERO(&fdsetIO);
	FD_SET(fd, &fdsetIO);

	for(;;)
	{
		if(select(fd + 1, &fdsetIO, NULL, NULL, &tvlIO))
		{
			read(fd, &IOBuffer, 1);
			PDEBUG_SNIP("|%s| ",&IOBuffer);
			i++;
		}
		else
		{
			PDEBUG_SNIP("IO-Buffer seems to be clear :-) %d bytes read!\n",i);
			return 0;
		}
	}
	return -1;
}
/* end of libserial.c -----------------------------------*/
