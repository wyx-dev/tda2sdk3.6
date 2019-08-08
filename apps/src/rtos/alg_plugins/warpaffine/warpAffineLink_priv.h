/******************************************************************************
*
* Copyright (C) 2019-2020 Momenta Incorporated - https://www.momenta.ai/
*
* ALL RIGHTS RESERVED
*
******************************************************************************/

/**
 *******************************************************************************
 *
 * \ingroup ALGORITHM_LINK_API
 * \defgroup ALGORITHM_LINK_IMPL Algorithm Link Implementation
 *
 * @{
 */

/**
 *******************************************************************************
 *
 * \file warpAffineLink_priv.h Warp Affine Link private API/Data
 *       structures
 *
 * \brief  This link private header file has defined
 *         - Algorithm link instance/handle object
 *         - All the local data structures
 *         - Algorithm plug in function interfaces
 *
 * \version 0.0 (Feb 2019) : [NN] First version
 *
 *******************************************************************************
 */

#ifndef _WARPAFFINE_LINK_PRIV_H_
#define _WARPAFFINE_LINK_PRIV_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  Include files
 *******************************************************************************
 */
#include <include/link_api/system.h>
#include <include/link_api/algorithmLink.h>
#include <include/alglink_api/algorithmLink_warpAffine.h>
#include <include/link_api/algorithmLink_algPluginSupport.h>
#include <src/rtos/utils_common/include/utils_prf.h>
#include <edma_utils/edma_utils_memcpy.h>
#include <src/rtos/alg_plugins/common/include/alg_ivision.h>
#include <src/rtos/utils_common/include/utils_link_stats_if.h>
#include "warp_affine_algo.h"

/*******************************************************************************
 *  Enums
 *******************************************************************************
 */


/*******************************************************************************
 *  Defines
 *******************************************************************************
 */
#define WA_MAX_ALG_IN_BUFS                        (1)
#define WA_MAX_ALG_OUT_BUFS                       (1)

#define WA_ALGLINK_OUT_BUF_WIDTH                  (112)
#define WA_ALGLINK_OUT_BUF_HEIGHT                 (112)
#define WA_ALGLINK_OUT_BUF_CHANNEL                (3)
#define WA_ALGLINK_OUT_BUF_NUMBER                 (6)
#define WA_ALGLINK_OUT_BUF_SIZE                   (WA_ALGLINK_OUT_BUF_WIDTH * \
                                                   WA_ALGLINK_OUT_BUF_HEIGHT * \
                                                   WA_ALGLINK_OUT_BUF_CHANNEL * \
                                                   WA_ALGLINK_OUT_BUF_NUMBER)

/**
 *******************************************************************************
 *
 *   \brief Max number of buffers
 *
 *   WA generates flow vectors for the objects in the frame. Flow vectors
 *   give the direction of motion of the object. The WA algorithm generates
 *   flow vectors for a group of pixels.
 *
 *   SUPPORTED in ALL platforms
 *
 *******************************************************************************
 */
#define WARPAFFINE_LINK_MAX_NUM_OUTPUT            (8)

/**
 *******************************************************************************
 *
 *   \brief Max number of channels
 *
 *******************************************************************************
 */
#define WARPAFFINE_LINK_MAX_CH                    (5)

/*******************************************************************************
 *  Data structures
 *******************************************************************************
 */

typedef struct
{
    Void                  *algWaHandle;
    WA_CreateParams       algWaCreateParams;

    WA_InArgs             algWaInArgs;
    WA_OutArgs            algWaOutArgs;

    IVISION_InBufs        algWaInBufs;
    IVISION_OutBufs       algWaOutBufs;

    IVISION_BufDesc       algWaInBufDesc;
    IVISION_BufDesc       *algWaInBufDescList[WA_MAX_ALG_IN_BUFS];

    IVISION_BufDesc       algWaOutBufDesc;
    IVISION_BufDesc       *algWaOutBufDescList[WA_MAX_ALG_OUT_BUFS];

    Void                  *pAlgWaOutBuf;
    UInt32                algWaOutBufSize;

    System_Buffer         buffers[WARPAFFINE_LINK_MAX_NUM_OUTPUT];
    /**< System buffers to exchange data with next link */

    System_MetaDataBuffer metaDataBuffers[WARPAFFINE_LINK_MAX_NUM_OUTPUT];
    /**< Payload for the system buffers */

} AlgorithmLink_WarpAffineChObj;


/**
 *******************************************************************************
 *
 *   \brief Structure containing Warp Affine algorithm link
 *          parameters
 *
 *          This structure holds any algorithm parameters specific to this link.
 *
 *******************************************************************************
*/
typedef struct
{
    AlgorithmLink_WarpAffineCreateParams algLinkCreateParams;

    AlgorithmLink_WarpAffineChObj        chObj[WARPAFFINE_LINK_MAX_CH];

    AlgorithmLink_InputQueueInfo         inputQInfo;
    /**< All the information about input Queue*/

    AlgorithmLink_OutputQueueInfo        outputQInfo;
    /**< All the information about output Queue*/

    System_LinkStatistics                *linkStatsInfo;
    /**< Pointer to the Link statistics information,
         used to store below information
            1, min, max and average latency of the link
            2, min, max and average latency from source to this link
            3, links statistics like frames captured, dropped etc
        Pointer is assigned at the link create time from shared
        memory maintained by utils_link_stats layer */

    Bool                                 isFirstFrameRecv;
    /**< Flag to indicate if first frame is received, this is used as trigger
     *   to start stats counting
     */

} AlgorithmLink_WarpAffineObj;

/*******************************************************************************
 *  Algorithm Link Private Functions
 *******************************************************************************
 */
Int32 AlgorithmLink_warpAffineCreate(void *pObj, void *pCreateParams);
Int32 AlgorithmLink_warpAffineProcess(void *pObj);
Int32 AlgorithmLink_warpAffineControl(void *pObj, void *pControlParams);
Int32 AlgorithmLink_warpAffineStop(void *pObj);
Int32 AlgorithmLink_warpAffineDelete(void *pObj);
Int32 AlgorithmLink_warpAffinePrintStatistics(void *pObj,
                AlgorithmLink_WarpAffineObj *pWarpAffineObj);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/*@}*/

/* Nothing beyond this point */
