/*      Header-file for
 *      generic CAN communication commands
 *
 *      Ralf Kaestner   ralf.kaestner@gmail.com
 *      Created:        6.5.2008
 */

#ifndef _CAN_H
#define _CAN_H

/** \file
  * \brief Generic CAN communication
  * Common commands used to communicate via the CAN protocol.
  * These methods are implemented by all CAN communication backends.
  */

/** \brief Structure defining a CAN message
  */
typedef struct {
  int id;                    //!< The CAN message identifier.
  unsigned char content[8];  //!< The actual CAN message content.
} can_message_t;

/** \brief Initialize CAN communication by opening devices
  */
void can_init(void);

/** \brief Close CAN communication by closing devices
  */
void can_close(void);

/** \brief Send a CAN message
  * \param[in] message The CAN message to be sent.
  */
void can_send_message(can_message_t* message);

/** \brief Asynchronously read a CAN message
  * This function instantly returns to the caller.
  */
void can_read_message(void);

/** \brief Asynchronous CAN read message handler
  * \param[in] message The received CAN message.
  */
void can_read_message_handler(const can_message_t* message);

#endif
