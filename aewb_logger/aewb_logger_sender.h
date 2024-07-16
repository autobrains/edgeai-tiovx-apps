#ifndef _AEWB_LOGGER_SENDER_H
#define _AEWB_LOGGER_SENDER_H

#include <stdlib.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "aewb_logger_types.h"
#include "aewb_logger_sender_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// creates a sender instance, to be used with the functions that follow below.
aewb_logger_sender_state_t *aewb_logger_create_sender(const char *dest_ip, in_port_t port);

// cleans up a sender instance.
void aewb_logger_destroy_sender(aewb_logger_sender_state_t *p_state);

// serializes aewb_logger_sender_args_t into a buffer.
// returns the number of bytes written to the buffer.
int32_t aewb_logger_write_log_to_buffer(aewb_logger_sender_args_t *args, log_aewb_message_t * dest_buffer);

// sends the buffer in p_state over udp (ie the aewb_logger_sender_args_t previously serialized into p_state).
// returns the number of bytes sent.
int32_t aewb_logger_send_bytes(aewb_logger_sender_state_t *p_state);

// serializes aewb_logger_sender_args_t into a buffer and then it sends over udp
// returns the number of bytes sent.
int32_t aewb_logger_send_log(
    aewb_logger_sender_state_t *p_state, 
    TI_2A_wrapper *ti_2a_wrapper,
    sensor_config_get *sensor_in_data,
    sensor_config_set *sensor_out_data,
    tivx_aewb_config_t *aewb_config,
    tivx_h3a_data_t *h3a_data,
    tivx_ae_awb_params_t *ae_awb_result);


#ifdef __cplusplus
}
#endif

#endif //_AEWB_LOGGER_SENDER_H
