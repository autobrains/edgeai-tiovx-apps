#ifndef _AEWB_LOGGER_RECEIVER_TYPES_H
#define _AEWB_LOGGER_RECEIVER_TYPES_H

#include <arpa/inet.h>
#include "aewb_logger_types.h"

struct _aewb_logger_receiver_state_t {
    struct sockaddr_in bind_addr; 
    int sock_fd; 
    log_aewb_message_t buffer;
};
typedef struct _aewb_logger_receiver_state_t aewb_logger_receiver_state_t;

#endif //_AEWB_LOGGER_RECEIVER_TYPES_H
