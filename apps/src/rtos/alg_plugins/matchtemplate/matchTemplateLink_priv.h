/******************************************************************************
*
* Copyright (C) 2018-2019 Momenta Incorporated - https://www.momenta.ai/
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
 * \file matchTemplateLink_priv.h Match Template Link private API/Data
 *       structures
 *
 * \brief  This link private header file has defined
 *         - Algorithm link instance/handle object
 *         - All the local data structures
 *         - Algorithm plug in function interfaces
 *
 * \version 0.0 (Nov 2018) : [NN] First version
 *
 *******************************************************************************
 */

#ifndef _MATCHTEMPLATE_LINK_PRIV_H_
#define _MATCHTEMPLATE_LINK_PRIV_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  Include files
 *******************************************************************************
 */
#include <include/link_api/system.h>
#include <include/link_api/algorithmLink.h>
#include <include/alglink_api/algorithmLink_matchTemplate.h>
#include <include/link_api/algorithmLink_algPluginSupport.h>
#include <src/rtos/utils_common/include/utils_prf.h>
#include <edma_utils/edma_utils_memcpy.h>
#include <src/rtos/alg_plugins/common/include/alg_ivision.h>
#include <src/rtos/utils_common/include/utils_link_stats_if.h>
#include "match_template_algo.h"

/*******************************************************************************
 *  Enums
 *******************************************************************************
 */


/*******************************************************************************
 *  Defines
 *******************************************************************************
 */
#define MT_MAX_ALG_IN_BUFS                        (1)
#define MT_MAX_ALG_OUT_BUFS                       (1)

#define MT_ALGLINK_OUT_BUF_SIZE                   (4 * sizeof(UInt32))

/**
 *******************************************************************************
 *
 *   \brief Max number of buffers
 *
 *   MT generates flow vectors for the objects in the frame. Flow vectors
 *   give the direction of motion of the object. The MT algorithm generates
 *   flow vectors for a group of pixels.
 *
 *   SUPPORTED in ALL platforms
 *
 *******************************************************************************
 */
#define MATCHTEMPLATE_LINK_MAX_NUM_OUTPUT         (8)

/**
 *******************************************************************************
 *
 *   \brief Max number of channels
 *
 *******************************************************************************
 */
#define MATCHTEMPLATE_LINK_MAX_CH                 (5)

/*******************************************************************************
 *  Data structures
 *******************************************************************************
 */

typedef struct
{
    Void                  *algMtHandle;
    MT_CreateParams       algMtCreateParams;

    MT_InArgs             algMtInArgs;
    MT_OutArgs            algMtOutArgs;

    IVISION_InBufs        algMtInBufs;
    IVISION_OutBufs       algMtOutBufs;

    IVISION_BufDesc       algMtInBufDesc;
    IVISION_BufDesc       *algMtInBufDescList[MT_MAX_ALG_IN_BUFS];

    IVISION_BufDesc       algMtOutBufDesc;
    IVISION_BufDesc       *algMtOutBufDescList[MT_MAX_ALG_OUT_BUFS];

    Void                  *pAlgMtOutBuf;
    UInt32                algMtOutBufSize;

    System_Buffer         buffers[MATCHTEMPLATE_LINK_MAX_NUM_OUTPUT];
    /**< System buffers to exchange data with next link */

    System_MetaDataBuffer metaDataBuffers[MATCHTEMPLATE_LINK_MAX_NUM_OUTPUT];
    /**< Payload for the system buffers */

} AlgorithmLink_MatchTemplateChObj;


/**
 *******************************************************************************
 *
 *   \brief Structure containing Match Template algorithm link
 *          parameters
 *
 *          This structure holds any algorithm parameters specific to this link.
 *
 *******************************************************************************
*/
typedef struct
{
    AlgorithmLink_MatchTemplateCreateParams algLinkCreateParams;

    AlgorithmLink_MatchTemplateChObj        chObj[MATCHTEMPLATE_LINK_MAX_CH];

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

} AlgorithmLink_MatchTemplateObj;

/*******************************************************************************
 *  Algorithm Link Private Functions
 *******************************************************************************
 */
Int32 AlgorithmLink_matchTemplateCreate(void *pObj, void *pCreateParams);
Int32 AlgorithmLink_matchTemplateProcess(void *pObj);
Int32 AlgorithmLink_matchTemplateControl(void *pObj, void *pControlParams);
Int32 AlgorithmLink_matchTemplateStop(void *pObj);
Int32 AlgorithmLink_matchTemplateDelete(void *pObj);
Int32 AlgorithmLink_matchTemplatePrintStatistics(void *pObj,
                AlgorithmLink_MatchTemplateObj *pMatchTemplateObj);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/*@}*/

/* Nothing beyond this point */
