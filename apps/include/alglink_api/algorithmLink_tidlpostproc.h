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

 THIS SOFTWARE IS PROVIDED BY TI AND TI’S LICENSORS "AS IS" AND ANY EXPRESS OR
 IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL TI AND TI’S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/

/**
 *******************************************************************************
 *
 * \ingroup  ALGORITHM_LINK_PLUGIN
 * \defgroup ALGORITHM_LINK_TIDLPOSTPROC  Bitmap Colot Table Look up API
 *
 * \brief  This module has the interface for using Deep Learning algorithm
 *
 * @{
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \file algorithmLink_tidlpostproc.h
 *
 * \brief Algorithm Link API specific to Bitmap Color LUT algorithm
 *
 *******************************************************************************
 */

#ifndef ALGORITHM_LINK_TIDLPOSTPROC_H_
#define ALGORITHM_LINK_TIDLPOSTPROC_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  Include files
 *******************************************************************************
 */
#include <include/link_api/system.h>
#include <include/link_api/algorithmLink.h>

#define TIDLPOSTPROC_LINK_MAX_NUMQUEUES          (4)

/*******************************************************************************
 *  Data structures
 *******************************************************************************
 */
typedef enum
{
    TIDLPOSTPROC_MODE_CLUT,
    TIDLPOSTPROC_MODE_COPY

}AlgorithmLink_tidlpostprocMode;


typedef struct
{
    AlgorithmLink_CreateParams baseClassCreate;
    /**< Base class create params. This structure should be first element */
    System_LinkInQueParams inQueParams[TIDLPOSTPROC_LINK_MAX_NUMQUEUES];
    /**< Input Queue Parameters */
    System_LinkOutQueParams outQueParams;
    /**< Output Queue Parameters */
    System_VideoDataFormat outDataFormat;
    /**< Output Data format */
    UInt32 outWidth;
    /**< Output Width */
    UInt32 outHeight;
    /**< Output Height */
    UInt8 (*colPalette888)[3];
    /**< Color Palette Address */
    UInt16 *colPalette565;
    /**< Color Palette Address */
    UInt32 inPad;
    /**< Input Padding */
    UInt32 numInputQueues;
    /**< No of input queues */
    AlgorithmLink_tidlpostprocMode mode;
    /**< Alg mode */
}AlgorithmLink_tidlpostprocCreateParams;

/**
 *******************************************************************************
 *
 * \brief Set defaults for plugin create parameters
 *
 * \param pPrm  [OUT] plugin create parameters
 *
 *******************************************************************************
 */
static inline void AlgorithmLink_tidlpostprocCreateParams_Init(
                                        AlgorithmLink_tidlpostprocCreateParams *pPrm)
{
    memset(pPrm, 0x00, sizeof(AlgorithmLink_tidlpostprocCreateParams));

    /* Default data format is RGB24_888 */
    pPrm->outDataFormat = SYSTEM_DF_RGB24_888;
}

/**
 *******************************************************************************
 *
 * \brief Implementation of function to init plugins()
 *
 *        This function will be called by AlgorithmLink_initAlgPlugins, so as
 *        register plugins of sfm algorithm
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 AlgorithmLink_tidlpostproc_initPlugin(Void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/* @} */

/* Nothing beyond this point */
