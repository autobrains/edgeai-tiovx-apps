/*
 *
 * Copyright (c) 2024 Texas Instruments Incorporated
 *
 * All rights reserved not granted herein.
 *
 * Limited License.
 *
 * Texas Instruments Incorporated grants a world-wide, royalty-free, non-exclusive
 * license under copyrights and patents it now or hereafter owns or controls to make,
 * have made, use, import, offer to sell and sell ("Utilize") this software subject to the
 * terms herein.  With respect to the foregoing patent license, such license is granted
 * solely to the extent that any such patent is necessary to Utilize the software alone.
 * The patent license shall not apply to any combinations which include this software,
 * other than combinations with devices manufactured by or for TI ("TI Devices").
 * No hardware patent is licensed hereunder.
 *
 * Redistributions must preserve existing copyright notices and reproduce this license
 * (including the above copyright notice and the disclaimer and (if applicable) source
 * code license limitations below) in the documentation and/or other materials provided
 * with the distribution
 *
 * Redistribution and use in binary form, without modification, are permitted provided
 * that the following conditions are met:
 *
 * *       No reverse engineering, decompilation, or disassembly of this software is
 * permitted with respect to any software provided in binary form.
 *
 * *       any redistribution and use are licensed by TI for use only with TI Devices.
 *
 * *       Nothing shall obligate TI to provide you with source code for the software
 * licensed and provided to you in object code.
 *
 * If software source code is provided to you, modification and redistribution of the
 * source code are permitted provided that the following conditions are met:
 *
 * *       any redistribution and use of the source code, including any resulting derivative
 * works, are licensed by TI for use only with TI Devices.
 *
 * *       any redistribution and use of any object code compiled from the source code
 * and any resulting derivative works, are licensed by TI for use only with TI Devices.
 *
 * Neither the name of Texas Instruments Incorporated nor the names of its suppliers
 *
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * DISCLAIMER.
 *
 * THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL TI AND TI'S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <apps/include/input_block.h>

#define LDC_TABLE_WIDTH     (1920)
#define LDC_TABLE_HEIGHT    (1080)
#define LDC_DS_FACTOR       (2)
#define LDC_BLOCK_WIDTH     (64)
#define LDC_BLOCK_HEIGHT    (32)
#define LDC_PIXEL_PAD       (1)

void initialize_input_block(InputBlock *input_block)
{
    input_block->input_pad = NULL;

    for(uint32_t i = 0; i < TIOVX_MODULES_MAX_NODE_OUTPUTS; i++)
    {
        input_block->output_pads[i] = NULL;
    }

    input_block->num_outputs = 0;

    tiovx_tee_init_cfg(&input_block->tee_cfg);
    input_block->tee_cfg.num_outputs = 0;
}

int32_t connect_pad_to_input_block(InputBlock *input_block,
                                   Pad* pad)
{

    input_block->output_pads[input_block->num_outputs] = pad;
    input_block->tee_cfg.num_outputs++;
    input_block->num_outputs++;
    return 0;
}

int32_t create_input_block(GraphObj *graph, InputBlock *input_block)
{
    int32_t status = 0;
    int32_t i;
    InputInfo *input_info;
    Pad *input_pad = NULL, *output_pad = NULL;

    char sensor_name[256];
    uint32_t format_pixel_container = 0;
    uint32_t format_msb = 0;
    uint32_t output_width = 0;
    uint32_t output_height = 0;
    
    if(0 == input_block->num_outputs)
    {
        TIOVX_APPS_ERROR("Number of inputs in input block cannot be 0\n");
        return -1;
    }
    if(NULL == input_block->input_info)
    {
        TIOVX_APPS_ERROR("InputInfo info of input block is null\n");
        return -1;
    }

    input_info = input_block->input_info;

    if(RTOS_CAM == input_info->source || LINUX_CAM == input_info->source)
    {
        if (0 == strcmp("imx390",input_info->sensor_name))
        {
            sprintf(sensor_name, "SENSOR_SONY_IMX390_UB953_D3");
            format_pixel_container = TIVX_RAW_IMAGE_16_BIT;
            format_msb = 11;
            output_width = 1936;
            output_height = 1096;
        }
        else if (0 == strcmp("rpi",input_info->sensor_name))
        {
            sprintf(sensor_name, "SENSOR_SONY_IMX219_RPI");
            format_pixel_container = TIVX_RAW_IMAGE_8_BIT;
            format_msb = 7;
            output_width = 1920;
            output_height = 1080;
        }
        else
        {
            TIOVX_APPS_ERROR("Invalid sensor name %s\n", input_info->sensor_name);
            return -1;
        }
    }

#if !defined(SOC_AM62A)
    /* RTOS CAM */
    if(RTOS_CAM == input_info->source)
    {
        Pad *viss_aewb_pad = NULL;

        /* RTOS Capture. */
        {
            TIOVXCaptureNodeCfg capture_cfg;
            NodeObj *capture_node;

            tiovx_capture_init_cfg(&capture_cfg);

            sprintf(capture_cfg.sensor_name, sensor_name);
            capture_cfg.ch_mask = input_info->channel_mask;

            capture_node = tiovx_modules_add_node(graph,
                                                  TIOVX_CAPTURE,
                                                  (void *)&capture_cfg);
            
            output_pad = &capture_node->srcs[0];
        }

        /* TEE */
        {
            TIOVXTeeNodeCfg tee_cfg;
            NodeObj *tee_node;

            tiovx_tee_init_cfg(&tee_cfg);

            tee_cfg.peer_pad = output_pad;
            tee_cfg.num_outputs = 2;

            tee_node = tiovx_modules_add_node(graph,
                                              TIOVX_TEE,
                                              (void *)&tee_cfg);

            tee_node->srcs[0].bufq_depth = 4; /* This must be greater than 3 */

            input_pad = &tee_node->srcs[0];
            output_pad = &tee_node->srcs[1];
        }

        /* VISS */
        {
            TIOVXVissNodeCfg viss_cfg;
            NodeObj *viss_node;

            tiovx_viss_init_cfg(&viss_cfg);

            viss_cfg.width = output_width;
            viss_cfg.height = output_height;

            sprintf(viss_cfg.sensor_name, sensor_name);
            snprintf(viss_cfg.dcc_config_file, TIVX_FILEIO_FILE_PATH_LENGTH, "%s", input_info->viss_dcc_path);
            sprintf(viss_cfg.target_string, TIVX_TARGET_VPAC_VISS1);
            viss_cfg.input_cfg.params.format[0].pixel_container = format_pixel_container;
            viss_cfg.input_cfg.params.format[0].msb = format_msb;
            viss_cfg.enable_h3a_pad = vx_true_e;

            viss_cfg.num_channels = input_info->num_channels;

            viss_node = tiovx_modules_add_node(graph,
                                               TIOVX_VISS,
                                               (void *)&viss_cfg);

            /* Link TEE to VISS */
            tiovx_modules_link_pads(output_pad, &viss_node->sinks[0]);

            output_pad = &viss_node->srcs[0];
            if (vx_true_e == viss_cfg.enable_h3a_pad)
            {
                viss_aewb_pad = &viss_node->srcs[1];
            }
        }

         /* AEWB */
        {
            TIOVXAewbNodeCfg aewb_cfg;
            NodeObj *aewb_node;

            tiovx_aewb_init_cfg(&aewb_cfg);

            sprintf(aewb_cfg.sensor_name, sensor_name);
            aewb_cfg.ch_mask = input_info->channel_mask;
            aewb_cfg.awb_mode = ALGORITHMS_ISS_AWB_AUTO;
            aewb_cfg.awb_num_skip_frames = 9;
            aewb_cfg.ae_num_skip_frames  = 9;

            aewb_node = tiovx_modules_add_node(graph,
                                               TIOVX_AEWB,
                                               (void *)&aewb_cfg);

            /* Link VISS to AEWB */
            tiovx_modules_link_pads(viss_aewb_pad, &aewb_node->sinks[0]);
        }

        /* LDC */
        if('\0' != input_info->ldc_dcc_path[0])
        {
            TIOVXLdcNodeCfg ldc_cfg;
            NodeObj *ldc_node;

            tiovx_ldc_init_cfg(&ldc_cfg);
        
            ldc_cfg.input_cfg.width = output_width;
            ldc_cfg.input_cfg.height = output_height;

            sprintf(ldc_cfg.sensor_name, sensor_name);
            snprintf(ldc_cfg.dcc_config_file, TIVX_FILEIO_FILE_PATH_LENGTH, "%s", input_info->ldc_dcc_path);
            ldc_cfg.ldc_mode = TIOVX_MODULE_LDC_OP_MODE_DCC_DATA;

            ldc_cfg.output_cfgs[0].width = input_info->width;
            ldc_cfg.output_cfgs[0].height = input_info->height;

            ldc_cfg.table_width = LDC_TABLE_WIDTH;
            ldc_cfg.table_height = LDC_TABLE_HEIGHT;
            ldc_cfg.ds_factor = LDC_DS_FACTOR;
            ldc_cfg.out_block_width = LDC_BLOCK_WIDTH;
            ldc_cfg.out_block_height = LDC_BLOCK_HEIGHT;
            ldc_cfg.pixel_pad = LDC_PIXEL_PAD;

            ldc_cfg.num_channels = input_info->num_channels;

            ldc_node = tiovx_modules_add_node(graph,
                                              TIOVX_LDC,
                                              (void *)&ldc_cfg);

            /* Link VISS0 to LDC */
            tiovx_modules_link_pads(output_pad, &ldc_node->sinks[0]);

            output_pad = &ldc_node->srcs[0];
        }
    }
#endif

    /* LINUX CAM */
    if(LINUX_CAM == input_info->source)
    {
        /* V4L2 Caprture */
        {
            v4l2CaptureCfg v4l2_capture_cfg;
            v4l2_capture_init_cfg(&v4l2_capture_cfg);
            v4l2_capture_cfg.width = output_width;
            v4l2_capture_cfg.height = output_height;
            v4l2_capture_cfg.pix_format = V4L2_PIX_FMT_SRGGB8;
            v4l2_capture_cfg.bufq_depth = 4 + 1;
            sprintf(v4l2_capture_cfg.device, input_info->device);
            input_block->v4l2_capture_handle = v4l2_capture_create_handle(&v4l2_capture_cfg);
        }
        /* VISS */
        {
            TIOVXVissNodeCfg viss_cfg;
            NodeObj *viss_node;

            tiovx_viss_init_cfg(&viss_cfg);

            viss_cfg.width = output_width;
            viss_cfg.height = output_height;

            sprintf(viss_cfg.sensor_name, sensor_name);
            snprintf(viss_cfg.dcc_config_file, TIVX_FILEIO_FILE_PATH_LENGTH, "%s", input_info->viss_dcc_path);
            sprintf(viss_cfg.target_string, TIVX_TARGET_VPAC_VISS1);
            viss_cfg.input_cfg.params.format[0].pixel_container = format_pixel_container;
            viss_cfg.input_cfg.params.format[0].msb = format_msb;

            viss_cfg.num_channels = input_info->num_channels;
            viss_node = tiovx_modules_add_node(graph,
                                               TIOVX_VISS,
                                               (void *)&viss_cfg);

            viss_node->sinks[0].bufq_depth = 4;

            input_pad = &viss_node->sinks[0];
            output_pad = &viss_node->srcs[0];
        }
    }

    /* TEE */
    {
        TIOVXTeeNodeCfg tee_cfg;
        NodeObj *tee_node = NULL;

        tiovx_tee_init_cfg(&tee_cfg);

        tee_cfg = input_block->tee_cfg;
        
        if(NULL != output_pad)
        {
            tee_cfg.peer_pad = output_pad;
            i = 0;
        }
        else
        {
            tee_cfg.peer_pad = input_block->output_pads[0];
            i = 1;
        }

        tee_node = tiovx_modules_add_node(graph,
                                          TIOVX_TEE,
                                          (void *)&tee_cfg);
        /* Link Output pads to TEE */
        for (; i < tee_cfg.num_outputs; i++)
        {
            tiovx_modules_link_pads(&tee_node->srcs[i], input_block->output_pads[i]);
        }

        if(NULL == input_pad)
        {
            input_pad = &tee_node->sinks[0];
        }
    }
   
    input_block->input_pad = input_pad;

    return status;
}