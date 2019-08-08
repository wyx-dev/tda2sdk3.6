/******************************************************************************
*
* Copyright (C) 2018-2019 Momenta Incorporated - https://www.momenta.ai/
*
* ALL RIGHTS RESERVED
*
******************************************************************************/

/**
----------------------------------------------------------------------------
*  @file    match_template_algo.h
*  @brief     This file defines the ivision interface for
*             Match Template Algorithm.
*  @version 0.1 (Nov 2018) : Initial version
----------------------------------------------------------------------------
*/

#ifndef MATCH_TEMPLATE_ALGO_H
#define MATCH_TEMPLATE_ALGO_H

#include <ivision.h>
#include <stdint.h>

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */

/**
*  @brief The unique const funtion table for the Match Template Algorithm.
*/
extern const IVISION_Fxns MT_MOMENTA_VISION_FXNS;

/**
*  @struct MT_CreateParams
*  @brief  This structure contains all the parameters which controls
*          MOMENTA Match Template Algorithm at create time
*
*  @param  visionParams
*          Common parmeters for all ivison based modules
*
*  @param  imgWidth
*          Width in bytes for the input image
*
*  @param  imgHeight
*          Height in bytes for the input image
*
*  @param  imgPitch
*          Pitch in bytes for the input image
*
*  @param  patchWidth
*          Processing region patch width
*
*  @param  patchHeight
*          Processing region patch height
*
*  @param  searchWidth
*          Processing region search width
*
*  @param  searchHeight
*          Processing region search height
*/
typedef struct
{
  IVISION_Params visionParams;

  uint32_t imgWidth;
  uint32_t imgHeight;
  uint32_t imgPitch;

  uint32_t patchWidth;
  uint32_t patchHeight;
  uint32_t searchWidth;
  uint32_t searchHeight;
} MT_CreateParams;

/**
*  @struct MT_InArgs
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
} MT_InArgs;

/**
*  @struct MT_OutArgs
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
} MT_OutArgs;

#endif /*MATCH_TEMPLATE_ALGO_H */
/*==========================================================================*/
/*     END of the FILE                                                      */
/*==========================================================================*/
