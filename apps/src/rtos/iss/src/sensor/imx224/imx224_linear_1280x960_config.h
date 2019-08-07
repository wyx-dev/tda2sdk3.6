/******************************************************************************
Copyright (c) [2012 - 2017] Texas Instruments Incorporated

All rights reserved not granted herein.

Limited License.

 Texas Instruments Incorporated grants a world-wide, royalty-free, non-exclusive
 license under copyrights and patents it now or hereafter owns or controls to
 make,  have made, use, import, offer to sell and sell ("Utilize") this software
 subject to the terms herein.  With respect to the foregoing patent license,
 such license is granted  solely to the extent that any such patent is necessary
 to Utilize the software alone.  The patent license shall not apply to any
 combinations which include this software, other than combinations with devices
 manufactured by or for TI ("TI Devices").  No hardware patent is licensed
 hereunder.

 Redistributions must preserve existing copyright notices and reproduce this
 license (including the above copyright notice and the disclaimer and
 (if applicable) source code license limitations below) in the documentation
 and/or other materials provided with the distribution

 Redistribution and use in binary form, without modification, are permitted
 provided that the following conditions are met:

 * No reverse engineering, decompilation, or disassembly of this software
   is permitted with respect to any software provided in binary form.

 * Any redistribution and use are licensed by TI for use only with TI Devices.

 * Nothing shall obligate TI to provide you with source code for the software
   licensed and provided to you in object code.

 If software source code is provided to you, modification and redistribution of
 the source code are permitted provided that the following conditions are met:

 * Any redistribution and use of the source code, including any resulting
   derivative works, are licensed by TI for use only with TI Devices.

 * Any redistribution and use of any object code compiled from the source code
   and any resulting derivative works, are licensed by TI for use only with TI
   Devices.

 Neither the name of Texas Instruments Incorporated nor the names of its
 suppliers may be used to endorse or promote products derived from this software
 without specific prior written permission.

 DISCLAIMER.

 THIS SOFTWARE IS PROVIDED BY TI AND TI�S LICENSORS "AS IS" AND ANY EXPRESS OR
 IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL TI AND TI�S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/

/**
 *  \file imx224_linear_1280x960_config.h
 *
 *  \brief IMX224 sensor Register file.
 *
 */

#ifndef IMX224_LINEAR_1280X960_CONFIG_H_
#define IMX224_LINEAR_1280X960_CONFIG_H_

/* imx224 recommended setting */
#define IMX224_LINEAR_1280X960_CONFIG                                          \
{                                                                              \
/*                                                                             \
 * imx224_xx_m for 1280*960@30fps,mipi 4lane,raw12                             \
 * */                                                                          \
        /*                                                                     \
        ID:02                                                                  \
        Mode:1280*960@30fps                                                    \
        */                                                                     \
        {0x3000, 0x01, 0x0},                                                   \
        {0x3001, 0x00, 0x0},                                                   \
        {0x3002, 0x00, 0x0}, /* 01 */                                          \
        {0x3003, 0x00, 0x0},                                                   \
        {0x3004, 0x10, 0x0},                                                   \
        {0x3005, 0x01, 0x0},                                                   \
        {0x3006, 0x00, 0x0},                                                   \
        {0x3007, 0x03, 0x0}, /* 1280x720, H & V Reversed */                    \
        {0x3008, 0x00, 0x0},                                                   \
        {0x3009, 0x12, 0x0},/* 0x01, */                                        \
        {0x300A, 0xF0, 0x0},                                                   \
        {0x300B, 0x00, 0x0},                                                   \
        {0x300C, 0x00, 0x0},                                                   \
        {0x300D, 0x00, 0x0},                                                   \
        {0x300E, 0x01, 0x0},                                                   \
        {0x300F, 0x00, 0x0}, /* 01 */                                          \
        {0x3010, 0x01, 0x0},                                                   \
        {0x3011, 0x00, 0x0},                                                   \
        {0x3012, 0x2C, 0x0}, /* F0 */                                          \
        {0x3013, 0x01, 0x0}, /* 00 */                                          \
        {0x3014, 0x00, 0x0}, /* 00(gain low 8bits) */                          \
        {0x3015, 0x00, 0x0}, /* 00(gain high 2bits) */                         \
        {0x3016, 0x09, 0x0}, /* 08 */                                          \
        {0x3017, 0x00, 0x0},                                                   \
        {0x3018, 0x4C, 0x0},                                                   \
        {0x3019, 0x04, 0x0},                                                   \
        {0x301A, 0x00, 0x0},                                                   \
        {0x301B, 0x94, 0x0},                                                   \
        {0x301C, 0x11, 0x0},                                                   \
        {0x301D, 0xC2, 0x0}, /* B2 */                                          \
        {0x301E, 0x01, 0x0},                                                   \
        {0x301F, 0x00, 0x0},                                                   \
        {0x3020, 0x00, 0x0}, /* 00 */                                          \
        {0x3021, 0x01, 0x0}, /* 00 */                                          \
        {0x3022, 0x00, 0x0}, /* 00 */                                          \
        {0x3023, 0x00, 0x0},                                                   \
        {0x3024, 0x00, 0x0},                                                   \
        {0x3025, 0x00, 0x0},                                                   \
        {0x3026, 0x00, 0x0},                                                   \
        {0x3027, 0x00, 0x0},                                                   \
        {0x3028, 0x00, 0x0},                                                   \
        {0x3029, 0x00, 0x0},                                                   \
        {0x302A, 0x00, 0x0},                                                   \
        {0x302B, 0x00, 0x0},                                                   \
        {0x302C, 0x00, 0x0},                                                   \
        {0x302D, 0x00, 0x0},                                                   \
        {0x302E, 0x00, 0x0},                                                   \
        {0x302F, 0x00, 0x0},                                                   \
        {0x3030, 0x00, 0x0},                                                   \
        {0x3031, 0x00, 0x0},                                                   \
        {0x3032, 0x00, 0x0},                                                   \
        {0x3033, 0x00, 0x0},                                                   \
        {0x3034, 0x00, 0x0},                                                   \
        {0x3035, 0x00, 0x0},                                                   \
        {0x3036, 0x10, 0x0},                                                   \
        {0x3037, 0x00, 0x0},                                                   \
        {0x3038, 0x00, 0x0},                                                   \
        {0x3039, 0x00, 0x0},                                                   \
        {0x303A, 0xD1, 0x0},                                                   \
        {0x303B, 0x03, 0x0},                                                   \
        {0x303C, 0x00, 0x0},                                                   \
        {0x303D, 0x00, 0x0},                                                   \
        {0x303E, 0x1C, 0x0},                                                   \
        {0x303F, 0x05, 0x0},                                                   \
        {0x3040, 0x01, 0x0},                                                   \
        {0x3041, 0x00, 0x0},                                                   \
        {0x3042, 0x01, 0x0},                                                   \
        {0x3043, 0x01, 0x0},                                                   \
        {0x3044, 0x01, 0x0},                                                   \
        {0x3045, 0x01, 0x0},                                                   \
        {0x3046, 0x00, 0x0},                                                   \
        {0x3047, 0x08, 0x0},                                                   \
        {0x3048, 0x00, 0x0},                                                   \
        {0x3049, 0x0A, 0x0}, /* 00 */                                          \
        {0x304A, 0x00, 0x0},                                                   \
        {0x304B, 0x00, 0x0},                                                   \
        {0x304C, 0x00, 0x0},                                                   \
        {0x304D, 0x00, 0x0},                                                   \
        {0x304E, 0x00, 0x0},                                                   \
        {0x304F, 0x00, 0x0},                                                   \
        {0x3050, 0x26, 0x0},                                                   \
        {0x3051, 0x00, 0x0},                                                   \
        {0x3052, 0x01, 0x0},                                                   \
        {0x3053, 0x00, 0x0},                                                   \
        {0x3054, 0x66, 0x0},/* 0x67, */                                        \
        {0x3055, 0x00, 0x0},                                                   \
        {0x3056, 0x13, 0x0},                                                   \
        {0x3057, 0x15, 0x0},                                                   \
        {0x3058, 0x05, 0x0},                                                   \
        {0x3059, 0x00, 0x0},                                                   \
        {0x305A, 0x00, 0x0},                                                   \
        {0x305B, 0x00, 0x0},                                                   \
        {0x305C, 0x2C, 0x0},                                                   \
        {0x305D, 0x00, 0x0}, /* 10 */                                          \
        {0x305E, 0x2C, 0x0},                                                   \
        {0x305F, 0x00, 0x0}, /* 10 */                                          \
        {0x3060, 0x00, 0x0},                                                   \
        {0x3061, 0x4A, 0x0},                                                   \
        {0x3062, 0x41, 0x0},                                                   \
        {0x3063, 0xB0, 0x0},                                                   \
        {0x3064, 0x00, 0x0},                                                   \
        {0x3065, 0x02, 0x0},                                                   \
        {0x3066, 0x00, 0x0},                                                   \
        {0x3067, 0x01, 0x0},                                                   \
        {0x3068, 0x00, 0x0},                                                   \
        {0x3069, 0x0C, 0x0},                                                   \
        {0x306A, 0x19, 0x0},                                                   \
        {0x306B, 0x00, 0x0},                                                   \
        {0x306C, 0x00, 0x0},                                                   \
        {0x306D, 0x64, 0x0},                                                   \
        {0x306E, 0x00, 0x0},                                                   \
        {0x306F, 0x00, 0x0},                                                   \
        {0x3070, 0x02, 0x0}, /* 01 */                                          \
        {0x3071, 0x01, 0x0}, /* 00 */                                          \
        {0x3072, 0x00, 0x0},                                                   \
        {0x3073, 0x00, 0x0},                                                   \
        {0x3074, 0x00, 0x0},                                                   \
        {0x3075, 0x00, 0x0},                                                   \
        {0x3076, 0x00, 0x0},                                                   \
        {0x3077, 0x00, 0x0},                                                   \
        {0x3078, 0x00, 0x0},                                                   \
        {0x3079, 0x00, 0x0},                                                   \
        {0x307A, 0x00, 0x0},                                                   \
        {0x307B, 0x00, 0x0},                                                   \
        {0x307C, 0x00, 0x0},                                                   \
        {0x307D, 0x00, 0x0},                                                   \
        {0x307E, 0x00, 0x0},                                                   \
        {0x307F, 0x00, 0x0},                                                   \
        {0x3080, 0x00, 0x0},                                                   \
        {0x3081, 0x00, 0x0},                                                   \
        {0x3082, 0x00, 0x0},                                                   \
        {0x3083, 0x00, 0x0},                                                   \
        {0x3084, 0x00, 0x0},                                                   \
        {0x3085, 0x00, 0x0},                                                   \
        {0x3086, 0x00, 0x0},                                                   \
        {0x3087, 0x00, 0x0},                                                   \
        {0x3088, 0x00, 0x0},                                                   \
        {0x3089, 0xFF, 0x0},                                                   \
        {0x308A, 0x00, 0x0},                                                   \
        {0x308B, 0x00, 0x0},                                                   \
        {0x308C, 0x02, 0x0},                                                   \
        {0x308D, 0x00, 0x0},                                                   \
        {0x308E, 0x00, 0x0},                                                   \
        {0x308F, 0x00, 0x0},                                                   \
        {0x3090, 0x00, 0x0},                                                   \
        {0x3091, 0x00, 0x0},                                                   \
        {0x3092, 0x00, 0x0},                                                   \
        {0x3093, 0x01, 0x0},                                                   \
        {0x3094, 0x01, 0x0},                                                   \
        {0x3095, 0x01, 0x0},                                                   \
        {0x3096, 0x00, 0x0},                                                   \
        {0x3097, 0x00, 0x0},                                                   \
        {0x3098, 0x00, 0x0},                                                   \
        {0x3099, 0x00, 0x0},                                                   \
        {0x309A, 0x00, 0x0},                                                   \
        {0x309B, 0x00, 0x0},                                                   \
        {0x309C, 0x00, 0x0},                                                   \
        {0x309D, 0x7D, 0x0},                                                   \
        {0x309E, 0x22, 0x0}, /* 20 */                                          \
        {0x309F, 0x00, 0x0},                                                   \
        {0x30A0, 0x00, 0x0},                                                   \
        {0x30A1, 0x04, 0x0},                                                   \
        {0x30A2, 0x5A, 0x0},                                                   \
        {0x30A3, 0x5A, 0x0},                                                   \
        {0x30A4, 0x2A, 0x0},                                                   \
        {0x30A5, 0xFB, 0x0}, /* 2A */                                          \
        {0x30A6, 0x02, 0x0}, /* 00 */                                          \
        {0x30A7, 0x22, 0x0},                                                   \
        {0x30A8, 0x00, 0x0},                                                   \
        {0x30A9, 0x00, 0x0},                                                   \
        {0x30AA, 0x00, 0x0},                                                   \
        {0x30AB, 0x10, 0x0},                                                   \
        {0x30AC, 0x00, 0x0},                                                   \
        {0x30AD, 0x10, 0x0},                                                   \
        {0x30AE, 0x00, 0x0},                                                   \
        {0x30AF, 0x10, 0x0},                                                   \
        {0x30B0, 0x00, 0x0},                                                   \
        {0x30B1, 0x10, 0x0},                                                   \
        {0x30B2, 0x00, 0x0},                                                   \
        {0x30B3, 0xFF, 0x0}, /* 7F */                                          \
        {0x30B4, 0x01, 0x0}, /* 02 */                                          \
        {0x30B5, 0x42, 0x0}, /* 41 */                                          \
        {0x30B6, 0x00, 0x0},                                                   \
        {0x30B7, 0x60, 0x0},                                                   \
        {0x30B8, 0x10, 0x0}, /* 12 */                                          \
        {0x30B9, 0x12, 0x0},                                                   \
        {0x30BA, 0x0F, 0x0},                                                   \
        {0x30BB, 0x00, 0x0},                                                   \
        {0x30BC, 0x00, 0x0},                                                   \
        {0x30BD, 0x00, 0x0},                                                   \
        {0x30BE, 0x00, 0x0},                                                   \
        {0x30BF, 0x10, 0x0},                                                   \
        {0x30C0, 0x00, 0x0},                                                   \
        {0x30C1, 0x00, 0x0},                                                   \
        {0x30C2, 0x01, 0x0}, /* 03 */                                          \
        {0x30C3, 0x04, 0x0},                                                   \
        {0x30C4, 0x12, 0x0},                                                   \
        {0x30C5, 0x10, 0x0},                                                   \
        {0x30C6, 0x00, 0x0},                                                   \
        {0x30C7, 0x00, 0x0},                                                   \
        {0x30C8, 0x00, 0x0},                                                   \
        {0x30C9, 0xFF, 0x0},                                                   \
        {0x30CA, 0x03, 0x0},                                                   \
        {0x30CB, 0xD8, 0x0},                                                   \
        {0x30CC, 0x7D, 0x0},                                                   \
        {0x30CD, 0x00, 0x0},                                                   \
        {0x30CE, 0xFB, 0x0},                                                   \
        {0x30CF, 0x0B, 0x0},                                                   \
        {0x30D0, 0x01, 0x0},                                                   \
        {0x30D1, 0xF0, 0x0},                                                   \
        {0x30D2, 0x00, 0x0},                                                   \
        {0x30D3, 0x0B, 0x0},                                                   \
        {0x30D4, 0xF0, 0x0},                                                   \
        {0x30D5, 0x00, 0x0},                                                   \
        {0x30D6, 0x0B, 0x0},                                                   \
        {0x30D7, 0xF0, 0x0},                                                   \
        {0x30D8, 0x00, 0x0},                                                   \
        {0x30D9, 0x13, 0x0},                                                   \
        {0x30DA, 0x70, 0x0},                                                   \
        {0x30DB, 0x01, 0x0},                                                   \
        {0x30DC, 0x13, 0x0},                                                   \
        {0x30DD, 0x70, 0x0},                                                   \
        {0x30DE, 0x01, 0x0},                                                   \
        {0x30DF, 0x00, 0x0},                                                   \
        {0x30E0, 0x00, 0x0},                                                   \
        {0x30E1, 0xFF, 0x0},                                                   \
        {0x30E2, 0xFF, 0x0},                                                   \
        {0x30E3, 0xFF, 0x0},                                                   \
        {0x30E4, 0xFF, 0x0},                                                   \
        {0x30E5, 0xFF, 0x0},                                                   \
        {0x30E6, 0xFF, 0x0},                                                   \
        {0x30E7, 0x00, 0x0},                                                   \
        {0x30E8, 0x00, 0x0},                                                   \
        {0x30E9, 0x00, 0x0},                                                   \
        {0x30EA, 0x00, 0x0},                                                   \
        {0x30EB, 0x01, 0x0},                                                   \
        {0x30EC, 0x02, 0x0},                                                   \
        {0x30ED, 0x43, 0x0},                                                   \
        {0x30EE, 0x44, 0x0},                                                   \
        {0x30EF, 0x00, 0x0},                                                   \
        {0x30F0, 0xF0, 0x0},                                                   \
        {0x30F1, 0x00, 0x0},                                                   \
        {0x30F2, 0x00, 0x0},                                                   \
        {0x30F3, 0x00, 0x0},                                                   \
        {0x30F4, 0xF0, 0x0},                                                   \
        {0x30F5, 0x00, 0x0},                                                   \
        {0x30F6, 0x00, 0x0},                                                   \
        {0x30F7, 0x00, 0x0},                                                   \
        {0x30F8, 0xF0, 0x0},                                                   \
        {0x30F9, 0x00, 0x0},                                                   \
        {0x30FA, 0x00, 0x0},                                                   \
        {0x30FB, 0x00, 0x0},                                                   \
        {0x30FC, 0x00, 0x0},                                                   \
        {0x30FD, 0x00, 0x0},                                                   \
        {0x30FE, 0x00, 0x0},                                                   \
                                                                               \
                                                                               \
        /*                                                                     \
        ID:03                                                                  \
        Mode:1280*960@30fps                                                    \
        */                                                                     \
        {0x3100, 0x00, 0x0},                                                   \
        {0x3101, 0x00, 0x0},                                                   \
        {0x3102, 0x80, 0x0},                                                   \
        {0x3103, 0x00, 0x0},                                                   \
        {0x3104, 0x00, 0x0},                                                   \
        {0x3105, 0x00, 0x0},                                                   \
        {0x3106, 0x00, 0x0},                                                   \
        {0x3107, 0x00, 0x0},                                                   \
        {0x3108, 0x00, 0x0},                                                   \
        {0x3109, 0x00, 0x0},                                                   \
        {0x310A, 0x00, 0x0},                                                   \
        {0x310B, 0x04, 0x0},                                                   \
        {0x310C, 0x00, 0x0},                                                   \
        {0x310D, 0x00, 0x0},                                                   \
        {0x310E, 0x01, 0x0},                                                   \
        {0x310F, 0x0F, 0x0}, /* 16 */                                          \
        {0x3110, 0x0E, 0x0}, /* 16 */                                          \
        {0x3111, 0xE7, 0x0}, /* 00 */                                          \
        {0x3112, 0x9C, 0x0}, /* 00 */                                          \
        {0x3113, 0x83, 0x0}, /* 00 */                                          \
        {0x3114, 0x10, 0x0}, /* 00 */                                          \
        {0x3115, 0x42, 0x0}, /* 00 */                                          \
        {0x3116, 0xBF, 0x0},                                                   \
        {0x3117, 0x03, 0x0},                                                   \
        {0x3118, 0x44, 0x0},                                                   \
        {0x3119, 0x44, 0x0},                                                   \
        {0x311A, 0x00, 0x0},                                                   \
        {0x311B, 0x06, 0x0},                                                   \
        {0x311C, 0x88, 0x0},                                                   \
        {0x311D, 0x0E, 0x0},                                                   \
        {0x311E, 0x02, 0x0},                                                   \
        {0x311F, 0x04, 0x0},                                                   \
        {0x3120, 0x12, 0x0},                                                   \
        {0x3121, 0x00, 0x0},                                                   \
        {0x3122, 0x00, 0x0},                                                   \
        {0x3123, 0x00, 0x0},                                                   \
        {0x3124, 0x00, 0x0},                                                   \
        {0x3125, 0x44, 0x0},                                                   \
        {0x3126, 0x44, 0x0},                                                   \
        {0x3127, 0x00, 0x0},                                                   \
        {0x3128, 0x1E, 0x0}, /* 00 */                                          \
        {0x3129, 0x1A, 0x0},                                                   \
        {0x312A, 0x00, 0x0},                                                   \
        {0x312B, 0x08, 0x0},                                                   \
        {0x312C, 0x00, 0x0},                                                   \
        {0x312D, 0x00, 0x0},                                                   \
        {0x312E, 0x00, 0x0},                                                   \
        {0x312F, 0x00, 0x0},                                                   \
        {0x3130, 0x00, 0x0},                                                   \
        {0x3131, 0x00, 0x0},                                                   \
        {0x3132, 0x30, 0x0},                                                   \
        {0x3133, 0x00, 0x0},                                                   \
        {0x3134, 0x00, 0x0},                                                   \
        {0x3135, 0x00, 0x0},                                                   \
        {0x3136, 0x00, 0x0},                                                   \
        {0x3137, 0x00, 0x0},                                                   \
        {0x3138, 0x08, 0x0},                                                   \
        {0x3139, 0x00, 0x0},                                                   \
        {0x313A, 0x60, 0x0},                                                   \
        {0x313B, 0x03, 0x0},                                                   \
        {0x313C, 0x08, 0x0},                                                   \
        {0x313D, 0x00, 0x0},                                                   \
        {0x313E, 0x00, 0x0},                                                   \
        {0x313F, 0x00, 0x0},                                                   \
        {0x3140, 0x02, 0x0},                                                   \
        {0x3141, 0x87, 0x0},                                                   \
        {0x3142, 0x00, 0x0},                                                   \
        {0x3143, 0x00, 0x0},                                                   \
        {0x3144, 0x07, 0x0},                                                   \
        {0x3145, 0xFF, 0x0},                                                   \
        {0x3146, 0x1F, 0x0},                                                   \
        {0x3147, 0x00, 0x0},                                                   \
        {0x3148, 0xFF, 0x0},                                                   \
        {0x3149, 0x1F, 0x0},                                                   \
        {0x314A, 0x00, 0x0},                                                   \
        {0x314B, 0x00, 0x0},                                                   \
        {0x314C, 0x00, 0x0},                                                   \
        {0x314D, 0x02, 0x0},                                                   \
        {0x314E, 0x87, 0x0},                                                   \
        {0x314F, 0x00, 0x0},                                                   \
        {0x3150, 0x00, 0x0},                                                   \
        {0x3151, 0x00, 0x0},                                                   \
        {0x3152, 0x07, 0x0},                                                   \
        {0x3153, 0x40, 0x0},                                                   \
        {0x3154, 0x00, 0x0},                                                   \
        {0x3155, 0x00, 0x0},                                                   \
        {0x3156, 0x00, 0x0},                                                   \
        {0x3157, 0x00, 0x0},                                                   \
        {0x3158, 0x00, 0x0},                                                   \
        {0x3159, 0x00, 0x0},                                                   \
        {0x315A, 0x00, 0x0},                                                   \
        {0x315B, 0x00, 0x0},                                                   \
        {0x315C, 0x00, 0x0},                                                   \
        {0x315D, 0x00, 0x0},                                                   \
        {0x315E, 0x12, 0x0},                                                   \
        {0x315F, 0x80, 0x0},                                                   \
        {0x3160, 0x00, 0x0},                                                   \
        {0x3161, 0x70, 0x0},                                                   \
        {0x3162, 0x07, 0x0},                                                   \
        {0x3163, 0x00, 0x0},                                                   \
        {0x3164, 0x12, 0x0},                                                   \
        {0x3165, 0x80, 0x0},                                                   \
        {0x3166, 0x00, 0x0},                                                   \
        {0x3167, 0x02, 0x0},                                                   \
        {0x3168, 0x00, 0x0},                                                   \
        {0x3169, 0x00, 0x0},                                                   \
        {0x316A, 0x10, 0x0},                                                   \
        {0x316B, 0x04, 0x0},                                                   \
        {0x316C, 0x00, 0x0},                                                   \
        {0x316D, 0x00, 0x0},                                                   \
        {0x316E, 0x00, 0x0},                                                   \
        {0x316F, 0xFF, 0x0},                                                   \
        {0x3170, 0x01, 0x0},                                                   \
        {0x3171, 0x02, 0x0},                                                   \
        {0x3172, 0x00, 0x0},                                                   \
        {0x3173, 0x00, 0x0},                                                   \
        {0x3174, 0x05, 0x0},                                                   \
        {0x3175, 0x00, 0x0},                                                   \
        {0x3176, 0x00, 0x0},                                                   \
        {0x3177, 0x00, 0x0},                                                   \
        {0x3178, 0x05, 0x0},                                                   \
        {0x3179, 0x00, 0x0},                                                   \
        {0x317A, 0xFB, 0x0},                                                   \
        {0x317B, 0x77, 0x0},                                                   \
        {0x317C, 0x01, 0x0},                                                   \
        {0x317D, 0x17, 0x0},                                                   \
        {0x317E, 0x00, 0x0},                                                   \
        {0x317F, 0x00, 0x0},                                                   \
        {0x3180, 0x00, 0x0},                                                   \
        {0x3181, 0x00, 0x0},                                                   \
        {0x3182, 0x00, 0x0},                                                   \
        {0x3183, 0x00, 0x0},                                                   \
        {0x3184, 0x04, 0x0},                                                   \
        {0x3185, 0x00, 0x0},                                                   \
        {0x3186, 0x13, 0x0},                                                   \
        {0x3187, 0x00, 0x0},                                                   \
        {0x3188, 0x2C, 0x0},                                                   \
        {0x3189, 0x00, 0x0},                                                   \
        {0x318A, 0xFC, 0x0},                                                   \
        {0x318B, 0x03, 0x0},                                                   \
        {0x318C, 0xFC, 0x0},                                                   \
        {0x318D, 0x03, 0x0},                                                   \
        {0x318E, 0xFC, 0x0},                                                   \
        {0x318F, 0x03, 0x0},                                                   \
        {0x3190, 0x0A, 0x0},                                                   \
        {0x3191, 0x00, 0x0},                                                   \
        {0x3192, 0x00, 0x0},                                                   \
        {0x3193, 0x04, 0x0},                                                   \
        {0x3194, 0x08, 0x0},                                                   \
        {0x3195, 0x10, 0x0},                                                   \
        {0x3196, 0x11, 0x0},                                                   \
        {0x3197, 0x00, 0x0},                                                   \
        {0x3198, 0x7C, 0x0},                                                   \
        {0x3199, 0x00, 0x0},                                                   \
        {0x319A, 0xD0, 0x0},                                                   \
        {0x319B, 0x07, 0x0},                                                   \
        {0x319C, 0x0E, 0x0},                                                   \
        {0x319D, 0x04, 0x0},                                                   \
        {0x319E, 0x0D, 0x0},                                                   \
        {0x319F, 0x04, 0x0},                                                   \
        {0x31A0, 0x00, 0x0},                                                   \
        {0x31A1, 0x00, 0x0},                                                   \
        {0x31A2, 0x00, 0x0},                                                   \
        {0x31A3, 0x04, 0x0},                                                   \
        {0x31A4, 0x00, 0x0},                                                   \
        {0x31A5, 0x4A, 0x0},                                                   \
        {0x31A6, 0x01, 0x0},                                                   \
        {0x31A7, 0x4D, 0x0},                                                   \
        {0x31A8, 0x01, 0x0},                                                   \
        {0x31A9, 0x00, 0x0},                                                   \
        {0x31AA, 0x03, 0x0},                                                   \
        {0x31AB, 0x03, 0x0},                                                   \
        {0x31AC, 0x00, 0x0},                                                   \
        {0x31AD, 0x00, 0x0},                                                   \
        {0x31AE, 0x00, 0x0},                                                   \
        {0x31AF, 0x02, 0x0},                                                   \
        {0x31B0, 0x01, 0x0},                                                   \
        {0x31B1, 0x00, 0x0},                                                   \
        {0x31B2, 0x00, 0x0},                                                   \
        {0x31B3, 0x00, 0x0},                                                   \
        {0x31B4, 0x00, 0x0},                                                   \
        {0x31B5, 0x00, 0x0},                                                   \
        {0x31B6, 0x00, 0x0},                                                   \
        {0x31B7, 0x00, 0x0},                                                   \
        {0x31B8, 0x00, 0x0},                                                   \
        {0x31B9, 0x0D, 0x0},                                                   \
        {0x31BA, 0x04, 0x0},                                                   \
        {0x31BB, 0x00, 0x0},                                                   \
        {0x31BC, 0x00, 0x0},                                                   \
        {0x31BD, 0x00, 0x0},                                                   \
        {0x31BE, 0x00, 0x0},                                                   \
        {0x31BF, 0x00, 0x0},                                                   \
        {0x31C0, 0x00, 0x0},                                                   \
        {0x31C1, 0x1F, 0x0},                                                   \
        {0x31C2, 0x53, 0x0},                                                   \
        {0x31C3, 0x06, 0x0},                                                   \
        {0x31C4, 0x16, 0x0},                                                   \
        {0x31C5, 0x00, 0x0},                                                   \
        {0x31C6, 0x00, 0x0},                                                   \
        {0x31C7, 0x00, 0x0},                                                   \
        {0x31C8, 0x00, 0x0},                                                   \
        {0x31C9, 0x00, 0x0},                                                   \
        {0x31CA, 0x00, 0x0},                                                   \
        {0x31CB, 0x00, 0x0},                                                   \
        {0x31CC, 0x00, 0x0},                                                   \
        {0x31CD, 0x00, 0x0},                                                   \
        {0x31CE, 0x00, 0x0},                                                   \
        {0x31CF, 0x00, 0x0},                                                   \
        {0x31D0, 0x00, 0x0},                                                   \
        {0x31D1, 0x00, 0x0},                                                   \
        {0x31D2, 0x00, 0x0},                                                   \
        {0x31D3, 0x00, 0x0},                                                   \
        {0x31D4, 0x00, 0x0},                                                   \
        {0x31D5, 0x00, 0x0},                                                   \
        {0x31D6, 0x00, 0x0},                                                   \
        {0x31D7, 0x00, 0x0},                                                   \
        {0x31D8, 0x00, 0x0},                                                   \
        {0x31D9, 0x00, 0x0},                                                   \
        {0x31DA, 0x00, 0x0},                                                   \
        {0x31DB, 0x00, 0x0},                                                   \
        {0x31DC, 0x00, 0x0},                                                   \
        {0x31DD, 0x00, 0x0},                                                   \
        {0x31DE, 0x00, 0x0},                                                   \
        {0x31DF, 0x00, 0x0},                                                   \
        {0x31E0, 0x00, 0x0},                                                   \
        {0x31E1, 0x00, 0x0},                                                   \
        {0x31E2, 0x00, 0x0},                                                   \
        {0x31E3, 0x00, 0x0},                                                   \
        {0x31E4, 0x00, 0x0},                                                   \
        {0x31E5, 0x00, 0x0},                                                   \
        {0x31E6, 0x00, 0x0},                                                   \
        {0x31E7, 0x00, 0x0},                                                   \
        {0x31E8, 0x01, 0x0},                                                   \
        {0x31E9, 0x53, 0x0},                                                   \
        {0x31EA, 0x0A, 0x0},                                                   \
        {0x31EB, 0xD7, 0x0},                                                   \
        {0x31EC, 0x0F, 0x0},                                                   \
        {0x31ED, 0x38, 0x0}, /* 0E */                                          \
        {0x31EE, 0x00, 0x0},                                                   \
        {0x31EF, 0x00, 0x0},                                                   \
        {0x31F0, 0x00, 0x0},                                                   \
        {0x31F1, 0x00, 0x0},                                                   \
        {0x31F2, 0x00, 0x0},                                                   \
        {0x31F3, 0x78, 0x0},                                                   \
        {0x31F4, 0x04, 0x0},                                                   \
        {0x31F5, 0x00, 0x0},                                                   \
        {0x31F6, 0x00, 0x0},                                                   \
        {0x31F7, 0x00, 0x0},                                                   \
        {0x31F8, 0x00, 0x0},                                                   \
        {0x31F9, 0x00, 0x0},                                                   \
        {0x31FA, 0x00, 0x0},                                                   \
        {0x31FB, 0x00, 0x0},                                                   \
        {0x31FC, 0x00, 0x0},                                                   \
        {0x31FD, 0x00, 0x0},                                                   \
        {0x31FE, 0x00, 0x0},                                                   \
                                                                               \
        /*                                                                     \
        ID:04                                                                  \
        Mode:1280*960@30fps                                                    \
        */                                                                     \
        {0x3200, 0xAA, 0x0},                                                   \
        {0x3201, 0x00, 0x0},                                                   \
        {0x3202, 0x00, 0x0},                                                   \
        {0x3203, 0x5A, 0x0},                                                   \
        {0x3204, 0xE0, 0x0},                                                   \
        {0x3205, 0x15, 0x0},                                                   \
        {0x3206, 0x56, 0x0},                                                   \
        {0x3207, 0xF0, 0x0},                                                   \
        {0x3208, 0x23, 0x0},                                                   \
        {0x3209, 0xFF, 0x0},                                                   \
        {0x320A, 0x0F, 0x0},                                                   \
        {0x320B, 0x00, 0x0},                                                   \
        {0x320C, 0xCF, 0x0},/* 0xD4, */                                        \
        {0x320D, 0x80, 0x0},                                                   \
        {0x320E, 0x05, 0x0},                                                   \
        {0x320F, 0x32, 0x0},                                                   \
        {0x3210, 0x90, 0x0},                                                   \
        {0x3211, 0x19, 0x0},                                                   \
        {0x3212, 0x2C, 0x0},                                                   \
        {0x3213, 0x41, 0x0},                                                   \
        {0x3214, 0x06, 0x0},                                                   \
        {0x3215, 0x13, 0x0},                                                   \
        {0x3216, 0x50, 0x0},                                                   \
        {0x3217, 0x13, 0x0},                                                   \
        {0x3218, 0x01, 0x0},                                                   \
        {0x3219, 0x40, 0x0},                                                   \
        {0x321A, 0x08, 0x0},                                                   \
        {0x321B, 0x31, 0x0},                                                   \
        {0x321C, 0x02, 0x0},                                                   \
        {0x321D, 0x30, 0x0},                                                   \
        {0x321E, 0x00, 0x0},                                                   \
        {0x321F, 0x00, 0x0},                                                   \
        {0x3220, 0x32, 0x0},                                                   \
        {0x3221, 0x06, 0x0},                                                   \
        {0x3222, 0x71, 0x0},                                                   \
        {0x3223, 0x10, 0x0},                                                   \
        {0x3224, 0x00, 0x0},                                                   \
        {0x3225, 0x21, 0x0},                                                   \
        {0x3226, 0x05, 0x0},                                                   \
        {0x3227, 0x00, 0x0},                                                   \
        {0x3228, 0x00, 0x0},                                                   \
        {0x3229, 0x00, 0x0},                                                   \
        {0x322A, 0x01, 0x0},                                                   \
        {0x322B, 0x00, 0x0},                                                   \
        {0x322C, 0x00, 0x0},                                                   \
        {0x322D, 0x03, 0x0},                                                   \
        {0x322E, 0x06, 0x0},                                                   \
        {0x322F, 0x40, 0x0},                                                   \
        {0x3230, 0x03, 0x0},                                                   \
        {0x3231, 0x06, 0x0},                                                   \
        {0x3232, 0x60, 0x0},                                                   \
        {0x3233, 0x1A, 0x0},                                                   \
        {0x3234, 0x00, 0x0},                                                   \
        {0x3235, 0xD7, 0x0},                                                   \
        {0x3236, 0x50, 0x0},                                                   \
        {0x3237, 0x10, 0x0},                                                   \
        {0x3238, 0xD9, 0x0},                                                   \
        {0x3239, 0xE0, 0x0},                                                   \
        {0x323A, 0x0F, 0x0},                                                   \
        {0x323B, 0x00, 0x0},                                                   \
        {0x323C, 0xD9, 0x0},                                                   \
        {0x323D, 0xE0, 0x0},                                                   \
        {0x323E, 0x0F, 0x0},                                                   \
        {0x323F, 0x00, 0x0},                                                   \
        {0x3240, 0xDA, 0x0},                                                   \
        {0x3241, 0x80, 0x0},                                                   \
        {0x3242, 0x0F, 0x0},                                                   \
        {0x3243, 0x00, 0x0},                                                   \
        {0x3244, 0xF9, 0x0},                                                   \
        {0x3245, 0xD0, 0x0},                                                   \
        {0x3246, 0x0F, 0x0},                                                   \
        {0x3247, 0x00, 0x0},                                                   \
        {0x3248, 0x07, 0x0},                                                   \
        {0x3249, 0x40, 0x0},                                                   \
        {0x324A, 0x1A, 0x0},                                                   \
        {0x324B, 0x07, 0x0},                                                   \
        {0x324C, 0x40, 0x0},/* 0xD0, */                                        \
        {0x324D, 0x03, 0x0},/* 0x01, */                                        \
        {0x324E, 0xFF, 0x0},                                                   \
        {0x324F, 0xFF, 0x0},                                                   \
        {0x3250, 0xFF, 0x0},                                                   \
        {0x3251, 0x00, 0x0},                                                   \
        {0x3252, 0xD8, 0x0},                                                   \
        {0x3253, 0x40, 0x0},                                                   \
        {0x3254, 0x10, 0x0},                                                   \
        {0x3255, 0x00, 0x0},                                                   \
        {0x3256, 0xD9, 0x0},                                                   \
        {0x3257, 0xE0, 0x0},                                                   \
        {0x3258, 0x0F, 0x0},                                                   \
        {0x3259, 0xFF, 0x0},                                                   \
        {0x325A, 0xFF, 0x0},                                                   \
        {0x325B, 0xFF, 0x0},                                                   \
        {0x325C, 0x00, 0x0},                                                   \
        {0x325D, 0x08, 0x0},                                                   \
        {0x325E, 0xE0, 0x0},                                                   \
        {0x325F, 0x19, 0x0},                                                   \
        {0x3260, 0x08, 0x0},                                                   \
        {0x3261, 0xE0, 0x0},/* 0x70, */                                        \
        {0x3262, 0x02, 0x0},/* 0x01, */                                        \
        {0x3263, 0x00, 0x0},                                                   \
        {0x3264, 0xFF, 0x0},                                                   \
        {0x3265, 0x30, 0x0},                                                   \
        {0x3266, 0x10, 0x0},                                                   \
        {0x3267, 0xFF, 0x0},                                                   \
        {0x3268, 0xFF, 0x0},                                                   \
        {0x3269, 0xFF, 0x0},                                                   \
        {0x326A, 0x00, 0x0},                                                   \
        {0x326B, 0x9F, 0x0},                                                   \
        {0x326C, 0x31, 0x0},                                                   \
        {0x326D, 0x1A, 0x0},                                                   \
        {0x326E, 0x2F, 0x0},/* 0x18, */                                        \
        {0x326F, 0x30, 0x0},/* 0xC0, */                                        \
        {0x3270, 0x03, 0x0},/* 0x01, */                                        \
        {0x3271, 0x00, 0x0},                                                   \
        {0x3272, 0xF9, 0x0},                                                   \
        {0x3273, 0x00, 0x0},                                                   \
        {0x3274, 0x1A, 0x0},                                                   \
        {0x3275, 0x00, 0x0},                                                   \
        {0x3276, 0x00, 0x0},                                                   \
        {0x3277, 0x00, 0x0},                                                   \
        {0x3278, 0x00, 0x0},                                                   \
        {0x3279, 0x00, 0x0},                                                   \
        {0x327A, 0x00, 0x0},                                                   \
        {0x327B, 0x00, 0x0},                                                   \
        {0x327C, 0x00, 0x0},                                                   \
        {0x327D, 0x00, 0x0},                                                   \
        {0x327E, 0x90, 0x0},                                                   \
        {0x327F, 0x1A, 0x0},                                                   \
        {0x3280, 0xFF, 0x0},                                                   \
        {0x3281, 0xFF, 0x0},                                                   \
        {0x3282, 0xFF, 0x0},                                                   \
        {0x3283, 0x00, 0x0},                                                   \
        {0x3284, 0x05, 0x0},                                                   \
        {0x3285, 0x90, 0x0},                                                   \
        {0x3286, 0x1A, 0x0},                                                   \
        {0x3287, 0xFF, 0x0},                                                   \
        {0x3288, 0xFF, 0x0},                                                   \
        {0x3289, 0xFF, 0x0},                                                   \
        {0x328A, 0x00, 0x0},                                                   \
        {0x328B, 0x83, 0x0},                                                   \
        {0x328C, 0x00, 0x0},                                                   \
        {0x328D, 0x1A, 0x0},                                                   \
        {0x328E, 0x00, 0x0},                                                   \
        {0x328F, 0x00, 0x0},                                                   \
        {0x3290, 0x00, 0x0},                                                   \
        {0x3291, 0x00, 0x0},                                                   \
        {0x3292, 0x00, 0x0},                                                   \
        {0x3293, 0x00, 0x0},                                                   \
        {0x3294, 0x00, 0x0},                                                   \
        {0x3295, 0x00, 0x0},                                                   \
        {0x3296, 0x00, 0x0},                                                   \
        {0x3297, 0x88, 0x0},                                                   \
        {0x3298, 0x00, 0x0},/* 0x40, */                                        \
        {0x3299, 0x08, 0x0},                                                   \
        {0x329A, 0x12, 0x0},/* 0xE0, */                                        \
        {0x329B, 0xF1, 0x0},/* 0xC0, */                                        \
        {0x329C, 0x0C, 0x0},/* 0x0D, */                                        \
        {0x329D, 0x00, 0x0},                                                   \
        {0x329E, 0x0B, 0x0},                                                   \
        {0x329F, 0x80, 0x0},                                                   \
        {0x32A0, 0x00, 0x0},                                                   \
        {0x32A1, 0x0B, 0x0},                                                   \
        {0x32A2, 0x80, 0x0},                                                   \
        {0x32A3, 0x00, 0x0},                                                   \
        {0x32A4, 0x7A, 0x0},                                                   \
        {0x32A5, 0x40, 0x0},                                                   \
        {0x32A6, 0x00, 0x0},                                                   \
        {0x32A7, 0x00, 0x0},                                                   \
        {0x32A8, 0x04, 0x0},                                                   \
        {0x32A9, 0xC0, 0x0},                                                   \
        {0x32AA, 0x06, 0x0},                                                   \
        {0x32AB, 0x00, 0x0},                                                   \
        {0x32AC, 0x04, 0x0},                                                   \
        {0x32AD, 0x10, 0x0},                                                   \
        {0x32AE, 0x08, 0x0},                                                   \
        {0x32AF, 0x00, 0x0},                                                   \
        {0x32B0, 0x04, 0x0},                                                   \
        {0x32B1, 0x10, 0x0},                                                   \
        {0x32B2, 0x08, 0x0},                                                   \
        {0x32B3, 0x00, 0x0},                                                   \
        {0x32B4, 0xD9, 0x0},                                                   \
        {0x32B5, 0x00, 0x0},                                                   \
        {0x32B6, 0x00, 0x0},                                                   \
        {0x32B7, 0x00, 0x0},                                                   \
        {0x32B8, 0x08, 0x0},                                                   \
        {0x32B9, 0x40, 0x0},                                                   \
        {0x32BA, 0x00, 0x0},                                                   \
        {0x32BB, 0x00, 0x0},                                                   \
        {0x32BC, 0x84, 0x0},                                                   \
        {0x32BD, 0x10, 0x0},                                                   \
        {0x32BE, 0x08, 0x0},                                                   \
        {0x32BF, 0x00, 0x0},                                                   \
        {0x32C0, 0x08, 0x0},                                                   \
        {0x32C1, 0x50, 0x0},                                                   \
        {0x32C2, 0x00, 0x0},                                                   \
        {0x32C3, 0xDC, 0x0},                                                   \
        {0x32C4, 0x90, 0x0},                                                   \
        {0x32C5, 0x0D, 0x0},                                                   \
        {0x32C6, 0x00, 0x0},                                                   \
        {0x32C7, 0x08, 0x0},                                                   \
        {0x32C8, 0x50, 0x0},                                                   \
        {0x32C9, 0x00, 0x0},                                                   \
        {0x32CA, 0xA0, 0x0},                                                   \
        {0x32CB, 0x01, 0x0},                                                   \
        {0x32CC, 0x05, 0x0},                                                   \
        {0x32CD, 0x00, 0x0},                                                   \
        {0x32CE, 0x50, 0x0},                                                   \
        {0x32CF, 0x00, 0x0},                                                   \
        {0x32D0, 0x1A, 0x0},                                                   \
        {0x32D1, 0x01, 0x0},                                                   \
        {0x32D2, 0x00, 0x0},                                                   \
        {0x32D3, 0x00, 0x0},                                                   \
        {0x32D4, 0x00, 0x0},                                                   \
        {0x32D5, 0x00, 0x0},                                                   \
        {0x32D6, 0x9E, 0x0},                                                   \
        {0x32D7, 0x01, 0x0},                                                   \
        {0x32D8, 0x09, 0x0},                                                   \
        {0x32D9, 0x00, 0x0},                                                   \
        {0x32DA, 0x9A, 0x0},                                                   \
        {0x32DB, 0xF0, 0x0},                                                   \
        {0x32DC, 0x0C, 0x0},                                                   \
        {0x32DD, 0x30, 0x0},                                                   \
        {0x32DE, 0x41, 0x0},                                                   \
        {0x32DF, 0x19, 0x0},                                                   \
        {0x32E0, 0x00, 0x0},                                                   \
        {0x32E1, 0x1E, 0x0},                                                   \
        {0x32E2, 0x30, 0x0},                                                   \
        {0x32E3, 0x05, 0x0},                                                   \
        {0x32E4, 0x5C, 0x0},                                                   \
        {0x32E5, 0x00, 0x0},                                                   \
        {0x32E6, 0x0C, 0x0},                                                   \
        {0x32E7, 0x96, 0x0},                                                   \
        {0x32E8, 0x30, 0x0},                                                   \
        {0x32E9, 0x0D, 0x0},                                                   \
        {0x32EA, 0x2C, 0x0},                                                   \
        {0x32EB, 0x81, 0x0},                                                   \
        {0x32EC, 0x19, 0x0},                                                   \
        {0x32ED, 0x00, 0x0},                                                   \
        {0x32EE, 0x1A, 0x0},                                                   \
        {0x32EF, 0x70, 0x0},                                                   \
        {0x32F0, 0x05, 0x0},                                                   \
        {0x32F1, 0x58, 0x0},                                                   \
        {0x32F2, 0x40, 0x0},                                                   \
        {0x32F3, 0x0C, 0x0},                                                   \
        {0x32F4, 0x8C, 0x0},                                                   \
        {0x32F5, 0x20, 0x0},                                                   \
        {0x32F6, 0x09, 0x0},                                                   \
        {0x32F7, 0x00, 0x0},                                                   \
        {0x32F8, 0x84, 0x0},                                                   \
        {0x32F9, 0x40, 0x0},                                                   \
        {0x32FA, 0x07, 0x0},                                                   \
        {0x32FB, 0x1A, 0x0},                                                   \
        {0x32FC, 0xA1, 0x0},                                                   \
        {0x32FD, 0x10, 0x0},                                                   \
        {0x32FE, 0x00, 0x0},                                                   \
                                                                               \
                                                                               \
    /*   ID:05 */                                                              \
    /*   Mode:1280*960@30fps */                                                \
                                                                               \
        {0x3300, 0x00, 0x0},                                                   \
        {0x3301, 0x85, 0x0},                                                   \
        {0x3302, 0x50, 0x0},                                                   \
        {0x3303, 0x07, 0x0},                                                   \
        {0x3304, 0x1B, 0x0},                                                   \
        {0x3305, 0xB1, 0x0},                                                   \
        {0x3306, 0x10, 0x0},                                                   \
        {0x3307, 0x00, 0x0},                                                   \
        {0x3308, 0x7C, 0x0},                                                   \
        {0x3309, 0xC0, 0x0},                                                   \
        {0x330A, 0x08, 0x0},                                                   \
        {0x330B, 0x12, 0x0},                                                   \
        {0x330C, 0x21, 0x0},                                                   \
        {0x330D, 0x12, 0x0},                                                   \
        {0x330E, 0x00, 0x0},                                                   \
        {0x330F, 0x7D, 0x0},                                                   \
        {0x3310, 0xD0, 0x0},                                                   \
        {0x3311, 0x08, 0x0},                                                   \
        {0x3312, 0x13, 0x0},                                                   \
        {0x3313, 0x31, 0x0},                                                   \
        {0x3314, 0x12, 0x0},                                                   \
        {0x3315, 0x00, 0x0},                                                   \
        {0x3316, 0x7E, 0x0},                                                   \
        {0x3317, 0x20, 0x0},                                                   \
        {0x3318, 0x08, 0x0},                                                   \
        {0x3319, 0x14, 0x0},                                                   \
        {0x331A, 0x81, 0x0},                                                   \
        {0x331B, 0x11, 0x0},                                                   \
        {0x331C, 0x00, 0x0},                                                   \
        {0x331D, 0x86, 0x0},                                                   \
        {0x331E, 0xA0, 0x0},                                                   \
        {0x331F, 0x08, 0x0},                                                   \
        {0x3320, 0x1C, 0x0},                                                   \
        {0x3321, 0x01, 0x0},                                                   \
        {0x3322, 0x12, 0x0},                                                   \
        {0x3323, 0x00, 0x0},                                                   \
        {0x3324, 0x8C, 0x0},                                                   \
        {0x3325, 0x20, 0x0},                                                   \
        {0x3326, 0x09, 0x0},                                                   \
        {0x3327, 0x00, 0x0},                                                   \
        {0x3328, 0xCF, 0x0},                                                   \
        {0x3329, 0x10, 0x0},                                                   \
        {0x332A, 0x0D, 0x0},                                                   \
        {0x332B, 0x00, 0x0},                                                   \
        {0x332C, 0x53, 0x0},                                                   \
        {0x332D, 0x50, 0x0},                                                   \
        {0x332E, 0x05, 0x0},                                                   \
        {0x332F, 0x00, 0x0},                                                   \
        {0x3330, 0x01, 0x0},                                                   \
        {0x3331, 0x20, 0x0},                                                   \
        {0x3332, 0x00, 0x0},                                                   \
        {0x3333, 0x00, 0x0},                                                   \
        {0x3334, 0x45, 0x0},                                                   \
        {0x3335, 0xB1, 0x0},                                                   \
        {0x3336, 0x19, 0x0},                                                   \
        {0x3337, 0x01, 0x0},                                                   \
        {0x3338, 0x00, 0x0},                                                   \
        {0x3339, 0x00, 0x0},                                                   \
        {0x333A, 0x00, 0x0},                                                   \
        {0x333B, 0x00, 0x0},                                                   \
        {0x333C, 0x00, 0x0},                                                   \
        {0x333D, 0x00, 0x0},                                                   \
        {0x333E, 0x00, 0x0},                                                   \
        {0x333F, 0x00, 0x0},                                                   \
        {0x3340, 0x00, 0x0},                                                   \
        {0x3341, 0x11, 0x0},                                                   \
        {0x3342, 0x11, 0x0},                                                   \
        {0x3343, 0x00, 0x0},                                                   \
        {0x3344, 0x20, 0x0},                                                   \
        {0x3345, 0x00, 0x0},                                                   \
        {0x3346, 0x03, 0x0},                                                   \
        {0x3347, 0x00, 0x0},                                                   \
        {0x3348, 0x00, 0x0},                                                   \
        {0x3349, 0x00, 0x0},                                                   \
        {0x334A, 0x00, 0x0},                                                   \
        {0x334B, 0x00, 0x0},                                                   \
        {0x334C, 0x00, 0x0},                                                   \
        {0x334D, 0x00, 0x0},                                                   \
        {0x334E, 0x0F, 0x0},                                                   \
        {0x334F, 0x20, 0x0},                                                   \
        {0x3350, 0x00, 0x0},                                                   \
        {0x3351, 0x0F, 0x0},                                                   \
        {0x3352, 0x01, 0x0},                                                   \
        {0x3353, 0x0E, 0x0},                                                   \
        {0x3354, 0x01, 0x0},                                                   \
        {0x3355, 0x00, 0x0},                                                   \
        {0x3356, 0x00, 0x0},                                                   \
        {0x3357, 0xD1, 0x0},                                                   \
        {0x3358, 0x03, 0x0},                                                   \
        {0x3359, 0x01, 0x0},                                                   \
        {0x335A, 0x00, 0x0},                                                   \
        {0x335B, 0x40, 0x0},                                                   \
        {0x335C, 0x00, 0x0},                                                   \
        {0x335D, 0x00, 0x0},                                                   \
        {0x335E, 0x00, 0x0},                                                   \
        {0x335F, 0x00, 0x0},                                                   \
        {0x3360, 0x00, 0x0},                                                   \
        {0x3361, 0x00, 0x0},                                                   \
        {0x3362, 0x00, 0x0},                                                   \
        {0x3363, 0x00, 0x0},                                                   \
        {0x3364, 0x00, 0x0},                                                   \
        {0x3365, 0x16, 0x0},                                                   \
        {0x3366, 0x00, 0x0},                                                   \
        {0x3367, 0x00, 0x0},                                                   \
        {0x3368, 0x00, 0x0},                                                   \
        {0x3369, 0x00, 0x0},                                                   \
        {0x336A, 0x00, 0x0},                                                   \
        {0x336B, 0x27, 0x0},                                                   \
        {0x336C, 0x1F, 0x0},                                                   \
        {0x336D, 0x03, 0x0},                                                   \
        {0x336E, 0x00, 0x0},                                                   \
        {0x336F, 0x00, 0x0},                                                   \
        {0x3370, 0x00, 0x0},                                                   \
        {0x3371, 0x00, 0x0},                                                   \
        {0x3372, 0x00, 0x0},                                                   \
        {0x3373, 0x00, 0x0},                                                   \
        {0x3374, 0x00, 0x0},                                                   \
        {0x3375, 0x00, 0x0},                                                   \
        {0x3376, 0x00, 0x0},                                                   \
        {0x3377, 0x00, 0x0},                                                   \
        {0x3378, 0x00, 0x0},                                                   \
        {0x3379, 0x00, 0x0},                                                   \
        {0x337A, 0x00, 0x0},                                                   \
        {0x337B, 0x00, 0x0},                                                   \
        {0x337C, 0x00, 0x0},                                                   \
        {0x337D, 0x0C, 0x0},                                                   \
        {0x337E, 0x0C, 0x0},                                                   \
        {0x337F, 0x03, 0x0},                                                   \
        {0x3380, 0x00, 0x0},                                                   \
        {0x3381, 0x1B, 0x0},/* 0x36,no need for p */                           \
        {0x3382, 0x57, 0x0},                                                   \
        {0x3383, 0x0F, 0x0},                                                   \
        {0x3384, 0x27, 0x0},                                                   \
        {0x3385, 0x0F, 0x0},                                                   \
        {0x3386, 0x0F, 0x0},                                                   \
        {0x3387, 0x07, 0x0},                                                   \
        {0x3388, 0x37, 0x0},                                                   \
        {0x3389, 0x1F, 0x0},                                                   \
        {0x338A, 0x30, 0x0},                                                   \
        {0x338B, 0x30, 0x0},                                                   \
        {0x338C, 0x30, 0x0},                                                   \
        {0x338D, 0x3D, 0x0},/* 0x67,no need for p */                           \
        {0x338E, 0x01, 0x0},/* 0x03,no need for p */                           \
        {0x338F, 0xBA, 0x0},                                                   \
        {0x3390, 0x2C, 0x0},                                                   \
        {0x3391, 0x18, 0x0},                                                   \
        {0x3392, 0x08, 0x0},                                                   \
        {0x3393, 0x08, 0x0},                                                   \
        {0x3394, 0x01, 0x0},                                                   \
        {0x3395, 0x00, 0x0},                                                   \
        {0x3396, 0x00, 0x0},                                                   \
        {0x3397, 0x00, 0x0},                                                   \
        {0x3398, 0x00, 0x0},                                                   \
        {0x3399, 0x24, 0x0},                                                   \
        {0x339A, 0x02, 0x0},                                                   \
        {0x339B, 0x01, 0x0},                                                   \
        {0x339C, 0x01, 0x0},                                                   \
        {0x339D, 0x00, 0x0},                                                   \
        {0x339E, 0x01, 0x0},                                                   \
        {0x339F, 0xF0, 0x0},                                                   \
        {0x33A0, 0x00, 0x0},                                                   \
        {0x33A1, 0x0C, 0x0},                                                   \
        {0x33A2, 0x0C, 0x0},                                                   \
        {0x33A3, 0x00, 0x0},                                                   \
        {0x33A4, 0x00, 0x0},                                                   \
        {0x33A5, 0x00, 0x0},                                                   \
        {0x33A6, 0xF0, 0x0},                                                   \
        {0x33A7, 0x00, 0x0},                                                   \
        {0x33A8, 0x01, 0x0},                                                   \
        {0x33A9, 0x00, 0x0},                                                   \
        {0x33AA, 0x00, 0x0},                                                   \
        {0x33AB, 0x00, 0x0},                                                   \
        {0x33AC, 0x00, 0x0},                                                   \
        {0x33AD, 0xF0, 0x0},                                                   \
        {0x33AE, 0x00, 0x0},                                                   \
        {0x33AF, 0x01, 0x0},                                                   \
        {0x33B0, 0x00, 0x0},                                                   \
        {0x33B1, 0xF0, 0x0},                                                   \
        {0x33B2, 0x00, 0x0},                                                   \
        {0x33B3, 0x00, 0x0},                                                   \
        {0x33B4, 0x00, 0x0},                                                   \
        {0x33B5, 0x00, 0x0},                                                   \
        {0x33B6, 0x00, 0x0},                                                   \
        {0x33B7, 0x00, 0x0},                                                   \
        {0x33B8, 0x00, 0x0},                                                   \
        {0x33B9, 0x00, 0x0},                                                   \
        {0x33BA, 0x00, 0x0},                                                   \
        {0x33BB, 0x00, 0x0},                                                   \
        {0x33BC, 0x00, 0x0},                                                   \
        {0x33BD, 0x00, 0x0},                                                   \
        {0x33BE, 0x00, 0x0},                                                   \
        {0x33BF, 0x00, 0x0},                                                   \
        {0x33C0, 0x00, 0x0},                                                   \
        {0x33C1, 0x00, 0x0},                                                   \
        {0x33C2, 0x00, 0x0},                                                   \
        {0x33C3, 0x00, 0x0},                                                   \
        {0x33C4, 0x00, 0x0},                                                   \
        {0x33C5, 0x00, 0x0},                                                   \
        {0x33C6, 0x00, 0x0},                                                   \
        {0x33C7, 0x00, 0x0},                                                   \
        {0x33C8, 0x00, 0x0},                                                   \
        {0x33C9, 0x65, 0x0},                                                   \
        {0x33CA, 0x04, 0x0},                                                   \
        {0x33CB, 0x00, 0x0},                                                   \
        {0x33CC, 0x98, 0x0},                                                   \
        {0x33CD, 0x08, 0x0},                                                   \
        {0x33CE, 0x00, 0x0},                                                   \
        {0x33CF, 0x00, 0x0},                                                   \
        {0x33D0, 0x00, 0x0},                                                   \
        {0x33D1, 0x00, 0x0},                                                   \
        {0x33D2, 0x9B, 0x0},                                                   \
        {0x33D3, 0x07, 0x0},                                                   \
        {0x33D4, 0xC8, 0x0},                                                   \
        {0x33D5, 0x04, 0x0},                                                   \
        {0x33D6, 0x9C, 0x0},                                                   \
        {0x33D7, 0x07, 0x0},                                                   \
        {0x33D8, 0xC9, 0x0},                                                   \
        {0x33D9, 0x04, 0x0},                                                   \
        {0x33DA, 0x00, 0x0},                                                   \
        {0x33DB, 0x00, 0x0},                                                   \
        {0x33DC, 0x00, 0x0},                                                   \
        {0x33DD, 0x00, 0x0},                                                   \
        {0x33DE, 0x00, 0x0},                                                   \
        {0x33DF, 0x00, 0x0},                                                   \
        {0x33E0, 0x1B, 0x0},                                                   \
        {0x33E1, 0x05, 0x0},                                                   \
        {0x33E2, 0xD0, 0x0},                                                   \
        {0x33E3, 0x03, 0x0},                                                   \
        {0x33E4, 0x7C, 0x0},                                                   \
        {0x33E5, 0x01, 0x0},                                                   \
        {0x33E6, 0x31, 0x0},                                                   \
        {0x33E7, 0x01, 0x0},                                                   \
        {0x33E8, 0x1C, 0x0},                                                   \
        {0x33E9, 0x05, 0x0},                                                   \
        {0x33EA, 0xD1, 0x0},                                                   \
        {0x33EB, 0x03, 0x0},                                                   \
        {0x33EC, 0x01, 0x0},                                                   \
        {0x33ED, 0x03, 0x0},                                                   \
        {0x33EE, 0x00, 0x0},                                                   \
        {0x33EF, 0x00, 0x0},                                                   \
        {0x33F0, 0x0B, 0x0},                                                   \
        {0x33F1, 0x00, 0x0},                                                   \
        {0x33F2, 0x0C, 0x0},                                                   \
        {0x33F3, 0x00, 0x0},                                                   \
        {0x33F4, 0x00, 0x0},                                                   \
        {0x33F5, 0x00, 0x0},                                                   \
        {0x33F6, 0x0F, 0x0},                                                   \
        {0x33F7, 0x00, 0x0},                                                   \
        {0x33F8, 0x06, 0x0},                                                   \
        {0x33F9, 0x00, 0x0},                                                   \
        {0x33FA, 0x10, 0x0},                                                   \
        {0x33FB, 0x00, 0x0},                                                   \
        {0x33FC, 0x00, 0x0},                                                   \
        {0x33FD, 0x00, 0x0},                                                   \
        {0x33FE, 0x00, 0x0},                                                   \
        {0x3000, 0x00, 0x0}                                                    \
}

#endif /* IMX224_LINEAR_1280X960_CONFIG_H_ */
