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
 * \ingroup  ALGORITHM_LINK_PLUGIN
 * \defgroup ALGORITHM_LINK_MATCHTEMPLATECLASSIC Match Template Classic API
 *
 * \brief  This module has the interface for using MTC algorithm
 *
 * @{
 *
 *******************************************************************************
 */

/**
 *******************************************************************************
 *
 * \file algorithmLink_matchTemplateClassic.h
 *
 * \brief Algorithm Link API specific to match template classic algorithm
 *
 * \version 0.0 (Apr 2019) : [NN] First version
 *
 *******************************************************************************
 */

#ifndef ALGORITHM_LINK_MATCHTEMPLATECLASSIC_H_
#define ALGORITHM_LINK_MATCHTEMPLATECLASSIC_H_

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
 *  \brief  Enumerations for MTC config.
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

} AlgorithmLink_MatchTemplateClassicCreateParams;

/**
 *******************************************************************************
 *
 *   \brief Structure containing control parameters for MTC algorithm
 *
 *******************************************************************************
*/
typedef struct
{
    AlgorithmLink_ControlParams baseClassControl;
    /**< Base class control params */
} AlgorithmLink_MatchTemplateClassicControlParams;

/*******************************************************************************
 *  Functions
 *******************************************************************************
 */
static inline void AlgorithmLink_MatchTemplateClassic_Init(
                        AlgorithmLink_MatchTemplateClassicCreateParams *pPrm);

/**
 *******************************************************************************
 *
 * \brief Set defaults for plugin create parameters
 *
 * \param pPrm  [OUT] plugin create parameters
 *
 *******************************************************************************
 */
static inline void AlgorithmLink_MatchTemplateClassic_Init(
    AlgorithmLink_MatchTemplateClassicCreateParams *pPrm)
{
/* MISRA.ONEDEFRULE.FUNC
 * MISRAC_2004 Rule 8.5
 * Function Definition in header file
 * KW State: Defer -> Waiver -> Case by case
 * MISRAC_WAIVER: This function initializes the create time argument to default.
 *  Defining this in the header file aids maintainability.
 */
    memset(pPrm, 0, sizeof(AlgorithmLink_MatchTemplateClassicCreateParams));

    pPrm->baseClassCreate.size = (UInt32)
                            sizeof(AlgorithmLink_MatchTemplateClassicCreateParams);
    pPrm->baseClassCreate.algId = ALGORITHM_LINK_DSP_ALG_MATCH_TEMPLATE_CLASSIC;

    pPrm->inQueParams.prevLinkId = SYSTEM_LINK_ID_INVALID;
    pPrm->inQueParams.prevLinkQueId = 0U;
    pPrm->numOutBuffers = 1U;
    pPrm->outQueParams.nextLink = SYSTEM_LINK_ID_INVALID;
}


/**
 *******************************************************************************
 *
 * \brief Implementation of function to init plugins()
 *
 *        This function will be called by AlgorithmLink_initAlgPlugins, so as
 *        register plugins of Match Template Classic algorithm
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 AlgorithmLink_MatchTemplateClassic_initPlugin(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/* @} */

/* Nothing beyond this point */
