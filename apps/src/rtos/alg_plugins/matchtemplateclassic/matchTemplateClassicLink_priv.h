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
 * \file matchTemplateClassicLink_priv.h Match Template Classic Link private API/Data
 *       structures
 *
 * \brief  This link private header file has defined
 *         - Algorithm link instance/handle object
 *         - All the local data structures
 *         - Algorithm plug in function interfaces
 *
 * \version 0.0 (Apr 2019) : [NN] First version
 *
 *******************************************************************************
 */

#ifndef _MATCHTEMPLATECLASSIC_LINK_PRIV_H_
#define _MATCHTEMPLATECLASSIC_LINK_PRIV_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  Include files
 *******************************************************************************
 */
#include <include/link_api/system.h>
#include <include/link_api/algorithmLink.h>
#include <include/alglink_api/algorithmLink_matchTemplateClassic.h>
#include <include/link_api/algorithmLink_algPluginSupport.h>
#include <src/rtos/utils_common/include/utils_prf.h>
#include <edma_utils/edma_utils_memcpy.h>
#include <src/rtos/alg_plugins/common/include/alg_ivision.h>
#include <src/rtos/utils_common/include/utils_link_stats_if.h>
#include "match_template_classic_algo.h"

/*******************************************************************************
 *  Enums
 *******************************************************************************
 */


/*******************************************************************************
 *  Defines
 *******************************************************************************
 */
#define MTC_MAX_ALG_IN_BUFS                        (1)
#define MTC_MAX_ALG_OUT_BUFS                       (1)

#define MTC_ALGLINK_IMG_WIDTH_MAX                  (1280)
#define MTC_ALGLINK_IMG_HEIGHT_MAX                 (720)
#define MTC_ALGLINK_TEMP_IMG_WIDTH_MIN             (4)
#define MTC_ALGLINK_TEMP_IMG_HEIGHT_MIN            (4)
#define MTC_ALGLINK_OUT_SCORE_PITCH                (128)
#define MTC_ALGLINK_OUT_SCORE_HEIGHT               (128)
#define MTC_ALGLINK_OUT_BUF_SIZE                   (MTC_ALGLINK_OUT_SCORE_PITCH * \
                                                    MTC_ALGLINK_OUT_SCORE_HEIGHT * \
                                                    sizeof(float))

/**
 *******************************************************************************
 *
 *   \brief Max number of buffers
 *
 *   MTC generates flow vectors for the objects in the frame. Flow vectors
 *   give the direction of motion of the object. The MTC algorithm generates
 *   flow vectors for a group of pixels.
 *
 *   SUPPORTED in ALL platforms
 *
 *******************************************************************************
 */
#define MATCHTEMPLATECLASSIC_LINK_MAX_NUM_OUTPUT         (8)

/**
 *******************************************************************************
 *
 *   \brief Max number of channels
 *
 *******************************************************************************
 */
#define MATCHTEMPLATECLASSIC_LINK_MAX_CH                 (5)

/*******************************************************************************
 *  Data structures
 *******************************************************************************
 */

typedef struct
{
    Void                  *algMtcHandle;
    MTC_CreateParams       algMtcCreateParams;

    MTC_InArgs             algMtcInArgs;
    MTC_OutArgs            algMtcOutArgs;

    IVISION_InBufs        algMtcInBufs;
    IVISION_OutBufs       algMtcOutBufs;

    IVISION_BufDesc       algMtcInBufDesc;
    IVISION_BufDesc       *algMtcInBufDescList[MTC_MAX_ALG_IN_BUFS];

    IVISION_BufDesc       algMtcOutBufDesc;
    IVISION_BufDesc       *algMtcOutBufDescList[MTC_MAX_ALG_OUT_BUFS];

    Void                  *pAlgMtcOutBuf;
    UInt32                algMtcOutBufSize;

    System_Buffer         buffers[MATCHTEMPLATECLASSIC_LINK_MAX_NUM_OUTPUT];
    /**< System buffers to exchange data with next link */

    System_MetaDataBuffer metaDataBuffers[MATCHTEMPLATECLASSIC_LINK_MAX_NUM_OUTPUT];
    /**< Payload for the system buffers */

} AlgorithmLink_MatchTemplateClassicChObj;


/**
 *******************************************************************************
 *
 *   \brief Structure containing Match Template Classic algorithm link
 *          parameters
 *
 *          This structure holds any algorithm parameters specific to this link.
 *
 *******************************************************************************
*/
typedef struct
{
    AlgorithmLink_MatchTemplateClassicCreateParams algLinkCreateParams;

    AlgorithmLink_MatchTemplateClassicChObj        chObj[MATCHTEMPLATECLASSIC_LINK_MAX_CH];

    AlgorithmLink_InputQueueInfo            inputQInfo;
    /**< All the information about input Queue*/

    AlgorithmLink_OutputQueueInfo           outputQInfo;
    /**< All the information about output Queue*/

    System_LinkStatistics                   *linkStatsInfo;
    /**< Pointer to the Link statistics information,
         used to store below information
            1, min, max and average latency of the link
            2, min, max and average latency from source to this link
            3, links statistics like frames captured, dropped etc
        Pointer is assigned at the link create time from shared
        memory maintained by utils_link_stats layer */

    Bool                                    isFirstFrameRecv;
    /**< Flag to indicate if first frame is received, this is used as trigger
     *   to start stats counting
     */

} AlgorithmLink_MatchTemplateClassicObj;

/*******************************************************************************
 *  Algorithm Link Private Functions
 *******************************************************************************
 */
Int32 AlgorithmLink_matchTemplateClassicCreate(void *pObj, void *pCreateParams);
Int32 AlgorithmLink_matchTemplateClassicProcess(void *pObj);
Int32 AlgorithmLink_matchTemplateClassicControl(void *pObj, void *pControlParams);
Int32 AlgorithmLink_matchTemplateClassicStop(void *pObj);
Int32 AlgorithmLink_matchTemplateClassicDelete(void *pObj);
Int32 AlgorithmLink_matchTemplateClassicPrintStatistics(void *pObj,
                AlgorithmLink_MatchTemplateClassicObj *pMatchTemplateClassicObj);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/*@}*/

/* Nothing beyond this point */
