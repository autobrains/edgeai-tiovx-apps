#ifndef _AE_PARAMS_H
#define _AE_PARAMS_H

#include <iss_sensors.h>


#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t exposure_usec;
    uint32_t analog_gain; // 1024 = x1
} fixed_exposure_gain_config_t;

typedef struct {
    uint8_t enable;
    uint32_t num_frames_per_config;
    uint32_t num_configs;
    fixed_exposure_gain_config_t configs[20];
} periodic_fixed_exposure_gain_switch_config_t;

typedef struct {
    IssAeDynamicParams dyn_params;
    int cur_y_from_cc_pixels;
    periodic_fixed_exposure_gain_switch_config_t periodic_fixed_exposure_gain_switch;
} ae_params_t;

extern int ae_params_get(ae_params_t *p_params);
extern int ae_params_get_params_from_yaml(const char *path, ae_params_t *p_params);
extern void ae_params_dump(ae_params_t *p_params);

#ifdef __cplusplus
}
#endif


#endif // _AE_PARAMS_H