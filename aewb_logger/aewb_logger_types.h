#ifndef _AEWB_LOGGER_TYPES_H
#define _AEWB_LOGGER_TYPES_H
#include <time.h>
#include <stdint.h>

#define LOG_TIAE_MAX_RANGES 16U
#define LOG_TIAE_MAX_HIST   16U
#define LOG_MAX_AE_DYN_PARAMS 10U
#define LOG_ISS_SENSOR_MAX_EXPOSURE 3U

typedef struct
{
    uint16_t sensor_dcc_id; /*!<DCC ID of the sensor. Must correspond to the value specified in tuning XML files */
    uint8_t sensor_img_format; /*!<Image Format : BAYER = 0x0, Rest unsupported */
    uint8_t sensor_img_phase; /*!<Image Format : BGGR = 0, GBRG = 1, GRBG = 2, RGGB = 3 */
    uint8_t awb_mode; /*!<AWB Mode : 0 = Auto, 1 = Manual, 2 = Disabled */
    uint8_t ae_mode; /*!<AE Mode : 0 = Auto, 1 = Manual, 2 = Disabled */
    uint8_t awb_num_skip_frames; /*!<0 = Process every frame */
    uint8_t ae_num_skip_frames; /*!<0 = Process every frame */
    uint8_t channel_id; /*!<channel ID */
} log_tivx_aewb_config_t;

typedef struct
{
    // uint8_t  *dcc_buf;
    // uint32_t dcc_buf_size;
    uint32_t color_temparature;
    uint32_t exposure_time;
    uint32_t analog_gain;
    // uint32_t cameraId;
} log_dcc_parser_input_params_t;

typedef struct {
    int32_t min;
    int32_t max;
} log_tiae_range_t;

typedef struct {
    int32_t      target_brightness;                     // linear: e.g., 45 (45/256 = 17.6%)
    log_tiae_range_t target_brightness_range;               // linear: e.g., (40, 50)
    int32_t      exposure_time_step_size;               // linear: use 1000 for 1ms
    int32_t      num_ranges;                            // <= TIAE_RANGE_T
    log_tiae_range_t aperture_size_range[LOG_TIAE_MAX_RANGES];  // linear as in tiae_exp_t
    log_tiae_range_t exposure_time_range[LOG_TIAE_MAX_RANGES];  // linear as in tiae_exp_t
    log_tiae_range_t analog_gain_range  [LOG_TIAE_MAX_RANGES];  // linear as in tiae_exp_t
    log_tiae_range_t digital_gain_range [LOG_TIAE_MAX_RANGES];  // linear as in tiae_exp_t
    uint32_t     enableBLC;  // BackLightCompensation
} log_tiae_exp_prog_t;

typedef struct {
    int32_t aperture_size;  // linear: e.g., keep constant 1 if not used
    int32_t exposure_time;  // linear: e.g., use 1000 for 1ms
    int32_t analog_gain;    // linear: e.g., use 1024 for 1.0x gain
    int32_t digital_gain;   // linear: e.g., use 256 for 1.0x gain
} log_tiae_exp_t;

typedef struct {
    uint16_t            mode;                     // 0: auto, 1: manual, 2 : disabled
    log_tiae_exp_prog_t exposure_program;
    int32_t             num_history;
    int32_t             history_brightness[LOG_TIAE_MAX_HIST];
    int32_t             avg_y;
    int32_t             locked;
    int32_t             lock_cnt;
    int32_t             lock_thrld;
    int32_t             blc_enable;
    int32_t             blc_comp;
    log_tiae_exp_t      prev_ae;
    int32_t             frame_num_count;
    int32_t             frame_num_start;
    int32_t             frame_num_period;
} log_tiae_prm_t;

typedef struct {
    uint16_t red;   // Average value for red pixels in current paxel
    uint16_t green; // Average value for green pixels in current paxel
    uint16_t blue;  // Average value for blue pixels in current paxel
    uint16_t ext;
} log_h3a_aewb_paxel_data_t;

typedef struct {
    log_h3a_aewb_paxel_data_t *h3a_res;     // pointer to input data for AWB
    uint16_t               h3a_data_x;  // X size of H3A data, blocks (paxels)
    uint16_t               h3a_data_y;  // Y size of H3A data, blocks (paxels)
    uint16_t               pix_in_pax;
} log_awb_frame_data_t;

typedef struct {
    /*! Indicates the source data corresponding to this data:
     *  |          Enum                       |   Description                    |
     *  |:------------------------------------|:---------------------------------|
     *  | \ref TIVX_VPAC_VISS_H3A_IN_RAW0     | H3A Input is from RAW0           |
     *  | \ref TIVX_VPAC_VISS_H3A_IN_RAW1     | H3A Input is from RAW1           |
     *  | \ref TIVX_VPAC_VISS_H3A_IN_RAW2     | H3A Input is from RAW2           |
     *  | \ref TIVX_VPAC_VISS_H3A_IN_LSC      | H3A Input is from LSC            |
     *  | \ref TIVX_VPAC_VISS_H3A_IN_PCID     | H3A Input is from PCID           |
     *  \ref TIVX_VPAC_VISS_H3A_IN_PCID is applicable for only VPAC3L
     */
    uint32_t                    h3a_source_data;
    /*! Measured in micro seconds (us) */
    uint32_t                    exposure_time;
    /*! Analog Gain */
    uint32_t                    analog_gain;
    /*! Is AE output valid? */
    uint32_t                    ae_valid;
    /*! Is AE converged? */
    uint32_t                    ae_converged;
    /*! Digital Gain */
    uint32_t                    digital_gain;
    /*! White Balance Gains */
    uint32_t                    wb_gains[4];
    /*! White Balance Offsets */
    int32_t                     wb_offsets[4];
    /*! Color Temperature (K) */
    uint32_t                    color_temperature;
    /*! Is AWB output valid? */
    uint32_t                    awb_valid;
    /*! Is AWB converged? */
    uint32_t                    awb_converged;
} log_tivx_ae_awb_params_t;

typedef struct
{
    /* Input to DCC parser */
    log_dcc_parser_input_params_t dcc_input_params;

    /* Output from DCC parser */
    // dcc_parser_output_params_t dcc_output_params;

    /* Pointer to AWB params */
    log_awb_frame_data_t frame_data; // awbprm_t p_awb_params;

    /* Pointer to AE params */
    log_tiae_prm_t ae_params;

    /* Pointer to H3A merge buffer */
    // h3a_aewb_paxel_data_t * p_h3a_merge;

    /* Pointer to updated H3A result data */
    // h3a_aewb_paxel_data_t * awb_h3a_res;

    /* Pointer to AWB scratch buffer */
    // uint8_t * scratch_memory;

    /* Pointer to previous AEWB result */
    // log_tivx_ae_awb_params_t ae_awb_result_prev;

    /* Index of frame being currently processed */
    uint32_t  frame_count;

    /* Flag to indicate if sensor applies White Balance correction pre-HDR merge.
    1=True, 0=False */
    int32_t   sensor_pre_wb_gain;

    /* Flag to indicate if AWB was skipped by TI_2A_wrapper_process function
    1=YES, 0=NO */
    uint8_t skipAWB;

    /* Flag to indicate if AE was skipped by TI_2A_wrapper_process function
    1=YES, 0=NO */
    uint8_t skipAE;
} log_TI_2A_wrapper;

typedef struct {
    uint32_t min;
    /**< Min Value */
    uint32_t max;
    /**< Max Value */
} log_IssAeRange;

typedef struct {
    log_IssAeRange exposureTimeRange[LOG_MAX_AE_DYN_PARAMS];
    /**< range of exposure time in nanoseconds */
    log_IssAeRange analogGainRange[LOG_MAX_AE_DYN_PARAMS];
    /**< range of sensor gain, 1024 = 1x */
    log_IssAeRange digitalGainRange[LOG_MAX_AE_DYN_PARAMS];
    /**< range of ISP Digital gain, 256 = 1x */
    uint32_t                     numAeDynParams;
    /**< Number of Valid Entries in above arrays */
    log_IssAeRange targetBrightnessRange;
    /**< range of target brightness */
    uint32_t                     targetBrightness;
    /**< target brightness value */
    uint32_t                     threshold;
    /**< threshold for not using history brightness information */
    uint32_t                     exposureTimeStepSize;
    /**< step size of exposure time adjustment */
    uint32_t                     enableBlc;
    /**< TRUE enables Backlight compensation, disabled otherwise */
} log_IssAeDynamicParams;

typedef struct
{
    uint32_t chId;
    /**< Channel Id */
    uint32_t expRatio;
    /**< Exposure ratio for WDR output format, not used for linear mode */
    uint32_t exposureTime[LOG_ISS_SENSOR_MAX_EXPOSURE];
    /**< Exposure time in ms for all exposure outputs,
         For Linear mode output, only Long exposure entry is used */
    uint32_t analogGain[LOG_ISS_SENSOR_MAX_EXPOSURE];
    /**< Exposure time in ms for all exposure outputs,
         For Linear mode output, only Long exposure entry is used */
} log_IssSensor_ExposureParams;

typedef struct {
    // AewbCfg             cfg;
    log_tivx_aewb_config_t  aewb_config;
    // SensorObj           sensor_obj;
    // uint8_t             *dcc_2a_buf;
    // uint32_t            dcc_2a_buf_size;
    log_TI_2A_wrapper       ti_2a_wrapper;
    log_IssAeDynamicParams ae_dynPrms; //sensor_config_get   sensor_in_data;
    log_IssSensor_ExposureParams aePrms; //sensor_config_set   sensor_out_data;    
    // int fd;
} log_AewbHandle;

typedef struct
{
    /*! AEW Window Height */
    uint16_t                    aewwin1_WINH;
    /*! AEW Window Width */
    uint16_t                    aewwin1_WINW;
    /*! AEW Vertical Count */
    uint16_t                    aewwin1_WINVC;
    /*! AEW Horizontal Count */
    uint16_t                    aewwin1_WINHC;
    /*! AEW Subwindow Vertical Increment Value */
    uint16_t                    aewsubwin_AEWINCV;
    /*! AEW Subwindow Horizontal Increment Value */
    uint16_t                    aewsubwin_AEWINCH;
} log_tivx_h3a_aew_config;


typedef struct {
    /*! Indicates the contents of this buffer:
     *  |          Enum                       |   Description                    |
     *  |:------------------------------------|:---------------------------------|
     *  | \ref TIVX_VPAC_VISS_H3A_MODE_AEWB   | AEWB                             |
     *  | \ref TIVX_VPAC_VISS_H3A_MODE_AF     | AF                               |
     */
    uint32_t                    aew_af_mode;
    /*! Indicates the source data corresponding to this data:
     *  |          Enum                       |   Description                    |
     *  |:------------------------------------|:---------------------------------|
     *  | \ref TIVX_VPAC_VISS_H3A_IN_RAW0     | H3A Input is from RAW0           |
     *  | \ref TIVX_VPAC_VISS_H3A_IN_RAW1     | H3A Input is from RAW1           |
     *  | \ref TIVX_VPAC_VISS_H3A_IN_RAW2     | H3A Input is from RAW2           |
     *  | \ref TIVX_VPAC_VISS_H3A_IN_LSC      | H3A Input is from LSC            |
     *  | \ref TIVX_VPAC_VISS_H3A_IN_PCID     | H3A Input is from PCID           |
     *  \ref TIVX_VPAC_VISS_H3A_IN_PCID is applicable for only VPAC3L
     */
    uint32_t                    h3a_source_data;
    /*! If aew_af_mode == TIVX_VPAC_VISS_H3A_MODE_AEWB, this is the aew HW configuration used */
    log_tivx_h3a_aew_config         aew_config;
    /*! Identifier for cpu ID that the VISS node is running on.
     *  Currently used only for notifying AEWB node which cpu to send the update to based on these results
     *  when ae_awb_result from the graph is NULL. */
    // uint32_t                    cpu_id;
    /*! Identifier for camera channel ID.
     *  Currently used only for notifying AEWB node which channel to update based on these results
     *  when ae_awb_result from the graph is NULL. */
    // uint32_t                    channel_id;
    /*! Total used size of the data buffer in bytes */
    // uint32_t                    size;
    /*! Reserved dummy field to make data to be 64 byte aligned */
    // uint32_t                    resv[(TIVX_VPAC_VISS_H3A_OUT_BUFF_ALIGN-(sizeof(tivx_h3a_aew_config)+16U))/4U ];
    /*! Payload of the AEW or AF data */
    // uint8_t                     data[TIVX_VPAC_VISS_MAX_H3A_STAT_NUMBYTES];
} log_tivx_h3a_data_t;

typedef struct {
    struct timespec timestamp;
    uint32_t frame_id;    
} log_aewb_header_t;

typedef struct {
    log_aewb_header_t header;
    log_AewbHandle handle;
    log_tivx_ae_awb_params_t ae_awb_result;
    log_tivx_h3a_data_t h3a_data;
} log_aewb_message_t;

#endif //_AEWB_LOGGER_TYPES_H
