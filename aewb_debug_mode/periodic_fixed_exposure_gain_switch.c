#include "periodic_fixed_exposure_gain_switch.h"

typedef struct {
    periodic_fixed_exposure_gain_switch_config_t config;
    uint32_t frame_counter;
    uint32_t config_index;

} state_t;

static state_t state;


void periodic_fixed_exposure_gain_switch_init()
{
    state.config_index = 0;
    state.frame_counter = 0;
}

void periodic_fixed_exposure_gain_switch_set_config(periodic_fixed_exposure_gain_switch_config_t *config)
{
    memcpy(&state.config, config, sizeof(periodic_fixed_exposure_gain_switch_config_t));
}

void periodic_fixed_exposure_gain_switch_run_if_en(sensor_config_set *sensor_out_data, tivx_ae_awb_params_t *aewb_ptr)
{
    if (state.config.enable==0)
        return;

    sensor_out_data->aePrms.exposureTime[0] = state.config.configs[state.config_index].exposure_usec;
    sensor_out_data->aePrms.analogGain[0] = state.config.configs[state.config_index].analog_gain;
    aewb_ptr->exposure_time = sensor_out_data->aePrms.exposureTime[0];
    aewb_ptr->analog_gain = sensor_out_data->aePrms.analogGain[0];        

    if (state.frame_counter>=state.config.num_frames_per_config-1)
    {
        state.frame_counter = 0;
        state.config_index = (state.config_index+1)%state.config.num_configs;
    }
    else
    {
        state.frame_counter += 1;
    }
}