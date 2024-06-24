#ifndef _AEWB_LOGGER_SENDER_TYPES_H
#define _AEWB_LOGGER_SENDER_TYPES_H

#include <arpa/inet.h>
#include "ti_2a_wrapper.h"
#include "aewb_logger_types.h"

struct _aewb_logger_sender_state_t {
    struct sockaddr_in dest_addr; 
    int sock_fd; 
    log_aewb_message_t buffer;
};
typedef struct _aewb_logger_sender_state_t aewb_logger_sender_state_t;

typedef struct {
    TI_2A_wrapper *ti_2a_wrapper;
    sensor_config_get *sensor_in_data; 
    sensor_config_set *sensor_out_data;
    tivx_aewb_config_t *aewb_config;
    tivx_h3a_data_t *h3a_data;
    tivx_ae_awb_params_t *ae_awb_result;
} aewb_logger_sender_args_t;

#endif //_AEWB_LOGGER_SENDER_TYPES_H
