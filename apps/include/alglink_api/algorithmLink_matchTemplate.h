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
 * \ingroup  ALGORITHM_LINK_PLUGIN
 * \defgroup ALGORITHM_LINK_MATCHTEMPLATE Match Template API
 *
 * \brief  This module has the interface for using MT algorithm
 *
 * @{
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \file algorithmLink_matchTemplate.h
 *
 * \brief Algorithm Link API specific to match template algorithm
 *
 * \version 0.0 (Nov 2018) : [NN] First version
 *
 *******************************************************************************
 */

#ifndef ALGORITHM_LINK_MATCHTEMPLATE_H_
#define ALGORITHM_LINK_MATCHTEMPLATE_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include <include/link_api/system.h>
#include <include/link_api/algorithmLink.h>

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */

/*******************************************************************************
 *  Enum's
 *******************************************************************************
 */
/**
 *******************************************************************************
 *
 *  \brief  Enumerations for MT config.
 *
 *          Different configs need to be supported for different usecases
 *
 *******************************************************************************
*/

/*******************************************************************************
 *  Data structures
 *******************************************************************************
 */


typedef struct
{
    AlgorithmLink_CreateParams baseClassCreate;
    /**< Base class create params. This structure should be first element */
    System_LinkOutQueParams  outQueParams;
    /**< Output queue information */
    System_LinkInQueParams   inQueParams;
    /**< Input queue information */
    UInt32                   numOutBuffers;
    /**< Number of output Buffers */
    UInt32                   imgFrameWidth;
    /**< Width of the input frame */
    UInt32                   imgFrameHeight;
    /**< Height of the input frame */
    UInt32                   imgFramePitch;
    /**< Pitch of the input frame */
    UInt32                   patchWidth;
    /**< Processing region patch width */
    UInt32                   patchHeight;
    /**< Processing region patch height */
    UInt32                   searchWidth;
    /**< Processing region search width */
    UInt32                   searchHeight;
    /**< Processing region search height */

} AlgorithmLink_MatchTemplateCreateParams;

/**
 *******************************************************************************
 *
 *   \brief Structure containing control parameters for MT algorithm
 *
 *******************************************************************************
*/
typedef struct
{
    AlgorithmLink_ControlParams baseClassControl;
    /**< Base class control params */
} AlgorithmLink_MatchTemplateControlParams;

/*******************************************************************************
 *  Functions
 *******************************************************************************
 */
static inline void AlgorithmLink_MatchTemplate_Init(
                        AlgorithmLink_MatchTemplateCreateParams *pPrm);

/**
 *******************************************************************************
 *
 * \brief Set defaults for plugin create parameters
 *
 * \param pPrm  [OUT] plugin create parameters
 *
 *******************************************************************************
 */
static inline void AlgorithmLink_MatchTemplate_Init(
    AlgorithmLink_MatchTemplateCreateParams *pPrm)
{
/* MISRA.ONEDEFRULE.FUNC
 * MISRAC_2004 Rule 8.5
 * Function Definition in header file
 * KW State: Defer -> Waiver -> Case by case
 * MISRAC_WAIVER: This function initializes the create time argument to default.
 *  Defining this in the header file aids maintainability.
 */
    memset(pPrm, 0, sizeof(AlgorithmLink_MatchTemplateCreateParams));

    pPrm->baseClassCreate.size = (UInt32)
                            sizeof(AlgorithmLink_MatchTemplateCreateParams);
    pPrm->baseClassCreate.algId = ALGORITHM_LINK_DSP_ALG_MATCH_TEMPLATE;

    pPrm->inQueParams.prevLinkId = SYSTEM_LINK_ID_INVALID;
    pPrm->inQueParams.prevLinkQueId = 0U;
    pPrm->numOutBuffers = 1U;
    pPrm->outQueParams.nextLink = SYSTEM_LINK_ID_INVALID;

    pPrm->imgFrameWidth  = 1280U;
    pPrm->imgFrameHeight = 720U;
    pPrm->imgFramePitch  = 1280U;

    pPrm->patchWidth     = 128U;
    pPrm->patchHeight    = 48U;
    pPrm->searchWidth    = 6U;
    pPrm->searchHeight   = 12U;
}


/**
 *******************************************************************************
 *
 * \brief Implementation of function to init plugins()
 *
 *        This function will be called by AlgorithmLink_initAlgPlugins, so as
 *        register plugins of Match Template algorithm
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 AlgorithmLink_MatchTemplate_initPlugin(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/* @} */

/* Nothing beyond this point */
