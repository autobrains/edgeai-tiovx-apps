#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include "aewb_logger_sender.h"
#include "aewb_logger_types.h"

void get_ip_port_with_envvar_override(const char *in_dest_ip, in_port_t in_dest_port, bool *out_enable, const char **out_dest_ip, in_port_t *out_dest_port) {
    const char *env_aewb_log_en_str = getenv("AEWB_LOG_EN");
    const char *env_aewb_log_dest_ip_str = getenv("AEWB_LOG_DEST_IP");
    const char *env_aewb_log_dest_port_str = getenv("AEWB_LOG_DEST_PORT");
    in_port_t env_aewb_log_dest_port = 0U;
    
    if (env_aewb_log_en_str && strcmp(env_aewb_log_en_str, "0") == 0U) {
        *out_enable = false;
    }
    else {
        *out_enable = true;
    }

    if (env_aewb_log_dest_ip_str) {
         // env var ip takes precedence over function arg
        *out_dest_ip = env_aewb_log_dest_ip_str;
    }
    else {
        *out_dest_ip = in_dest_ip;
    }

    if (env_aewb_log_dest_port_str) {
         // env var port takes precedence over function arg
        errno = 0U;
        env_aewb_log_dest_port = strtol(env_aewb_log_dest_port_str, NULL, 10);
        if (errno)
            env_aewb_log_dest_port = 0U;
    }

    if (env_aewb_log_dest_port > 0U) {
        *out_dest_port = env_aewb_log_dest_port;
    }
    else { 
        *out_dest_port = (in_port_t)in_dest_port;
    }
}

aewb_logger_sender_state_t *aewb_logger_create_sender(const char *in_dest_ip, in_port_t in_dest_port) {
    const char *final_dest_ip = NULL;
    in_port_t final_dest_port = {0U};
    bool final_enable = false;

    get_ip_port_with_envvar_override(in_dest_ip, in_dest_port, &final_enable, &final_dest_ip, &final_dest_port);
    if (final_enable == false) {
        return NULL; // future aewb_logger_send_log() calls will not send anything
    }

    printf("aewb_logger_create_sender: enable %d, ip %s, port %d\n", final_enable, final_dest_ip, final_dest_port);

    aewb_logger_sender_state_t *p_state = malloc(sizeof(aewb_logger_sender_state_t));
    memset(p_state, 0, sizeof(aewb_logger_sender_state_t));

    // Creating socket file descriptor
    if ((p_state->sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        return NULL;
    }

    p_state->dest_addr.sin_family = AF_INET;
    p_state->dest_addr.sin_port = htons(final_dest_port);
    if (inet_pton(AF_INET, final_dest_ip, &p_state->dest_addr.sin_addr) <= 0U) {
        perror("Invalid address/ Address not supported");
        return NULL;
    }

    return p_state;   
}

void copy_to_log_message(log_aewb_message_t *dest, AewbHandle *src_handle, tivx_h3a_data_t *src_h3a_data, tivx_ae_awb_params_t *src_ae_awb_result)
{
    // Copying log_tivx_aewb_config_t
    dest->handle.aewb_config.sensor_dcc_id       = src_handle->aewb_config.sensor_dcc_id;
    dest->handle.aewb_config.sensor_img_format   = src_handle->aewb_config.sensor_img_format;
    dest->handle.aewb_config.sensor_img_phase    = src_handle->aewb_config.sensor_img_phase;
    dest->handle.aewb_config.awb_mode            = src_handle->aewb_config.awb_mode;
    dest->handle.aewb_config.ae_mode             = src_handle->aewb_config.ae_mode;
    dest->handle.aewb_config.awb_num_skip_frames = src_handle->aewb_config.awb_num_skip_frames;
    dest->handle.aewb_config.ae_num_skip_frames  = src_handle->aewb_config.ae_num_skip_frames;
    dest->handle.aewb_config.channel_id          = src_handle->aewb_config.channel_id;

    // Copying ti_2a_wrapper.dcc_input_params
    dest->handle.ti_2a_wrapper.dcc_input_params.color_temparature = src_handle->ti_2a_wrapper.dcc_input_params->color_temparature;
    dest->handle.ti_2a_wrapper.dcc_input_params.exposure_time     = src_handle->ti_2a_wrapper.dcc_input_params->exposure_time;
    dest->handle.ti_2a_wrapper.dcc_input_params.analog_gain       = src_handle->ti_2a_wrapper.dcc_input_params->analog_gain;

    // Copying ti_2a_wrapper.frame_data
    dest->handle.ti_2a_wrapper.frame_data.h3a_data_x = src_handle->ti_2a_wrapper.p_awb_params->ti_awb_data_in.frame_data.h3a_data_x;
    dest->handle.ti_2a_wrapper.frame_data.h3a_data_y = src_handle->ti_2a_wrapper.p_awb_params->ti_awb_data_in.frame_data.h3a_data_y;
    dest->handle.ti_2a_wrapper.frame_data.pix_in_pax = src_handle->ti_2a_wrapper.p_awb_params->ti_awb_data_in.frame_data.pix_in_pax;    

    // Copying ti_2a_wrapper.ae_params
    dest->handle.ti_2a_wrapper.ae_params.mode                      = src_handle->ti_2a_wrapper.p_ae_params->mode;
    dest->handle.ti_2a_wrapper.ae_params.num_history               = src_handle->ti_2a_wrapper.p_ae_params->num_history;
    dest->handle.ti_2a_wrapper.ae_params.avg_y                     = src_handle->ti_2a_wrapper.p_ae_params->avg_y;
    dest->handle.ti_2a_wrapper.ae_params.locked                    = src_handle->ti_2a_wrapper.p_ae_params->locked;
    dest->handle.ti_2a_wrapper.ae_params.lock_cnt                  = src_handle->ti_2a_wrapper.p_ae_params->lock_cnt;
    dest->handle.ti_2a_wrapper.ae_params.lock_thrld                = src_handle->ti_2a_wrapper.p_ae_params->lock_thrld;
    dest->handle.ti_2a_wrapper.ae_params.blc_enable                = src_handle->ti_2a_wrapper.p_ae_params->blc_enable;
    dest->handle.ti_2a_wrapper.ae_params.blc_comp                  = src_handle->ti_2a_wrapper.p_ae_params->blc_comp;
    dest->handle.ti_2a_wrapper.ae_params.frame_num_count           = src_handle->ti_2a_wrapper.p_ae_params->frame_num_count;
    dest->handle.ti_2a_wrapper.ae_params.frame_num_start           = src_handle->ti_2a_wrapper.p_ae_params->frame_num_start;
    dest->handle.ti_2a_wrapper.ae_params.frame_num_period          = src_handle->ti_2a_wrapper.p_ae_params->frame_num_period;  
    
    dest->handle.ti_2a_wrapper.ae_params.prev_ae.aperture_size          = src_handle->ti_2a_wrapper.p_ae_params->prev_ae.aperture_size;  
    dest->handle.ti_2a_wrapper.ae_params.prev_ae.exposure_time          = src_handle->ti_2a_wrapper.p_ae_params->prev_ae.exposure_time;  
    dest->handle.ti_2a_wrapper.ae_params.prev_ae.analog_gain            = src_handle->ti_2a_wrapper.p_ae_params->prev_ae.analog_gain;  
    dest->handle.ti_2a_wrapper.ae_params.prev_ae.digital_gain           = src_handle->ti_2a_wrapper.p_ae_params->prev_ae.digital_gain;  

    dest->handle.ti_2a_wrapper.ae_params.exposure_program.target_brightness           = src_handle->ti_2a_wrapper.p_ae_params->exposure_program.target_brightness;
    dest->handle.ti_2a_wrapper.ae_params.exposure_program.target_brightness_range.min = src_handle->ti_2a_wrapper.p_ae_params->exposure_program.target_brightness_range.min;
    dest->handle.ti_2a_wrapper.ae_params.exposure_program.target_brightness_range.max = src_handle->ti_2a_wrapper.p_ae_params->exposure_program.target_brightness_range.max;
    dest->handle.ti_2a_wrapper.ae_params.exposure_program.exposure_time_step_size     = src_handle->ti_2a_wrapper.p_ae_params->exposure_program.exposure_time_step_size;
    dest->handle.ti_2a_wrapper.ae_params.exposure_program.num_ranges                  = src_handle->ti_2a_wrapper.p_ae_params->exposure_program.num_ranges;
    dest->handle.ti_2a_wrapper.ae_params.exposure_program.enableBLC                   = src_handle->ti_2a_wrapper.p_ae_params->exposure_program.enableBLC;        
    for (int i = 0; i < LOG_TIAE_MAX_RANGES; i++) {
        dest->handle.ti_2a_wrapper.ae_params.exposure_program.aperture_size_range[i].min = src_handle->ti_2a_wrapper.p_ae_params->exposure_program.aperture_size_range[i].min;
        dest->handle.ti_2a_wrapper.ae_params.exposure_program.aperture_size_range[i].max = src_handle->ti_2a_wrapper.p_ae_params->exposure_program.aperture_size_range[i].max;
        dest->handle.ti_2a_wrapper.ae_params.exposure_program.exposure_time_range[i].min = src_handle->ti_2a_wrapper.p_ae_params->exposure_program.exposure_time_range[i].min;
        dest->handle.ti_2a_wrapper.ae_params.exposure_program.exposure_time_range[i].max = src_handle->ti_2a_wrapper.p_ae_params->exposure_program.exposure_time_range[i].max;
        dest->handle.ti_2a_wrapper.ae_params.exposure_program.analog_gain_range[i].min   = src_handle->ti_2a_wrapper.p_ae_params->exposure_program.analog_gain_range[i].min;
        dest->handle.ti_2a_wrapper.ae_params.exposure_program.analog_gain_range[i].max   = src_handle->ti_2a_wrapper.p_ae_params->exposure_program.analog_gain_range[i].max;
        dest->handle.ti_2a_wrapper.ae_params.exposure_program.digital_gain_range[i].min  = src_handle->ti_2a_wrapper.p_ae_params->exposure_program.digital_gain_range[i].min;
        dest->handle.ti_2a_wrapper.ae_params.exposure_program.digital_gain_range[i].max  = src_handle->ti_2a_wrapper.p_ae_params->exposure_program.digital_gain_range[i].max;
    }

    for (int i = 0; i < LOG_TIAE_MAX_HIST; i++) {
        dest->handle.ti_2a_wrapper.ae_params.history_brightness[i]  = src_handle->ti_2a_wrapper.p_ae_params->history_brightness[i];
    }    

    // Copying ti_2a_wrapper other
    dest->handle.ti_2a_wrapper.frame_count        = src_handle->ti_2a_wrapper.frame_count;
    dest->handle.ti_2a_wrapper.sensor_pre_wb_gain = src_handle->ti_2a_wrapper.sensor_pre_wb_gain;
    dest->handle.ti_2a_wrapper.skipAWB            = src_handle->ti_2a_wrapper.skipAWB;
    dest->handle.ti_2a_wrapper.skipAE             = src_handle->ti_2a_wrapper.skipAE;

    // Copying ae_dynPrms
    dest->handle.ae_dynPrms.targetBrightnessRange.min    = src_handle->sensor_in_data.ae_dynPrms.targetBrightnessRange.min;
    dest->handle.ae_dynPrms.targetBrightnessRange.max    = src_handle->sensor_in_data.ae_dynPrms.targetBrightnessRange.max;
    dest->handle.ae_dynPrms.targetBrightness             = src_handle->sensor_in_data.ae_dynPrms.targetBrightness;
    dest->handle.ae_dynPrms.threshold                    = src_handle->sensor_in_data.ae_dynPrms.threshold;
    dest->handle.ae_dynPrms.exposureTimeStepSize         = src_handle->sensor_in_data.ae_dynPrms.exposureTimeStepSize;
    dest->handle.ae_dynPrms.enableBlc                    = src_handle->sensor_in_data.ae_dynPrms.enableBlc;
    dest->handle.ae_dynPrms.numAeDynParams               = src_handle->sensor_in_data.ae_dynPrms.numAeDynParams;
    for (int i = 0; i < LOG_MAX_AE_DYN_PARAMS; i++) {
        dest->handle.ae_dynPrms.exposureTimeRange[i].min = src_handle->sensor_in_data.ae_dynPrms.exposureTimeRange[i].min;
        dest->handle.ae_dynPrms.exposureTimeRange[i].max = src_handle->sensor_in_data.ae_dynPrms.exposureTimeRange[i].max;
        dest->handle.ae_dynPrms.analogGainRange[i].min   = src_handle->sensor_in_data.ae_dynPrms.analogGainRange[i].min;
        dest->handle.ae_dynPrms.analogGainRange[i].max   = src_handle->sensor_in_data.ae_dynPrms.analogGainRange[i].max;
        dest->handle.ae_dynPrms.digitalGainRange[i].min  = src_handle->sensor_in_data.ae_dynPrms.digitalGainRange[i].min;
        dest->handle.ae_dynPrms.digitalGainRange[i].max  = src_handle->sensor_in_data.ae_dynPrms.digitalGainRange[i].max;
    }
    
    // Copying aePrms
    dest->handle.aePrms.chId                = src_handle->sensor_out_data.aePrms.chId;
    dest->handle.aePrms.expRatio            = src_handle->sensor_out_data.aePrms.expRatio;
    for (int i = 0; i < LOG_ISS_SENSOR_MAX_EXPOSURE; i++) {
        dest->handle.aePrms.exposureTime[i] = src_handle->sensor_out_data.aePrms.exposureTime[i];
        dest->handle.aePrms.analogGain[i]   = src_handle->sensor_out_data.aePrms.analogGain[i];
    }

    dest->h3a_data.aew_af_mode                  = src_h3a_data->aew_af_mode;
    dest->h3a_data.h3a_source_data              = src_h3a_data->h3a_source_data;
    dest->h3a_data.aew_config.aewwin1_WINH      = src_h3a_data->aew_config.aewwin1_WINH;
    dest->h3a_data.aew_config.aewwin1_WINW      = src_h3a_data->aew_config.aewwin1_WINW;
    dest->h3a_data.aew_config.aewwin1_WINVC     = src_h3a_data->aew_config.aewwin1_WINVC;
    dest->h3a_data.aew_config.aewwin1_WINHC     = src_h3a_data->aew_config.aewwin1_WINHC;
    dest->h3a_data.aew_config.aewsubwin_AEWINCV = src_h3a_data->aew_config.aewsubwin_AEWINCV;
    dest->h3a_data.aew_config.aewsubwin_AEWINCH = src_h3a_data->aew_config.aewsubwin_AEWINCH;

    // Copying ae_awb_result
    dest->ae_awb_result.h3a_source_data    = src_ae_awb_result->h3a_source_data;
    dest->ae_awb_result.exposure_time      = src_ae_awb_result->exposure_time;
    dest->ae_awb_result.analog_gain        = src_ae_awb_result->analog_gain;
    dest->ae_awb_result.ae_valid           = src_ae_awb_result->ae_valid;
    dest->ae_awb_result.ae_converged       = src_ae_awb_result->ae_converged;
    dest->ae_awb_result.digital_gain       = src_ae_awb_result->digital_gain;
    dest->ae_awb_result.color_temperature  = src_ae_awb_result->color_temperature;
    dest->ae_awb_result.awb_valid          = src_ae_awb_result->awb_valid;
    dest->ae_awb_result.awb_converged      = src_ae_awb_result->awb_converged;     
    for (int i = 0; i < 4; i++) {
        dest->ae_awb_result.wb_gains[i]    = src_ae_awb_result->wb_gains[i];
        dest->ae_awb_result.wb_offsets[i]  = src_ae_awb_result->wb_offsets[i];
    }
}

int32_t aewb_logger_write_log_to_buffer(AewbHandle *handle, tivx_h3a_data_t *h3a_ptr, tivx_ae_awb_params_t *ae_awb_result, log_aewb_message_t *dest_buffer)
{
    copy_to_log_message(dest_buffer, handle, h3a_ptr, ae_awb_result);
    return sizeof(log_AewbHandle);
}

int32_t aewb_logger_send_bytes(aewb_logger_sender_state_t *p_state)
{
    int32_t num_bytes_written = sendto(p_state->sock_fd, 
            (const int8_t *)&p_state->buffer, sizeof(p_state->buffer), MSG_CONFIRM, 
            (const struct sockaddr *)&p_state->dest_addr, sizeof(p_state->dest_addr)
    );

    if (num_bytes_written != sizeof(p_state->buffer)) {
        printf("aewb_logger_send_bytes: error num_bytes_written!=sizeof(p_state->buffer), %d!=%lu\n",
                num_bytes_written, sizeof(p_state->buffer));

    return num_bytes_written;
}

int32_t aewb_logger_send_log(aewb_logger_sender_state_t *p_state, AewbHandle *handle, tivx_h3a_data_t *h3a_ptr, tivx_ae_awb_params_t *ae_awb_result)
{
    if (p_state==NULL)
        return 0;

    memset(&p_state->buffer, 0, sizeof(p_state->buffer));

    clock_gettime(CLOCK_REALTIME, &p_state->buffer.header.timestamp);

    aewb_logger_write_log_to_buffer(handle, h3a_ptr, ae_awb_result, &p_state->buffer);

    return aewb_logger_send_bytes(p_state);
}

void aewb_logger_destroy_sender(aewb_logger_sender_state_t *p_state) {
    close(p_state->sock_fd);
    free(p_state);
}
