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

#include "linux_aewb_module.h"
#include "tiovx_utils.h"
#include "ti_2a_wrapper.h"

#include <sys/ioctl.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/videodev2.h>

#define AEWB_DEFAULT_DEVICE "/dev/v4l-rpi-subdev0"
#define AEWB_DEFAULT_2A_FILE "/opt/imaging/imx219/linear/dcc_2a.bin"
#define AEWB_DEFAULT_SENSOR "SENSOR_SONY_IMX219_RPI"

#define ISS_IMX390_GAIN_TBL_SIZE                (71U)
#define ISS_IMX728_GAIN_TBL_SIZE                (422U)

static const uint16_t gIMX390GainsTable[ISS_IMX390_GAIN_TBL_SIZE][2U] = {
  {1024, 0x20},
  {1121, 0x23},
  {1223, 0x26},
  {1325, 0x28},
  {1427, 0x2A},
  {1529, 0x2C},
  {1631, 0x2E},
  {1733, 0x30},
  {1835, 0x32},
  {1937, 0x33},
  {2039, 0x35},
  {2141, 0x36},
  {2243, 0x37},
  {2345, 0x39},
  {2447, 0x3A},
  {2549, 0x3B},
  {2651, 0x3C},
  {2753, 0x3D},
  {2855, 0x3E},
  {2957, 0x3F},
  {3059, 0x40},
  {3160, 0x41},
  {3262, 0x42},
  {3364, 0x43},
  {3466, 0x44},
  {3568, 0x45},
  {3670, 0x46},
  {3772, 0x46},
  {3874, 0x47},
  {3976, 0x48},
  {4078, 0x49},
  {4180, 0x49},
  {4282, 0x4A},
  {4384, 0x4B},
  {4486, 0x4B},
  {4588, 0x4C},
  {4690, 0x4D},
  {4792, 0x4D},
  {4894, 0x4E},
  {4996, 0x4F},
  {5098, 0x4F},
  {5200, 0x50},
  {5301, 0x50},
  {5403, 0x51},
  {5505, 0x51},
  {5607, 0x52},
  {5709, 0x52},
  {5811, 0x53},
  {5913, 0x53},
  {6015, 0x54},
  {6117, 0x54},
  {6219, 0x55},
  {6321, 0x55},
  {6423, 0x56},
  {6525, 0x56},
  {6627, 0x57},
  {6729, 0x57},
  {6831, 0x58},
  {6933, 0x58},
  {7035, 0x58},
  {7137, 0x59},
  {7239, 0x59},
  {7341, 0x5A},
  {7442, 0x5A},
  {7544, 0x5A},
  {7646, 0x5B},
  {7748, 0x5B},
  {7850, 0x5C},
  {7952, 0x5C},
  {8054, 0x5C},
  {8192, 0x5D}
};

static const uint32_t gIMX728GainsTable[ISS_IMX728_GAIN_TBL_SIZE][2U] =
{
  {1024, 0x0}, \
  {1036, 0x1}, \
  {1048, 0x2}, \
  {1060, 0x3}, \
  {1072, 0x4}, \
  {1085, 0x5}, \
  {1097, 0x6}, \
  {1110, 0x7}, \
  {1123, 0x8}, \
  {1136, 0x9}, \
  {1149, 0xA}, \
  {1162, 0xB}, \
  {1176, 0xC}, \
  {1189, 0xD}, \
  {1203, 0xE}, \
  {1217, 0xF}, \
  {1231, 0x10}, \
  {1245, 0x11}, \
  {1260, 0x12}, \
  {1274, 0x13}, \
  {1289, 0x14}, \
  {1304, 0x15}, \
  {1319, 0x16}, \
  {1334, 0x17}, \
  {1350, 0x18}, \
  {1366, 0x19}, \
  {1381, 0x1A}, \
  {1397, 0x1B}, \
  {1414, 0x1C}, \
  {1430, 0x1D}, \
  {1446, 0x1E}, \
  {1463, 0x1F}, \
  {1480, 0x20}, \
  {1497, 0x21}, \
  {1515, 0x22}, \
  {1532, 0x23}, \
  {1550, 0x24}, \
  {1568, 0x25}, \
  {1586, 0x26}, \
  {1604, 0x27}, \
  {1623, 0x28}, \
  {1642, 0x29}, \
  {1661, 0x2A}, \
  {1680, 0x2B}, \
  {1699, 0x2C}, \
  {1719, 0x2D}, \
  {1739, 0x2E}, \
  {1759, 0x2F}, \
  {1780, 0x30}, \
  {1800, 0x31}, \
  {1821, 0x32}, \
  {1842, 0x33}, \
  {1863, 0x34}, \
  {1885, 0x35}, \
  {1907, 0x36}, \
  {1929, 0x37}, \
  {1951, 0x38}, \
  {1974, 0x39}, \
  {1997, 0x3A}, \
  {2020, 0x3B}, \
  {2043, 0x3C}, \
  {2067, 0x3D}, \
  {2091, 0x3E}, \
  {2115, 0x3F}, \
  {2139, 0x40}, \
  {2164, 0x41}, \
  {2189, 0x42}, \
  {2215, 0x43}, \
  {2240, 0x44}, \
  {2266, 0x45}, \
  {2292, 0x46}, \
  {2319, 0x47}, \
  {2346, 0x48}, \
  {2373, 0x49}, \
  {2400, 0x4A}, \
  {2428, 0x4B}, \
  {2456, 0x4C}, \
  {2485, 0x4D}, \
  {2514, 0x4E}, \
  {2543, 0x4F}, \
  {2572, 0x50}, \
  {2602, 0x51}, \
  {2632, 0x52}, \
  {2663, 0x53}, \
  {2693, 0x54}, \
  {2725, 0x55}, \
  {2756, 0x56}, \
  {2788, 0x57}, \
  {2820, 0x58}, \
  {2853, 0x59}, \
  {2886, 0x5A}, \
  {2919, 0x5B}, \
  {2953, 0x5C}, \
  {2987, 0x5D}, \
  {3022, 0x5E}, \
  {3057, 0x5F}, \
  {3092, 0x60}, \
  {3128, 0x61}, \
  {3164, 0x62}, \
  {3201, 0x63}, \
  {3238, 0x64}, \
  {3276, 0x65}, \
  {3314, 0x66}, \
  {3352, 0x67}, \
  {3391, 0x68}, \
  {3430, 0x69}, \
  {3470, 0x6A}, \
  {3510, 0x6B}, \
  {3551, 0x6C}, \
  {3592, 0x6D}, \
  {3633, 0x6E}, \
  {3675, 0x6F}, \
  {3718, 0x70}, \
  {3761, 0x71}, \
  {3805, 0x72}, \
  {3849, 0x73}, \
  {3893, 0x74}, \
  {3938, 0x75}, \
  {3984, 0x76}, \
  {4030, 0x77}, \
  {4077, 0x78}, \
  {4124, 0x79}, \
  {4172, 0x7A}, \
  {4220, 0x7B}, \
  {4269, 0x7C}, \
  {4318, 0x7D}, \
  {4368, 0x7E}, \
  {4419, 0x7F}, \
  {4470, 0x80}, \
  {4522, 0x81}, \
  {4574, 0x82}, \
  {4627, 0x83}, \
  {4681, 0x84}, \
  {4735, 0x85}, \
  {4790, 0x86}, \
  {4845, 0x87}, \
  {4901, 0x88}, \
  {4958, 0x89}, \
  {5015, 0x8A}, \
  {5073, 0x8B}, \
  {5132, 0x8C}, \
  {5192, 0x8D}, \
  {5252, 0x8E}, \
  {5313, 0x8F}, \
  {5374, 0x90}, \
  {5436, 0x91}, \
  {5499, 0x92}, \
  {5563, 0x93}, \
  {5627, 0x94}, \
  {5692, 0x95}, \
  {5758, 0x96}, \
  {5825, 0x97}, \
  {5893, 0x98}, \
  {5961, 0x99}, \
  {6030, 0x9A}, \
  {6100, 0x9B}, \
  {6170, 0x9C}, \
  {6242, 0x9D}, \
  {6314, 0x9E}, \
  {6387, 0x9F}, \
  {6461, 0xA0}, \
  {6536, 0xA1}, \
  {6611, 0xA2}, \
  {6688, 0xA3}, \
  {6766, 0xA4}, \
  {6844, 0xA5}, \
  {6923, 0xA6}, \
  {7003, 0xA7}, \
  {7084, 0xA8}, \
  {7166, 0xA9}, \
  {7249, 0xAA}, \
  {7333, 0xAB}, \
  {7418, 0xAC}, \
  {7504, 0xAD}, \
  {7591, 0xAE}, \
  {7679, 0xAF}, \
  {7768, 0xB0}, \
  {7858, 0xB1}, \
  {7949, 0xB2}, \
  {8041, 0xB3}, \
  {8134, 0xB4}, \
  {8228, 0xB5}, \
  {8323, 0xB6}, \
  {8420, 0xB7}, \
  {8517, 0xB8}, \
  {8616, 0xB9}, \
  {8716, 0xBA}, \
  {8817, 0xBB}, \
  {8919, 0xBC}, \
  {9022, 0xBD}, \
  {9126, 0xBE}, \
  {9232, 0xBF}, \
  {9339, 0xC0}, \
  {9447, 0xC1}, \
  {9557, 0xC2}, \
  {9667, 0xC3}, \
  {9779, 0xC4}, \
  {9892, 0xC5}, \
  {10007, 0xC6}, \
  {10123, 0xC7}, \
  {10240, 0xC8}, \
  {10359, 0xC9}, \
  {10479, 0xCA}, \
  {10600, 0xCB}, \
  {10723, 0xCC}, \
  {10847, 0xCD}, \
  {10972, 0xCE}, \
  {11099, 0xCF}, \
  {11228, 0xD0}, \
  {11358, 0xD1}, \
  {11489, 0xD2}, \
  {11623, 0xD3}, \
  {11757, 0xD4}, \
  {11893, 0xD5}, \
  {12031, 0xD6}, \
  {12170, 0xD7}, \
  {12311, 0xD8}, \
  {12454, 0xD9}, \
  {12598, 0xDA}, \
  {12744, 0xDB}, \
  {12891, 0xDC}, \
  {13041, 0xDD}, \
  {13192, 0xDE}, \
  {13344, 0xDF}, \
  {13499, 0xE0}, \
  {13655, 0xE1}, \
  {13813, 0xE2}, \
  {13973, 0xE3}, \
  {14135, 0xE4}, \
  {14299, 0xE5}, \
  {14464, 0xE6}, \
  {14632, 0xE7}, \
  {14801, 0xE8}, \
  {14973, 0xE9}, \
  {15146, 0xEA}, \
  {15321, 0xEB}, \
  {15499, 0xEC}, \
  {15678, 0xED}, \
  {15860, 0xEE}, \
  {16044, 0xEF}, \
  {16229, 0xF0}, \
  {16417, 0xF1}, \
  {16607, 0xF2}, \
  {16800, 0xF3}, \
  {16994, 0xF4}, \
  {17191, 0xF5}, \
  {17390, 0xF6}, \
  {17591, 0xF7}, \
  {17795, 0xF8}, \
  {18001, 0xF9}, \
  {18210, 0xFA}, \
  {18420, 0xFB}, \
  {18634, 0xFC}, \
  {18850, 0xFD}, \
  {19068, 0xFE}, \
  {19289, 0xFF}, \
  {19512, 0x100}, \
  {19738, 0x101}, \
  {19966, 0x102}, \
  {20198, 0x103}, \
  {20431, 0x104}, \
  {20668, 0x105}, \
  {20907, 0x106}, \
  {21149, 0x107}, \
  {21394, 0x108}, \
  {21642, 0x109}, \
  {21893, 0x10A}, \
  {22146, 0x10B}, \
  {22403, 0x10C}, \
  {22662, 0x10D}, \
  {22925, 0x10E}, \
  {23190, 0x10F}, \
  {23458, 0x110}, \
  {23730, 0x111}, \
  {24005, 0x112}, \
  {24283, 0x113}, \
  {24564, 0x114}, \
  {24848, 0x115}, \
  {25136, 0x116}, \
  {25427, 0x117}, \
  {25722, 0x118}, \
  {26020, 0x119}, \
  {26321, 0x11A}, \
  {26626, 0x11B}, \
  {26934, 0x11C}, \
  {27246, 0x11D}, \
  {27561, 0x11E}, \
  {27880, 0x11F}, \
  {28203, 0x120}, \
  {28530, 0x121}, \
  {28860, 0x122}, \
  {29194, 0x123}, \
  {29532, 0x124}, \
  {29874, 0x125}, \
  {30220, 0x126}, \
  {30570, 0x127}, \
  {30924, 0x128}, \
  {31282, 0x129}, \
  {31645, 0x12A}, \
  {32011, 0x12B}, \
  {32382, 0x12C}, \
  {32757, 0x12D}, \
  {33136, 0x12E}, \
  {33520, 0x12F}, \
  {33908, 0x130}, \
  {34300, 0x131}, \
  {34698, 0x132}, \
  {35099, 0x133}, \
  {35506, 0x134}, \
  {35917, 0x135}, \
  {36333, 0x136}, \
  {36754, 0x137}, \
  {37179, 0x138}, \
  {37610, 0x139}, \
  {38045, 0x13A}, \
  {38486, 0x13B}, \
  {38931, 0x13C}, \
  {39382, 0x13D}, \
  {39838, 0x13E}, \
  {40300, 0x13F}, \
  {40766, 0x140}, \
  {41238, 0x141}, \
  {41716, 0x142}, \
  {42199, 0x143}, \
  {42687, 0x144}, \
  {43182, 0x145}, \
  {43682, 0x146}, \
  {44188, 0x147}, \
  {44699, 0x148}, \
  {45217, 0x149}, \
  {45740, 0x14A}, \
  {46270, 0x14B}, \
  {46806, 0x14C}, \
  {47348, 0x14D}, \
  {47896, 0x14E}, \
  {48451, 0x14F}, \
  {49012, 0x150}, \
  {49579, 0x151}, \
  {50153, 0x152}, \
  {50734, 0x153}, \
  {51322, 0x154}, \
  {51916, 0x155}, \
  {52517, 0x156}, \
  {53125, 0x157}, \
  {53740, 0x158}, \
  {54363, 0x159}, \
  {54992, 0x15A}, \
  {55629, 0x15B}, \
  {56273, 0x15C}, \
  {56925, 0x15D}, \
  {57584, 0x15E}, \
  {58251, 0x15F}, \
  {58925, 0x160}, \
  {59607, 0x161}, \
  {60298, 0x162}, \
  {60996, 0x163}, \
  {61702, 0x164}, \
  {62417, 0x165}, \
  {63139, 0x166}, \
  {63870, 0x167}, \
  {64610, 0x168}, \
  {65358, 0x169}, \
  {66115, 0x16A}, \
  {66881, 0x16B}, \
  {67655, 0x16C}, \
  {68438, 0x16D}, \
  {69231, 0x16E}, \
  {70033, 0x16F}, \
  {70843, 0x170}, \
  {71664, 0x171}, \
  {72494, 0x172}, \
  {73333, 0x173}, \
  {74182, 0x174}, \
  {75041, 0x175}, \
  {75910, 0x176}, \
  {76789, 0x177}, \
  {77678, 0x178}, \
  {78578, 0x179}, \
  {79488, 0x17A}, \
  {80408, 0x17B}, \
  {81339, 0x17C}, \
  {82281, 0x17D}, \
  {83234, 0x17E}, \
  {84198, 0x17F}, \
  {85173, 0x180}, \
  {86159, 0x181}, \
  {87157, 0x182}, \
  {88166, 0x183}, \
  {89187, 0x184}, \
  {90219, 0x185}, \
  {91264, 0x186}, \
  {92321, 0x187}, \
  {93390, 0x188}, \
  {94471, 0x189}, \
  {95565, 0x18A}, \
  {96672, 0x18B}, \
  {97791, 0x18C}, \
  {98924, 0x18D}, \
  {100069, 0x18E}, \
  {101228, 0x18F}, \
  {102400, 0x190}, \
  {103586, 0x191}, \
  {104785, 0x192}, \
  {105999, 0x193}, \
  {107226, 0x194}, \
  {108468, 0x195}, \
  {109724, 0x196}, \
  {110994, 0x197}, \
  {112279, 0x198}, \
  {113580, 0x199}, \
  {114895, 0x19A}, \
  {116225, 0x19B}, \
  {117571, 0x19C}, \
  {118932, 0x19D}, \
  {120310, 0x19E}, \
  {121703, 0x19F}, \
  {123112, 0x1A0}, \
  {124537, 0x1A1}, \
  {125980, 0x1A2}, \
  {127438, 0x1A3}, \
  {128914, 0x1A4}
};


void get_imx728_ae_dyn_params (IssAeDynamicParams *p_ae_dynPrms)
{
    uint8_t count = 0;

    p_ae_dynPrms->targetBrightnessRange.min = 40;
    p_ae_dynPrms->targetBrightnessRange.max = 50;
    p_ae_dynPrms->targetBrightness = 45;
    p_ae_dynPrms->threshold = 5;
    p_ae_dynPrms->enableBlc = 0;
    
    p_ae_dynPrms->exposureTimeStepSize         = 1000;  // usec
    p_ae_dynPrms->exposureTimeRange[count].min = 1000;
    p_ae_dynPrms->exposureTimeRange[count].max = 12000;
    p_ae_dynPrms->analogGainRange[count].min = 1024;
    p_ae_dynPrms->analogGainRange[count].max = 128914;
    p_ae_dynPrms->digitalGainRange[count].min = 256;
    p_ae_dynPrms->digitalGainRange[count].max = 256;
    count++;

    p_ae_dynPrms->numAeDynParams = count;
}

void get_imx219_ae_dyn_params (IssAeDynamicParams *p_ae_dynPrms)
{
    uint8_t count = 0;

    p_ae_dynPrms->targetBrightnessRange.min = 40;
    p_ae_dynPrms->targetBrightnessRange.max = 50;
    p_ae_dynPrms->targetBrightness = 45;
    p_ae_dynPrms->threshold = 1;
    p_ae_dynPrms->enableBlc = 1;
    p_ae_dynPrms->exposureTimeStepSize = 1;

    p_ae_dynPrms->exposureTimeRange[count].min = 100;
    p_ae_dynPrms->exposureTimeRange[count].max = 33333;
    p_ae_dynPrms->analogGainRange[count].min = 1024;
    p_ae_dynPrms->analogGainRange[count].max = 8192;
    p_ae_dynPrms->digitalGainRange[count].min = 256;
    p_ae_dynPrms->digitalGainRange[count].max = 256;
    count++;

    p_ae_dynPrms->numAeDynParams = count;
}

void get_imx390_ae_dyn_params (IssAeDynamicParams *p_ae_dynPrms)
{
    uint8_t count = 0;

    p_ae_dynPrms->targetBrightnessRange.min = 40;
    p_ae_dynPrms->targetBrightnessRange.max = 50;
    p_ae_dynPrms->targetBrightness = 45;
    p_ae_dynPrms->threshold = 1;
    p_ae_dynPrms->enableBlc = 1;
    p_ae_dynPrms->exposureTimeStepSize = 1;

    p_ae_dynPrms->exposureTimeRange[count].min = 100;
    p_ae_dynPrms->exposureTimeRange[count].max = 33333;
    p_ae_dynPrms->analogGainRange[count].min = 1024;
    p_ae_dynPrms->analogGainRange[count].max = 8192;
    p_ae_dynPrms->digitalGainRange[count].min = 256;
    p_ae_dynPrms->digitalGainRange[count].max = 256;
    count++;

    p_ae_dynPrms->numAeDynParams = count;
}

void get_ov2312_ae_dyn_params (IssAeDynamicParams *p_ae_dynPrms)
{
    uint8_t count = 0;

    p_ae_dynPrms->targetBrightnessRange.min = 40;
    p_ae_dynPrms->targetBrightnessRange.max = 50;
    p_ae_dynPrms->targetBrightness = 45;
    p_ae_dynPrms->threshold = 5;
    p_ae_dynPrms->enableBlc = 0;
    p_ae_dynPrms->exposureTimeStepSize = 1;

    p_ae_dynPrms->exposureTimeRange[count].min = 1000;
    p_ae_dynPrms->exposureTimeRange[count].max = 14450;
    p_ae_dynPrms->analogGainRange[count].min = 16;
    p_ae_dynPrms->analogGainRange[count].max = 16;
    p_ae_dynPrms->digitalGainRange[count].min = 1024;
    p_ae_dynPrms->digitalGainRange[count].max = 1024;
    count++;

    p_ae_dynPrms->exposureTimeRange[count].min = 14450;
    p_ae_dynPrms->exposureTimeRange[count].max = 14450;
    p_ae_dynPrms->analogGainRange[count].min = 16;
    p_ae_dynPrms->analogGainRange[count].max = 42;
    p_ae_dynPrms->digitalGainRange[count].min = 1024;
    p_ae_dynPrms->digitalGainRange[count].max = 1024;
    count++;

    p_ae_dynPrms->numAeDynParams = count;
}

void gst_tiovx_isp_map_2A_values (char *sensor_name, int exposure_time,
    int analog_gain, int *exposure_time_mapped, int *analog_gain_mapped)
{
  if (strcmp(sensor_name, "SENSOR_SONY_IMX390_UB953_D3") == 0) {
      int i;
      for (i = 0; i < ISS_IMX390_GAIN_TBL_SIZE - 1; i++) {
          if (gIMX390GainsTable[i][0] >= analog_gain) {
              break;
          }
      }
      *exposure_time_mapped = exposure_time;
      *analog_gain_mapped = gIMX390GainsTable[i][1];
  } else if (strcmp(sensor_name, "SENSOR_SONY_IMX728") == 0) {
      int i;
      for (i = 0; i < ISS_IMX728_GAIN_TBL_SIZE - 1; i++) {
          if (gIMX728GainsTable[i][0] >= analog_gain) {
              break;
          }
      }
      *exposure_time_mapped = exposure_time;
      *analog_gain_mapped = gIMX728GainsTable[i][1];
  }else if (strcmp(sensor_name, "SENSOR_SONY_IMX219_RPI") == 0) {
      double multiplier = 0;
      *exposure_time_mapped = (1080 * exposure_time / 33);
      multiplier = analog_gain / 1024.0;
      *analog_gain_mapped = 256.0 - 256.0 / multiplier;
  } else if (strcmp(sensor_name, "SENSOR_OV2312_UB953_LI") == 0) {
      *exposure_time_mapped = (60 * 1300 * exposure_time / 1000000);
      *analog_gain_mapped = analog_gain;
  } else {
      TIOVX_MODULE_ERROR("[AEWB] Unknown sensor: %s", sensor_name);
  }
}

void aewb_init_cfg(AewbCfg *cfg)
{
    sprintf(cfg->device, AEWB_DEFAULT_DEVICE);
    sprintf(cfg->dcc_2a_file, AEWB_DEFAULT_2A_FILE);
    sprintf(cfg->sensor_name, AEWB_DEFAULT_SENSOR);
    cfg->ae_mode = ALGORITHMS_ISS_AE_AUTO;
    cfg->awb_mode = ALGORITHMS_ISS_AWB_AUTO;
    cfg->ae_num_skip_frames = 0;
    cfg->awb_num_skip_frames = 0;
}

struct _AewbHandle {
    AewbCfg             cfg;
    tivx_aewb_config_t  aewb_config;
    SensorObj           sensor_obj;
    uint8_t             *dcc_2a_buf;
    uint32_t            dcc_2a_buf_size;
    TI_2A_wrapper       ti_2a_wrapper;
    sensor_config_get   sensor_in_data;
    sensor_config_set   sensor_out_data;
    int fd;
};

AewbHandle *aewb_create_handle(AewbCfg *cfg)
{
    AewbHandle *handle = NULL;
    vx_status status = VX_FAILURE;
    FILE *dcc_2a_fp = NULL;

    handle = malloc(sizeof(AewbHandle));
    handle->fd = -1;
    memcpy(&handle->cfg, cfg, sizeof(AewbHandle));

    handle->fd = open(cfg->device, O_RDWR | O_NONBLOCK, 0);
    if (-1 == handle->fd) {
        TIOVX_MODULE_ERROR("[AEWB] Cannot open '%s': %d, %s\n",
                            cfg->device, errno, strerror(errno));
        goto free_handle;
    }

    status = tiovx_init_sensor_obj(&handle->sensor_obj, cfg->sensor_name);
    if (VX_SUCCESS != status) {
        TIOVX_MODULE_ERROR("[AEWB] Sensor Init Failed\n");
        goto free_fd;
    }

    handle->aewb_config.sensor_img_phase = 3;
    handle->aewb_config.sensor_dcc_id = handle->sensor_obj.sensorParams.dccId;
    handle->aewb_config.sensor_img_format = 0;
    handle->aewb_config.awb_mode = cfg->awb_mode;
    handle->aewb_config.ae_mode = cfg->ae_mode;
    handle->aewb_config.awb_num_skip_frames = cfg->awb_num_skip_frames;
    handle->aewb_config.ae_num_skip_frames = cfg->ae_num_skip_frames;
    handle->aewb_config.channel_id = 0;

    dcc_2a_fp = fopen (cfg->dcc_2a_file, "rb");

    if (NULL == dcc_2a_fp) {
        TIOVX_MODULE_ERROR("[AEWB] Unable to open dcc 2a file\n");
        goto free_fd;
    }

    fseek(dcc_2a_fp, 0, SEEK_END);
    handle->dcc_2a_buf_size = ftell(dcc_2a_fp);
    fseek (dcc_2a_fp, 0, SEEK_SET);

    if (0 == handle->dcc_2a_buf_size) {
        TIOVX_MODULE_ERROR("[AEWB] dcc 2a file has 0 size\n");
        goto free_2a_file;
    }

    handle->dcc_2a_buf = (uint8_t *) tivxMemAlloc (handle->dcc_2a_buf_size,
                                                   TIVX_MEM_EXTERNAL);
    fread (handle->dcc_2a_buf, 1, handle->dcc_2a_buf_size, dcc_2a_fp);
    fclose (dcc_2a_fp);

    status = TI_2A_wrapper_create(&handle->ti_2a_wrapper, &handle->aewb_config,
                handle->dcc_2a_buf, handle->dcc_2a_buf_size);
    if (status) {
        TIOVX_MODULE_ERROR("[AEWB] Unable to create TI 2A wrapper: %d", status);
        goto free_2a_file;
    }

    if (strcmp(cfg->sensor_name, "SENSOR_SONY_IMX390_UB953_D3") == 0) {
      get_imx390_ae_dyn_params (&handle->sensor_in_data.ae_dynPrms);
    } else if (strcmp(cfg->sensor_name, "SENSOR_OV2312_UB953_LI") == 0) {
      get_ov2312_ae_dyn_params (&handle->sensor_in_data.ae_dynPrms);
    } else if (strcmp(cfg->sensor_name, "SENSOR_SONY_IMX728") == 0) {
      get_imx728_ae_dyn_params (&handle->sensor_in_data.ae_dynPrms);
    } else {
      get_imx219_ae_dyn_params (&handle->sensor_in_data.ae_dynPrms);
    }

    return handle;

free_2a_file:
    fclose(dcc_2a_fp);
free_fd:
    close(handle->fd);
free_handle:
    free(handle);
    return NULL;
}

int aewb_write_to_sensor(AewbHandle *handle)
{
    int ret = -1;
    int analog_gain = 0;
    int coarse_integration_time = 0;
    struct v4l2_control control;

    gst_tiovx_isp_map_2A_values (handle->cfg.sensor_name,
            handle->sensor_out_data.aePrms.exposureTime[0],
            handle->sensor_out_data.aePrms.analogGain[0],
            &coarse_integration_time, &analog_gain);

    control.id = V4L2_CID_EXPOSURE;
    control.value = coarse_integration_time;
    ret = ioctl (handle->fd, VIDIOC_S_CTRL, &control);
    if (ret < 0) {
        TIOVX_MODULE_ERROR("[AEWB] Unable to call exposure ioctl: %d", ret);
        return ret;
    }

    control.id = V4L2_CID_ANALOGUE_GAIN;
    control.value = analog_gain;
    ret = ioctl (handle->fd, VIDIOC_S_CTRL, &control);
    if (ret < 0) {
        TIOVX_MODULE_ERROR("[AEWB] Unable to call analog gain ioctl: %d", ret);
    }
    
    //TIOVX_MODULE_ERROR("exposure time: %d, analog gain: %d\n", handle->sensor_out_data.aePrms.exposureTime[0], handle->sensor_out_data.aePrms.analogGain[0]);

    return ret;
}

int aewb_process(AewbHandle *handle, Buf *h3a_buf, Buf *aewb_buf)
{
    vx_status status = VX_FAILURE;
    vx_map_id h3a_buf_map_id;
    vx_map_id aewb_buf_map_id;
    tivx_h3a_data_t *h3a_ptr = NULL;
    tivx_ae_awb_params_t *aewb_ptr = NULL;

    vxMapUserDataObject ((vx_user_data_object)h3a_buf->handle, 0,
            sizeof (tivx_h3a_data_t), &h3a_buf_map_id, (void **) &h3a_ptr,
            VX_READ_ONLY, VX_MEMORY_TYPE_HOST, 0);
    vxMapUserDataObject ((vx_user_data_object)aewb_buf->handle, 0,
            sizeof (tivx_ae_awb_params_t), &aewb_buf_map_id,
            (void **)&aewb_ptr, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST, 0);

    status = TI_2A_wrapper_process(&handle->ti_2a_wrapper, &handle->aewb_config,
            h3a_ptr, &handle->sensor_in_data, aewb_ptr,
            &handle->sensor_out_data);
    if (status) {
        TIOVX_MODULE_ERROR("[AEWB] Process call failed: %d", status);
    }

    vxUnmapUserDataObject ((vx_user_data_object)h3a_buf->handle, h3a_buf_map_id);
    vxUnmapUserDataObject ((vx_user_data_object)aewb_buf->handle, aewb_buf_map_id);

    status = aewb_write_to_sensor(handle);

    return status;
}

int aewb_delete_handle(AewbHandle *handle)
{
    int status = 0;

    TI_2A_wrapper_delete(&handle->ti_2a_wrapper);
    tivxMemFree((void *)handle->dcc_2a_buf, handle->dcc_2a_buf_size,
            TIVX_MEM_EXTERNAL);
    close(handle->fd);
    free(handle);

    return status;
}
