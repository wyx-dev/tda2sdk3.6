/******************************************************************************
*
* Copyright (C) 2019-2020 Momenta Incorporated - https://www.momenta.ai/
*
* ALL RIGHTS RESERVED
*
******************************************************************************/

/**
----------------------------------------------------------------------------
*  @file    warp_affine_algo.h
*  @brief     This file defines the ivision interface for
*             Warp Affine Algorithm.
*  @version 0.1 (Feb 2019) : Initial version
----------------------------------------------------------------------------
*/

#ifndef WARP_AFFINE_ALGO_H
#define WARP_AFFINE_ALGO_H

#include <ivision.h>
#include <stdint.h>

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */

/**
*  @brief The unique const funtion table for the Warp Affine Algorithm.
*/
extern const IVISION_Fxns WA_MOMENTA_VISION_FXNS;

/**
*  @struct WA_CreateParams
*  @brief  This structure contains all the parameters which controls
*          MOMENTA Warp Affine Algorithm at create time
*
*  @param  visionParams
*          Common parmeters for all ivison based modules
*  @param  sample
*          add your algorithm related parameters
*/
typedef struct
{
  IVISION_Params visionParams;
  int32_t sample;
} WA_CreateParams;

/**
*  @struct WA_InArgs
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
} WA_InArgs;

/**
*  @struct WA_OutArgs
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
} WA_OutArgs;

#endif /*WARP_AFFINE_ALGO_H */
/*==========================================================================*/
/*     END of the FILE                                                      */
/*==========================================================================*/
