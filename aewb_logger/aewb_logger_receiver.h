#ifndef _AEWB_LOGGER_RECEIVER_H
#define _AEWB_LOGGER_RECEIVER_H

#include "aewb_logger_types.h"
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
    log_AewbHandle buffer;
};
typedef struct _aewb_logger_receiver_state_t aewb_logger_receiver_state_t;

aewb_logger_receiver_state_t *aewb_logger_create_receiver(const char *bind_ip, in_port_t port);
void aewb_logger_destroy_receiver(aewb_logger_receiver_state_t *p_state);
int32_t aewb_logger_recv_bytes(aewb_logger_receiver_state_t *p_state);
int32_t aewb_logger_recv_log(aewb_logger_receiver_state_t *p_state);


#ifdef __cplusplus
}
#endif

#endif //_AEWB_LOGGER_RECEIVER_H