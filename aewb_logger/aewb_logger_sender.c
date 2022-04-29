#include "aewb_logger_sender.h"
#include "aewb_logger_types.h"
#include <stdlib.h>
#include <unistd.h>

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

void copy_to_log_AewbHandle(log_AewbHandle *dest, AewbHandle *src)
{
    // Copying log_tivx_aewb_config_t
    dest->aewb_config.sensor_dcc_id       = src->aewb_config.sensor_dcc_id;
    dest->aewb_config.sensor_img_format   = src->aewb_config.sensor_img_format;
    dest->aewb_config.sensor_img_phase    = src->aewb_config.sensor_img_phase;
    dest->aewb_config.awb_mode            = src->aewb_config.awb_mode;
    dest->aewb_config.ae_mode             = src->aewb_config.ae_mode;
    dest->aewb_config.awb_num_skip_frames = src->aewb_config.awb_num_skip_frames;
    dest->aewb_config.ae_num_skip_frames  = src->aewb_config.ae_num_skip_frames;
    dest->aewb_config.channel_id          = src->aewb_config.channel_id;

    // Copying ti_2a_wrapper.dcc_input_params
    dest->ti_2a_wrapper.dcc_input_params.color_temparature = src->ti_2a_wrapper.dcc_input_params->color_temparature;
    dest->ti_2a_wrapper.dcc_input_params.exposure_time     = src->ti_2a_wrapper.dcc_input_params->exposure_time;
    dest->ti_2a_wrapper.dcc_input_params.analog_gain       = src->ti_2a_wrapper.dcc_input_params->analog_gain;

    // Copying ti_2a_wrapper.frame_data
    dest->ti_2a_wrapper.frame_data.h3a_data_x = src->ti_2a_wrapper.p_awb_params->ti_awb_data_in.frame_data.h3a_data_x;
    dest->ti_2a_wrapper.frame_data.h3a_data_y = src->ti_2a_wrapper.p_awb_params->ti_awb_data_in.frame_data.h3a_data_y;
    dest->ti_2a_wrapper.frame_data.pix_in_pax = src->ti_2a_wrapper.p_awb_params->ti_awb_data_in.frame_data.pix_in_pax;    

    // Copying ti_2a_wrapper.ae_params
    dest->ti_2a_wrapper.ae_params.mode                      = src->ti_2a_wrapper.p_ae_params->mode;
    dest->ti_2a_wrapper.ae_params.num_history               = src->ti_2a_wrapper.p_ae_params->num_history;
    dest->ti_2a_wrapper.ae_params.avg_y                     = src->ti_2a_wrapper.p_ae_params->avg_y;
    dest->ti_2a_wrapper.ae_params.locked                    = src->ti_2a_wrapper.p_ae_params->locked;
    dest->ti_2a_wrapper.ae_params.lock_cnt                  = src->ti_2a_wrapper.p_ae_params->lock_cnt;
    dest->ti_2a_wrapper.ae_params.lock_thrld                = src->ti_2a_wrapper.p_ae_params->lock_thrld;
    dest->ti_2a_wrapper.ae_params.blc_enable                = src->ti_2a_wrapper.p_ae_params->blc_enable;
    dest->ti_2a_wrapper.ae_params.blc_comp                  = src->ti_2a_wrapper.p_ae_params->blc_comp;
    dest->ti_2a_wrapper.ae_params.frame_num_count           = src->ti_2a_wrapper.p_ae_params->frame_num_count;
    dest->ti_2a_wrapper.ae_params.frame_num_start           = src->ti_2a_wrapper.p_ae_params->frame_num_start;
    dest->ti_2a_wrapper.ae_params.frame_num_period          = src->ti_2a_wrapper.p_ae_params->frame_num_period;  

    dest->ti_2a_wrapper.ae_params.exposure_program.target_brightness           = src->ti_2a_wrapper.p_ae_params->exposure_program.target_brightness;
    dest->ti_2a_wrapper.ae_params.exposure_program.target_brightness_range.min = src->ti_2a_wrapper.p_ae_params->exposure_program.target_brightness_range.min;
    dest->ti_2a_wrapper.ae_params.exposure_program.target_brightness_range.max = src->ti_2a_wrapper.p_ae_params->exposure_program.target_brightness_range.max;
    dest->ti_2a_wrapper.ae_params.exposure_program.exposure_time_step_size     = src->ti_2a_wrapper.p_ae_params->exposure_program.exposure_time_step_size;
    dest->ti_2a_wrapper.ae_params.exposure_program.num_ranges                  = src->ti_2a_wrapper.p_ae_params->exposure_program.num_ranges;
    dest->ti_2a_wrapper.ae_params.exposure_program.enableBLC                   = src->ti_2a_wrapper.p_ae_params->exposure_program.enableBLC;        
    for (int i = 0; i < LOG_TIAE_MAX_RANGES; i++) {
        dest->ti_2a_wrapper.ae_params.exposure_program.aperture_size_range[i].min = src->ti_2a_wrapper.p_ae_params->exposure_program.aperture_size_range[i].min;
        dest->ti_2a_wrapper.ae_params.exposure_program.aperture_size_range[i].max = src->ti_2a_wrapper.p_ae_params->exposure_program.aperture_size_range[i].max;
        dest->ti_2a_wrapper.ae_params.exposure_program.exposure_time_range[i].min = src->ti_2a_wrapper.p_ae_params->exposure_program.exposure_time_range[i].min;
        dest->ti_2a_wrapper.ae_params.exposure_program.exposure_time_range[i].max = src->ti_2a_wrapper.p_ae_params->exposure_program.exposure_time_range[i].max;
        dest->ti_2a_wrapper.ae_params.exposure_program.analog_gain_range[i].min   = src->ti_2a_wrapper.p_ae_params->exposure_program.analog_gain_range[i].min;
        dest->ti_2a_wrapper.ae_params.exposure_program.analog_gain_range[i].max   = src->ti_2a_wrapper.p_ae_params->exposure_program.analog_gain_range[i].max;
        dest->ti_2a_wrapper.ae_params.exposure_program.digital_gain_range[i].min  = src->ti_2a_wrapper.p_ae_params->exposure_program.digital_gain_range[i].min;
        dest->ti_2a_wrapper.ae_params.exposure_program.digital_gain_range[i].max  = src->ti_2a_wrapper.p_ae_params->exposure_program.digital_gain_range[i].max;
    }

    dest->ti_2a_wrapper.ae_awb_result_prev.exposure_time     = src->ti_2a_wrapper.ae_awb_result_prev.exposure_time;
    dest->ti_2a_wrapper.ae_awb_result_prev.analog_gain       = src->ti_2a_wrapper.ae_awb_result_prev.analog_gain;
    dest->ti_2a_wrapper.ae_awb_result_prev.ae_valid          = src->ti_2a_wrapper.ae_awb_result_prev.ae_valid;
    dest->ti_2a_wrapper.ae_awb_result_prev.ae_converged      = src->ti_2a_wrapper.ae_awb_result_prev.ae_converged;
    dest->ti_2a_wrapper.ae_awb_result_prev.digital_gain      = src->ti_2a_wrapper.ae_awb_result_prev.digital_gain;
    dest->ti_2a_wrapper.ae_awb_result_prev.wb_gains[0]       = src->ti_2a_wrapper.ae_awb_result_prev.wb_gains[0];
    dest->ti_2a_wrapper.ae_awb_result_prev.wb_gains[1]       = src->ti_2a_wrapper.ae_awb_result_prev.wb_gains[1];
    dest->ti_2a_wrapper.ae_awb_result_prev.wb_gains[2]       = src->ti_2a_wrapper.ae_awb_result_prev.wb_gains[2];
    dest->ti_2a_wrapper.ae_awb_result_prev.wb_gains[3]       = src->ti_2a_wrapper.ae_awb_result_prev.wb_gains[3];
    dest->ti_2a_wrapper.ae_awb_result_prev.wb_offsets[0]     = src->ti_2a_wrapper.ae_awb_result_prev.wb_offsets[0];
    dest->ti_2a_wrapper.ae_awb_result_prev.wb_offsets[1]     = src->ti_2a_wrapper.ae_awb_result_prev.wb_offsets[1];
    dest->ti_2a_wrapper.ae_awb_result_prev.wb_offsets[2]     = src->ti_2a_wrapper.ae_awb_result_prev.wb_offsets[2];
    dest->ti_2a_wrapper.ae_awb_result_prev.wb_offsets[3]     = src->ti_2a_wrapper.ae_awb_result_prev.wb_offsets[3];
    dest->ti_2a_wrapper.ae_awb_result_prev.color_temperature = src->ti_2a_wrapper.ae_awb_result_prev.color_temperature;
    dest->ti_2a_wrapper.ae_awb_result_prev.awb_valid         = src->ti_2a_wrapper.ae_awb_result_prev.awb_valid;
    dest->ti_2a_wrapper.ae_awb_result_prev.awb_converged     = src->ti_2a_wrapper.ae_awb_result_prev.awb_converged;

    for (int i = 0; i < LOG_TIAE_MAX_HIST; i++) {
        dest->ti_2a_wrapper.ae_params.history_brightness[i]  = src->ti_2a_wrapper.p_ae_params->history_brightness[i];
    }    

    // Copying ti_2a_wrapper.ae_awb_result_prev
    dest->ti_2a_wrapper.ae_awb_result_prev.h3a_source_data    = src->ti_2a_wrapper.ae_awb_result_prev.h3a_source_data;
    dest->ti_2a_wrapper.ae_awb_result_prev.exposure_time      = src->ti_2a_wrapper.ae_awb_result_prev.exposure_time;
    dest->ti_2a_wrapper.ae_awb_result_prev.analog_gain        = src->ti_2a_wrapper.ae_awb_result_prev.analog_gain;
    dest->ti_2a_wrapper.ae_awb_result_prev.ae_valid           = src->ti_2a_wrapper.ae_awb_result_prev.ae_valid;
    dest->ti_2a_wrapper.ae_awb_result_prev.ae_converged       = src->ti_2a_wrapper.ae_awb_result_prev.ae_converged;
    dest->ti_2a_wrapper.ae_awb_result_prev.digital_gain       = src->ti_2a_wrapper.ae_awb_result_prev.digital_gain;
    dest->ti_2a_wrapper.ae_awb_result_prev.color_temperature  = src->ti_2a_wrapper.ae_awb_result_prev.color_temperature;
    dest->ti_2a_wrapper.ae_awb_result_prev.awb_valid          = src->ti_2a_wrapper.ae_awb_result_prev.awb_valid;
    dest->ti_2a_wrapper.ae_awb_result_prev.awb_converged      = src->ti_2a_wrapper.ae_awb_result_prev.awb_converged;     
    for (int i = 0; i < 4; i++) {
        dest->ti_2a_wrapper.ae_awb_result_prev.wb_gains[i]    = src->ti_2a_wrapper.ae_awb_result_prev.wb_gains[i];
        dest->ti_2a_wrapper.ae_awb_result_prev.wb_offsets[i]  = src->ti_2a_wrapper.ae_awb_result_prev.wb_offsets[i];
    }


    // Copying ti_2a_wrapper other
    dest->ti_2a_wrapper.frame_count        = src->ti_2a_wrapper.frame_count;
    dest->ti_2a_wrapper.sensor_pre_wb_gain = src->ti_2a_wrapper.sensor_pre_wb_gain;
    dest->ti_2a_wrapper.skipAWB            = src->ti_2a_wrapper.skipAWB;
    dest->ti_2a_wrapper.skipAE             = src->ti_2a_wrapper.skipAE;

    // Copying ae_dynPrms
    dest->ae_dynPrms.targetBrightnessRange.min    = src->sensor_in_data.ae_dynPrms.targetBrightnessRange.min;
    dest->ae_dynPrms.targetBrightnessRange.max    = src->sensor_in_data.ae_dynPrms.targetBrightnessRange.max;
    dest->ae_dynPrms.targetBrightness             = src->sensor_in_data.ae_dynPrms.targetBrightness;
    dest->ae_dynPrms.threshold                    = src->sensor_in_data.ae_dynPrms.threshold;
    dest->ae_dynPrms.exposureTimeStepSize         = src->sensor_in_data.ae_dynPrms.exposureTimeStepSize;
    dest->ae_dynPrms.enableBlc                    = src->sensor_in_data.ae_dynPrms.enableBlc;
    dest->ae_dynPrms.numAeDynParams               = src->sensor_in_data.ae_dynPrms.numAeDynParams;
    for (int i = 0; i < LOG_MAX_AE_DYN_PARAMS; i++) {
        dest->ae_dynPrms.exposureTimeRange[i].min = src->sensor_in_data.ae_dynPrms.exposureTimeRange[i].min;
        dest->ae_dynPrms.exposureTimeRange[i].max = src->sensor_in_data.ae_dynPrms.exposureTimeRange[i].max;
        dest->ae_dynPrms.analogGainRange[i].min   = src->sensor_in_data.ae_dynPrms.analogGainRange[i].min;
        dest->ae_dynPrms.analogGainRange[i].max   = src->sensor_in_data.ae_dynPrms.analogGainRange[i].max;
        dest->ae_dynPrms.digitalGainRange[i].min  = src->sensor_in_data.ae_dynPrms.digitalGainRange[i].min;
        dest->ae_dynPrms.digitalGainRange[i].max  = src->sensor_in_data.ae_dynPrms.digitalGainRange[i].max;
    }
    
    // Copying aePrms
    dest->aePrms.chId                = src->sensor_out_data.aePrms.chId;
    dest->aePrms.expRatio            = src->sensor_out_data.aePrms.expRatio;
    for (int i = 0; i < LOG_ISS_SENSOR_MAX_EXPOSURE; i++) {
        dest->aePrms.exposureTime[i] = src->sensor_out_data.aePrms.exposureTime[i];
        dest->aePrms.analogGain[i]   = src->sensor_out_data.aePrms.analogGain[i];
    }
}

int32_t aewb_logger_write_log_to_buffer(AewbHandle *handle, log_AewbHandle *dest_buffer)
{
    copy_to_log_AewbHandle(dest_buffer, handle);
    return sizeof(log_AewbHandle);
}

int32_t aewb_logger_send_bytes(aewb_logger_sender_state_t *p_state)
{
    int32_t num_bytes_written = sendto(p_state->sock_fd, 
            (const int8_t *)&p_state->buffer, sizeof(p_state->buffer), MSG_CONFIRM, 
            (const struct sockaddr *)&p_state->dest_addr, sizeof(p_state->dest_addr)
    );

    if (num_bytes_written!=sizeof(log_AewbHandle))
        printf("aewb_logger_send_bytes: error num_bytes_written!=sizeof(log_AewbHandle), %d!=%lu",
                num_bytes_written, sizeof(log_AewbHandle));

    return num_bytes_written;
}

int32_t aewb_logger_send_log(aewb_logger_sender_state_t *p_state, AewbHandle *handle)
{
    if (p_state==NULL)
        return 0;

    memset(&p_state->buffer, 0, sizeof(p_state->buffer));

    aewb_logger_write_log_to_buffer(handle, &p_state->buffer);

    return aewb_logger_send_bytes(p_state);
}

void aewb_logger_destroy_sender(aewb_logger_sender_state_t *p_state) {
    close(p_state->sock_fd);
    free(p_state);
}