#ifndef _AE_PARAMS_H
#define _AE_PARAMS_H

#include <iss_sensors.h>


#ifdef __cplusplus
extern "C" {
#endif

extern int ae_params_get(IssAeDynamicParams *p_params);
extern int ae_params_get_params_from_yaml(const char *path, IssAeDynamicParams *p_params);
extern void ae_params_dump(IssAeDynamicParams *p_ae_dynPrms);

#ifdef __cplusplus
}
#endif


#endif // _AE_PARAMS_H