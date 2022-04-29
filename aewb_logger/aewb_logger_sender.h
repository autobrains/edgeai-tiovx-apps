#ifndef _AEWB_LOGGER_SENDER_H
#define _AEWB_LOGGER_SENDER_H

#include "linux_aewb_module.h"
#include "aewb_logger_types.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif


struct _AewbHandle;  // avoid cyclic dependency
typedef struct _AewbHandle AewbHandle;

struct _aewb_logger_sender_state_t {
    struct sockaddr_in dest_addr; 
    int sock_fd; 
    log_aewb_message_t buffer;
};
typedef struct _aewb_logger_sender_state_t aewb_logger_sender_state_t;

aewb_logger_sender_state_t *aewb_logger_create_sender(const char *dest_ip, in_port_t port);
void aewb_logger_destroy_sender(aewb_logger_sender_state_t *p_state);
int32_t aewb_logger_write_log_to_buffer(AewbHandle *handle, tivx_h3a_data_t *h3a_ptr, log_aewb_message_t * dest_buffer);
int32_t aewb_logger_send_bytes(aewb_logger_sender_state_t *p_state);
int32_t aewb_logger_send_log(aewb_logger_sender_state_t *p_state, AewbHandle *handle, tivx_h3a_data_t *h3a_ptr);


#ifdef __cplusplus
}
#endif

#endif //_AEWB_LOGGER_SENDER_H