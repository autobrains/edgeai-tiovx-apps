/*
    this _AewbHandle definition used to be in linux_aewb_module.c
    it was moved to this new file so that aewb_logger_* files could include it
    it was moved to this file and not to linux_aewb_module.h to avoid changing the linux_aewb_module.h interface
*/
#ifndef _LINUX_AEWB_HANDLE_TYPE_H
#define _LINUX_AEWB_HANDLE_TYPE_H

#include "linux_aewb_module.h"
#include "tiovx_utils.h"
#include "ti_2a_wrapper.h"
#include "aewb_logger_sender_types.h"

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

struct _AewbHandle {
    AewbCfg             cfg;
    tivx_aewb_config_t  aewb_config;
    SensorObj           sensor_obj;
    uint8_t             *dcc_2a_buf;
    uint32_t            dcc_2a_buf_size;
    TI_2A_wrapper       ti_2a_wrapper;
    sensor_config_get   sensor_in_data;
    sensor_config_set   sensor_out_data;
    int fd;
    aewb_logger_sender_state_t *aewb_logger_sender_state_ptr;
};

#endif //_LINUX_AEWB_HANDLE_TYPE_H