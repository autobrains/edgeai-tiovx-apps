#include "aewb_logger_sender.h"
#include <stdlib.h>
#include <unistd.h>

typedef struct {
    int32_t size;
    int8_t *addr;
} field_t ;

aewb_logger_sender_state_t *aewb_logger_create_sender(const char *dest_ip, in_port_t port) {
    aewb_logger_sender_state_t *p_state = malloc(sizeof(aewb_logger_sender_state_t));
    memset(p_state, 0, sizeof(aewb_logger_sender_state_t));

    // Creating socket file descriptor
    if ((p_state->sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        return NULL;
    }

    p_state->dest_addr.sin_family = AF_INET;
    p_state->dest_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, dest_ip, &p_state->dest_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        return NULL;
    }

    return p_state;   
}

int32_t aewb_logger_write_log_to_buffer(AewbHandle *handle, tivx_h3a_data_t *h3a_ptr, tivx_ae_awb_params_t *aewb_ptr, int8_t * dest_buffer, int32_t buffer_size, int32_t *p_num_bytes_written)
{
    int8_t* buf_ptr = dest_buffer;

    // int32_t h3a_n_col = handle->ti_2a_wrapper.p_awb_params->ti_awb_data_in.frame_data.h3a_data_x;
    // int32_t h3a_n_row = handle->ti_2a_wrapper.p_awb_params->ti_awb_data_in.frame_data.h3a_data_y;

    field_t fields[] = {
        /* _AewbHandle */

        // {sizeof(handle->cfg), &handle->cfg;},
        {sizeof(handle->aewb_config), (int8_t*)&handle->aewb_config},
        // {sizeof(handle->sensor_obj), (int8_t*)&handle->sensor_obj},
        // {handle->dcc_2a_buf_size, (int8_t*)dcc_2a_buf},
        
        /* _AewbHandle.TI_2A_wrapper start */
        {sizeof(*handle->ti_2a_wrapper.dcc_input_params), (int8_t*)handle->ti_2a_wrapper.dcc_input_params},
        // {handle->ti_2a_wrapper.dcc_input_params->dcc_buf_size, (int8_t*)handle->ti_2a_wrapper.dcc_input_params->dcc_buf}, // skip, big
        // {sizeof(*handle->ti_2a_wrapper.dcc_output_params), (int8_t*)handle->ti_2a_wrapper.dcc_output_params}, // skip, big
        // {sizeof(*handle->ti_2a_wrapper.p_awb_params), (int8_t*)handle->ti_2a_wrapper.p_awb_params}, // skip, big
        // {sizeof(*handle->ti_2a_wrapper.p_awb_params->sen_awb_calc_data), (int8_t*)handle->ti_2a_wrapper.p_awb_params->sen_awb_calc_data}, // skip, big
        {sizeof(*handle->ti_2a_wrapper.p_ae_params), (int8_t*)handle->ti_2a_wrapper.p_ae_params},
        // {sizeof(h3a_aewb_paxel_data_t)*h3a_n_col*h3a_n_row, (int8_t*)handle->ti_2a_wrapper.p_h3a_merge}, // skip, derived from 3a sums
        // {sizeof(h3a_aewb_paxel_data_t)*h3a_n_col*h3a_n_row, (int8_t*)handle->ti_2a_wrapper.awb_h3a_res}, // skip, derived from 3a sums
        // {sizeof(handle->ti_2a_wrapper.ae_awb_result_prev), (int8_t*)handle->ti_2a_wrapper.ae_awb_result_prev}, // skip, already coverted by aewb_ptr
        {sizeof(handle->ti_2a_wrapper.frame_count), (int8_t*)&handle->ti_2a_wrapper.frame_count},
        {sizeof(handle->ti_2a_wrapper.sensor_pre_wb_gain), (int8_t*)&handle->ti_2a_wrapper.sensor_pre_wb_gain},
        {sizeof(handle->ti_2a_wrapper.skipAWB), (int8_t*)&handle->ti_2a_wrapper.skipAWB},
        {sizeof(handle->ti_2a_wrapper.skipAE), (int8_t*)&handle->ti_2a_wrapper.skipAE}, 

        /* _AewbHandle.TI_2A_wrapper end */
        
        {sizeof(handle->sensor_in_data), (int8_t*)&handle->sensor_in_data},
        {sizeof(handle->sensor_out_data), (int8_t*)&handle->sensor_out_data},
        
        /* tivx_ae_awb_params_t */
        {sizeof(tivx_ae_awb_params_t), (int8_t*)aewb_ptr}
    };

    for (int i=0; i<sizeof(fields)/sizeof(field_t); i+=1) {
        
        field_t *f = &fields[i];
        if (*p_num_bytes_written + f->size > buffer_size) {
            printf("aewb_logger_write_log_to_buffer: buffer smaller than expected, aborting write, i=%d, buffer_size=%d\n", i, buffer_size);
            break;
        }

        // printf("i=%d\n", i);
        memcpy(buf_ptr, f->addr, f->size);
        buf_ptr += f->size;
        *p_num_bytes_written += f->size;
    }

    return *p_num_bytes_written;
}

int32_t aewb_logger_send_bytes(aewb_logger_sender_state_t *p_state)
{
    return sendto(p_state->sock_fd, 
            (const int8_t *)p_state->buffer, p_state->num_bytes_written, MSG_CONFIRM, 
            (const struct sockaddr *)&p_state->dest_addr, sizeof(p_state->dest_addr)
    );
}

int32_t aewb_logger_send_log(aewb_logger_sender_state_t *p_state, AewbHandle *handle, tivx_h3a_data_t *h3a_ptr, tivx_ae_awb_params_t *aewb_ptr)
{
    if (p_state==NULL)
        return 0;

    memset(p_state->buffer, 0, sizeof(p_state->buffer));
    p_state->num_bytes_written = 0;

    aewb_logger_write_log_to_buffer(handle, h3a_ptr, aewb_ptr, p_state->buffer, sizeof(p_state->buffer), &p_state->num_bytes_written);

    return aewb_logger_send_bytes(p_state);
}

void aewb_logger_destroy_sender(aewb_logger_sender_state_t *p_state) {
    close(p_state->sock_fd);
    free(p_state);
}