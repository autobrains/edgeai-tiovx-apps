#ifndef _AEWB_LOGGER_RECEIVER_H
#define _AEWB_LOGGER_RECEIVER_H

#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "linux_aewb_handle_type.h"
#include "aewb_logger_types.h"
#include "aewb_logger_receiver_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// creates a receiver instance, to be used with the functions that follow below.
aewb_logger_receiver_state_t *aewb_logger_create_receiver(const char *bind_ip, in_port_t port);

// cleans up a receiver instance.
void aewb_logger_destroy_receiver(aewb_logger_receiver_state_t *p_state);

// receives a udp packet. 
// returns the number of bytes received.
int32_t aewb_logger_recv_bytes(aewb_logger_receiver_state_t *p_state);

// wrapper for aewb_logger_recv_bytes() with extra sanity checks. 
// returns the number of bytes received.
int32_t aewb_logger_recv_log(aewb_logger_receiver_state_t *p_state);


#ifdef __cplusplus
}
#endif

#endif //_AEWB_LOGGER_RECEIVER_H
