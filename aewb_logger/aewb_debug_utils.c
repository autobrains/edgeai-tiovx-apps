#include "aewb_debug_utils.h"
#include <time.h>
#include <math.h>
#include "linux_aewb_module.h"

typedef struct {
    uint16_t subsample_accum[4];
    uint16_t saturator_accum[4];
    // uint32_t sum_sq_minmax[4];
    //  uint16_t min[4];
    //  uint16_t max[4];
} h3a_win_t;

typedef struct {
    h3a_win_t windows[8];
    uint16_t unsaturated_count[8];
} h3a_win8_t;


time_t t0 = 0;
int gain_i = 0;
void aewb_debug_periodic_change(tiae_prm_t *h)
{
    if (time(NULL)-t0<5) {
        return;
    }

    int i;

    h->num_history = 10;
    for (i = 0; i < h->num_history; i++)
    {
        h->history_brightness[i] = -1;
    }
    h->avg_y = -1;
    h->locked = 0;
    h->lock_cnt = 0;
    h->lock_thrld = 10;

    h->blc_enable = 0;
    h->blc_comp = 1024;

    h->prev_ae.aperture_size = 1;
    h->prev_ae.exposure_time = 100;
    h->prev_ae.analog_gain   = 1024;
    h->prev_ae.digital_gain  = 256;

    h->frame_num_start = 0;
    h->frame_num_period = 3;
    h->frame_num_count = 0;

    h->exposure_program.analog_gain_range[0].max = 1024*pow(1.2,gain_i);       

    t0 = time(NULL);
    gain_i += 1;
    printf("------------------ gain_i=%d, max=%d\n", gain_i, h->exposure_program.analog_gain_range[0].max);
}

void dump_dcc(TI_2A_wrapper *ti_2a_wrapper) {

        printf("------------------- sat %d, alaw %d, sumshift %d, mode %d, pix_in_pax %d\n",
            ti_2a_wrapper->dcc_output_params->ipipeH3A_AEWBCfg.saturation_limit,
            ti_2a_wrapper->dcc_output_params->ipipeH3A_AEWBCfg.ALaw_En,
            ti_2a_wrapper->dcc_output_params->ipipeH3A_AEWBCfg.sum_shift,
            ti_2a_wrapper->dcc_output_params->ipipeH3A_AEWBCfg.mode,
            ti_2a_wrapper->p_awb_params->ti_awb_data_in.frame_data.pix_in_pax
    );   
}

void dump_sample(tivx_h3a_data_t *h3a_ptr) {
    h3a_win8_t *win_ptr = (h3a_win8_t *)h3a_ptr->data;
    h3a_win8_t *win_8_x = &win_ptr[0];    
    h3a_win_t *win_8_0 = &(win_8_x->windows[0]);

    printf("subsample_accum   % 8d % 8d % 8d % 8d\n",   win_8_0->subsample_accum[0],   win_8_0->subsample_accum[1],   win_8_0->subsample_accum[2],   win_8_0->subsample_accum[3]);
    printf("saturator_accum   % 8d % 8d % 8d % 8d\n",   win_8_0->saturator_accum[0],   win_8_0->saturator_accum[1],   win_8_0->saturator_accum[2],   win_8_0->saturator_accum[3]);
    // printf("min               % 8d % 8d % 8d % 8d\n",   win_8_0->min[0],               win_8_0->min[1],               win_8_0->min[2],               win_8_0->min[3]);
    // printf("max               % 8d % 8d % 8d % 8d\n",   win_8_0->max[0],               win_8_0->max[1],               win_8_0->max[2],               win_8_0->max[3]);        
    printf("unsaturated_count % 8d % 8d % 8d % 8d\n",   win_8_x->unsaturated_count[0], win_8_x->unsaturated_count[1], win_8_x->unsaturated_count[2], win_8_x->unsaturated_count[3]);    

}


