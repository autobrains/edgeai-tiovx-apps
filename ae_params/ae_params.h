#ifndef _AE_PARAMS_H
#define _AE_PARAMS_H

#include <iss_sensors.h>


#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    IssAeDynamicParams dyn_params;
    int cur_y_from_cc_pixels;
} ae_params_t;

extern int ae_params_get(ae_params_t *p_params);
extern int ae_params_get_params_from_yaml(const char *path, ae_params_t *p_params);
extern void ae_params_dump(ae_params_t *p_params);

#ifdef __cplusplus
}
#endif


#endif // _AE_PARAMS_H