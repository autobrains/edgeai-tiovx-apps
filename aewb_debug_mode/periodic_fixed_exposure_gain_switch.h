#ifndef _PERIODIC_FIXED_EXPOSURE_GAIN_SWITCH_
#define _PERIODIC_FIXED_EXPOSURE_GAIN_SWITCH_


#include "linux_aewb_module.h"
#include "ti_2a_wrapper.h"
#include "ae_params.h"

void periodic_fixed_exposure_gain_switch_init();
void periodic_fixed_exposure_gain_switch_set_config(periodic_fixed_exposure_gain_switch_config_t *config);
void periodic_fixed_exposure_gain_switch_run_if_en(sensor_config_set *sensor_out_data, tivx_ae_awb_params_t *aewb_ptr);

#endif //_PERIODIC_FIXED_EXPOSURE_GAIN_SWITCH_