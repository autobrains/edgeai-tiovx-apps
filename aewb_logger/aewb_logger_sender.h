#ifndef _AEWB_LOGGER_SENDER_H
#define _AEWB_LOGGER_SENDER_H

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include "linux_aewb_module.h"
#include "aewb_logger_types.h"
#include "aewb_logger_sender_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// creates a sender instance, to be used with the functions that follow below.
aewb_logger_sender_state_t *aewb_logger_create_sender(const char *dest_ip, in_port_t port);

// cleans up a sender instance.
void aewb_logger_destroy_sender(aewb_logger_sender_state_t *p_state);

// serializes AewbHandle into a buffer.
// returns the number of bytes written to the buffer.
int32_t aewb_logger_write_log_to_buffer(AewbHandle *handle, tivx_h3a_data_t *h3a_ptr, tivx_ae_awb_params_t *ae_awb_result, log_aewb_message_t * dest_buffer);

// sends the buffer in p_state over udp (ie the AewbHandle previously serialized into p_state).
// returns the number of bytes sent.
int32_t aewb_logger_send_bytes(aewb_logger_sender_state_t *p_state);

// serializes AewbHandle into a buffer and then it sends over udp
// returns the number of bytes sent.
int32_t aewb_logger_send_log(aewb_logger_sender_state_t *p_state, AewbHandle *handle, tivx_h3a_data_t *h3a_ptr, tivx_ae_awb_params_t *ae_awb_result);


#ifdef __cplusplus
}
#endif

#endif //_AEWB_LOGGER_SENDER_H
