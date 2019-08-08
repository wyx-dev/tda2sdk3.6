/******************************************************************************
*
* Copyright (C) 2019-2020 Momenta Incorporated - https://www.momenta.ai/
*
* ALL RIGHTS RESERVED
*
******************************************************************************/

/**
----------------------------------------------------------------------------
*  @file    match_template_classic_algo.h
*  @brief     This file defines the ivision interface for
*             Match Template Classic Algorithm.
*  @version 0.1 (Apr 2019) : Initial version
----------------------------------------------------------------------------
*/

#ifndef MATCH_TEMPLATE_CLASSIC_ALGO_H
#define MATCH_TEMPLATE_CLASSIC_ALGO_H

#include <ivision.h>
#include <stdint.h>

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */

/**
*  @brief The unique const funtion table for the Match Template Classic Algorithm.
*/
extern const IVISION_Fxns MTC_MOMENTA_VISION_FXNS;

/**
*  @struct MTC_CreateParams
*  @brief  This structure contains all the parameters which controls
*          MOMENTA Match Template Classic Algorithm at create time
*
*  @param  visionParams
*          Common parmeters for all ivison based modules
*/
typedef struct
{
  IVISION_Params visionParams;
} MTC_CreateParams;

/**
*  @struct MTC_InArgs
*  @brief  This structure contains all the parameters which controls
*          the applet at create time
*  @param  iVisionInArgs
*          Common inArgs for all ivison based modules
*  @param  sample
*          add your algorithm related parameters
*/
typedef struct
{
  IVISION_InArgs iVisionInArgs;
  int32_t sample;
} MTC_InArgs;

/**
*  @struct MTC_OutArgs
*  @brief  This structure contains all the parameters which controls
*          the applet at create time
*  @param  iVisionOutArgs
*          Common outArgs for all ivison based modules
*  @param  sample
*          add your algorithm related parameters
*/
typedef struct
{
  IVISION_OutArgs iVisionOutArgs;
  int32_t sample;
} MTC_OutArgs;

#endif /*MATCH_TEMPLATE_CLASSIC_ALGO_H */
/*==========================================================================*/
/*     END of the FILE                                                      */
/*==========================================================================*/
