#ifndef _AEWB_LOGGER_RECEIVER_H
#define _AEWB_LOGGER_RECEIVER_H

#include "linux_aewb_module.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif


struct _AewbHandle; // avoid cyclic dependency
typedef struct _AewbHandle AewbHandle;

struct _aewb_logger_receiver_state_t {
    struct sockaddr_in bind_addr; 
    int sock_fd; 
    int8_t buffer[1500];
    int32_t num_bytes_written;
};
typedef struct _aewb_logger_receiver_state_t aewb_logger_receiver_state_t;

aewb_logger_receiver_state_t *aewb_logger_create_receiver(const char *bind_ip, in_port_t port);
void aewb_logger_destroy_receiver(aewb_logger_receiver_state_t *p_state);
int32_t aewb_logger_read_log_from_buffer(int8_t * src_buffer, int32_t buffer_size, AewbHandle *handle, tivx_h3a_data_t *h3a_ptr, tivx_ae_awb_params_t *aewb_ptr);
int32_t aewb_logger_recv_bytes(aewb_logger_receiver_state_t *p_state);
int32_t aewb_logger_recv_log(aewb_logger_receiver_state_t *p_state, AewbHandle *handle, tivx_h3a_data_t *h3a_ptr, tivx_ae_awb_params_t *aewb_ptr);


#ifdef __cplusplus
}
#endif

#endif //_AEWB_LOGGER_RECEIVER_H