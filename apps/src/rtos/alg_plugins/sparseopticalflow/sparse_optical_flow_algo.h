/*
* module name       :Sparse Optical Flow Algorithm
*
* module descripton :Sparse Optical Flow Algorithm on DSP
*
* Copyright (C) 2018-2019 Momenta Incorporated - https://www.momenta.ai/
*
* ALL RIGHTS RESERVED
*
*
*/

/**
----------------------------------------------------------------------------
*  @file    sparse_optical_flow_algo.h
*  @brief     This file defines the ivision interface for
*             Sparse Optical Flow Algorithm.
*  @version 0.1 (Nov 2018) : Initial version
----------------------------------------------------------------------------
*/

#ifndef SPARSE_OPTICAL_FLOW_ALGO_H
#define SPARSE_OPTICAL_FLOW_ALGO_H

#include <ivision.h>
#include <stdint.h>

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */
#define PYRAMID_LK_TRACKER_TI_MAXLEVELS (8)

/**
*  @brief The unique const funtion table for the Sparse Optical Flow Algorithm.
*/
extern const IVISION_Fxns SOF_MOMENTA_VISION_FXNS;

/**
*  @struct SOF_CreateParams
*  @brief  This structure contains all the parameters which controls
*          MOMENTA Sparse Optical Flow Algorithm at create time
*
*  @param  visionParams
*          Common parmeters for all ivison based modules
*
*  @param  imWidth
*          Width in bytes for the input image
*
*  @param  imHeight
*          Height in bytes for the input image
*
*  @param  roiWidth
*          Processing region width
*
*  @param  roiHeight
*          Processing region height
*
*  @param  startX
*          X co-ordinate of the first (top-left) pixel in the image. Useful for a ROI processing
*          in a bigger image. Though the information about ROI is supplied via iVISION buf
*          desc but create time also this information is required as this algorithm use the XY
*          value as some create time initialization. It is for base level only.
*
*  @param  startY
*          Y co-ordinate of the first (top-left) pixel in the image. Useful for a ROI processing
*          in a bigger image. Though the information about ROI is supplied via iVISION buf
*          desc but create time also this information is required as this algorithm use the XY
*          value as some create time initialization. It is for base level only
*
*  @param  numLevels
*          Total number of pyramid levels including base level (1 means only base resolution)
*
*  @param  keyPointDetectMethod
*          The key points in the image can be detetcted can be selected using this parameter
*
*  @param  keyPointDetectInterval
*          key point detection interval in number of frames.
*          Setting to zero detects the key points in each frame
*
*  @param  maxNumKeyPoints
*          Maximum number of key points that are tracked by LK tracker
*
*  @param  maxPrevTrackPoints
*          Maximum number of old tracks to keep for the next frame.
*
*  @param  fast9Threshold
*          Threshold on difference between intensity of the central pixel and pixels
*          of a circle around this pixel for FAST9 corner detect applet.
*
*  @param  scoreMethod
*          Fast9 best feature to front applet supports more than one
*          score calculation method. This paramer is used to select the score computation method
*
*  @param  harrisScaling
*          Scale foactor used in Harris score computation
*
*  @param  nmsThreshold
*          NMS threshold for harris corner detection applet
*
*  @param harrisScoreMethod
*         Method to use for Harris Score calculation. Refer to
*         HARRIS_CORNER_DETECTION_32_TI_HarrisScoreMethod for valid values defined in
*         "iHarrisCornerDetection32_ti.h" file
*
*  @param harrisWindowSize
*          Window size to be used for harris score calculation.  Considers a harrisWindowSize x harrisWindowSize
*          neighborhood to calculate Harris Score. Kindly refer to HARRIS_CORNER_DETECTION_32_TI_HarrisWindowSize
*          for valid values in "iHarrisCornerDetection32_ti.h" file
*
*   @param suppressionMethod
*          Suppression method to be used for non maximum suppression. Kindly refer to
*          HARRIS_CORNER_DETECTION_32_TI_SuppressionMethod for valid values in
*         "iHarrisCornerDetection32_ti.h" file
*
*  @param trackOffsetMethod
*          The method to be used for the output of track coordinates. Kindly refer to
*          SOF_TrackOffsetMethod enum for valid values
*
*  @param  maxItersLK
*          Maximum number of iteration for each level
*
*  @param  minErrValue
*          Minimum flow vector difference value that needs to be
*          considered as no motion and exit the iteration for
*          each level
*
*  @param  searchRange
*         Search range in pixel for each level
*/
typedef struct
{
  IVISION_Params visionParams;

  uint16_t imWidth;
  uint16_t imHeight;
  uint16_t roiWidth;
  uint16_t roiHeight;
  uint16_t startX;
  uint16_t startY;
  uint8_t  numLevels;
  uint8_t  keyPointDetectMethod;
  uint8_t  keyPointDetectInterval;
  uint16_t maxNumKeyPoints;
  uint16_t maxPrevTrackPoints;
  uint8_t  fast9Threshold;
  uint8_t  scoreMethod;
  uint16_t harrisScaling;
  int32_t  nmsThreshold;
  uint8_t  harrisScoreMethod;
  uint8_t  harrisWindowSize;
  uint8_t  suppressionMethod;
  uint8_t  trackOffsetMethod;
  uint16_t maxItersLK[PYRAMID_LK_TRACKER_TI_MAXLEVELS];
  uint16_t minErrValue[PYRAMID_LK_TRACKER_TI_MAXLEVELS];
  uint8_t  searchRange[PYRAMID_LK_TRACKER_TI_MAXLEVELS];
} SOF_CreateParams;

/**
*  @struct SOF_InArgs
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
} SOF_InArgs;

/**
*  @struct SOF_outArgs
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
} SOF_outArgs;

#endif /*SPARSE_OPTICAL_FLOW_ALGO_H */
/*==========================================================================*/
/*     END of the FILE                                                      */
/*==========================================================================*/
