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
*  @file    sparse_optical_flow_algo.c
*  @brief     This file defines the XDAIS/iVISION functions for
*             Sparse Optical Flow Algorithm.
*  @version 0.1 (Nov 2018) : Initial version
----------------------------------------------------------------------------
*/

#include "sparse_optical_flow_algo_int.h"
//#include "match_template_algo.h"

Int32 Vps_printf(const char *format, ...);

/**
* IVISION interface of the algorithm
*/
const IVISION_Fxns SOF_MOMENTA_VISION_FXNS =
{
  {
    &sparse_optical_flow_algo_activate,    /* IALG_Fxns.implementationId */
    &sparse_optical_flow_algo_activate,    /* IALG_Fxns.algActivate */
    &sparse_optical_flow_algo_alloc,       /* IALG_Fxns.algAlloc */
    NULL,                                  /* IALG_Fxns.algControl */
    &sparse_optical_flow_algo_deactivate,  /* IALG_Fxns.algDeactivate */
    &sparse_optical_flow_algo_free,        /* IALG_Fxns.algFree */
    &sparse_optical_flow_algo_init,        /* IALG_Fxns.algInit */
    NULL,                                  /* IALG_Fxns.algMoved */
    &sparse_optical_flow_algo_numAlloc     /* IALG_Fxns.algNumAlloc */
  },
  &sparse_optical_flow_algo_process,       /* IVISION_Fxns.algProcess */
  &sparse_optical_flow_algo_control        /* IVISION_Fxns.algControl */
};

/**
----------------------------------------------------------------------------
*  @ingroup    SOF_MOMENTA_VISION_FXNS
*  @fn         sparse_optical_flow_algo_numAlloc
*  @brief      Function to return the number of memory table needed by algorithm
*  @remarks    None
*  @return     Number of memory table
----------------------------------------------------------------------------
*/
XDAS_Int32 sparse_optical_flow_algo_numAlloc(void)
{
  return MAX_NUM_MEMRECS;
}

/**
----------------------------------------------------------------------------
*  @ingroup    SOF_MOMENTA_VISION_FXNS
*  @fn         sparse_optical_flow_algo_alloc
*  @brief      Function to supply memory requirement to user
*              refer algAlloc function for details in XADIS (ialg.h file)
*  @param      params       : Input Parameters
*  @param      parentFxns   : Parent function pointer
*  @param      memRec       : memory table populated by the Algorithm
*  @remarks    None
*  @return     IALG_EOK   - Successful
*              IALG_EFAIL - Unspecified error
----------------------------------------------------------------------------
*/
XDAS_Int32 sparse_optical_flow_algo_alloc(const IALG_Params *params,
                             IALG_Fxns **parentFxns,
                             IALG_MemRec memRec[])
{
  int32_t status = IALG_EOK;
//  const SOF_CreateParams *createParams =
//    (const SOF_CreateParams*)(const void*)params;

  memRec[ALG_HANDLE_MEMREC].size              = sizeof(SOF_Obj);
  memRec[ALG_HANDLE_MEMREC].space             = IALG_EXTERNAL;
  memRec[ALG_HANDLE_MEMREC].attrs             = IALG_PERSIST;
  memRec[ALG_HANDLE_MEMREC].alignment         = 128;

  memRec[ALG_HANDLE_INT_MEMREC].size          = ALIGN_SIZE(sizeof(SOF_Obj), 8);
  memRec[ALG_HANDLE_INT_MEMREC].space         = IALG_DARAM1;
  memRec[ALG_HANDLE_INT_MEMREC].attrs         = IALG_SCRATCH;
  memRec[ALG_HANDLE_INT_MEMREC].alignment     = 128;

  memRec[ALG_HANDLE_TBKRECT].size          = ALIGN_SIZE(sizeof(TBKRect), 8);
  memRec[ALG_HANDLE_TBKRECT].space         = IALG_DARAM1;
  memRec[ALG_HANDLE_TBKRECT].attrs         = IALG_SCRATCH;
  memRec[ALG_HANDLE_TBKRECT].alignment     = 128;

  memRec[ALG_HANDLE_TBKTRACKER].size          = ALIGN_SIZE(sizeof(TBKTracker), 8);
  memRec[ALG_HANDLE_TBKTRACKER].space         = IALG_DARAM1;
  memRec[ALG_HANDLE_TBKTRACKER].attrs         = IALG_SCRATCH;
  memRec[ALG_HANDLE_TBKTRACKER].alignment     = 128;

  memRec[ALG_HANDLE_TBKTRACKER_PREROIFRAME].size          = ALIGN_SIZE(sizeof(uint8_t)*360000, 8);
  memRec[ALG_HANDLE_TBKTRACKER_PREROIFRAME].space         = IALG_EXTERNAL;
  memRec[ALG_HANDLE_TBKTRACKER_PREROIFRAME].attrs         = IALG_SCRATCH;
  memRec[ALG_HANDLE_TBKTRACKER_PREROIFRAME].alignment     = 128;

  memRec[ALG_HANDLE_TBKTRACKER_POINTS].size          = ALIGN_SIZE(sizeof(float)*100*2, 8);
  memRec[ALG_HANDLE_TBKTRACKER_POINTS].space         = IALG_EXTERNAL;
  memRec[ALG_HANDLE_TBKTRACKER_POINTS].attrs         = IALG_SCRATCH;
  memRec[ALG_HANDLE_TBKTRACKER_POINTS].alignment     = 128;

  return (status);
}

/**
----------------------------------------------------------------------------
*  @ingroup    SOF_MOMENTA_VISION_FXNS
*  @fn         sparse_optical_flow_algo_init
*  @brief      Function to accept memory pointers from user and initialize algo
*              Refer algInit function for details in XADIS (ialg.h file)
*  @param      handle   : Algorithm Instance handle
*  @param      memRec   : memory table populated by the Algorithm
*  @param      parent   : Parent function pointer
*  @param      params   : Input Parameters
*  @remarks    None
*  @return     IALG_EOK   - Successful
*              IALG_EFAIL - Unspecified error
----------------------------------------------------------------------------
*/
XDAS_Int32 sparse_optical_flow_algo_init(IALG_Handle handle,
                            const IALG_MemRec memRec[],
                            IALG_Handle parent,
                            const IALG_Params *params)
{
  int32_t status = IALG_EOK;
  const SOF_CreateParams *createParams =
    (const SOF_CreateParams*)(const void*)params;

  SOF_Handle algHandle =
    (SOF_Handle)(void*)memRec[ALG_HANDLE_MEMREC].base;

  //init TBKRect
  //TBKRect *tbkRect =
    //(TBKRect*)(void*)memRec[ALG_HANDLE_TBKRECT].base;
  //tbkRect->left = 1.1f;
  //tbkRect->right = 1.1f;
  //tbkRect->top = 1.1f;
  //tbkRect->bottom = 1.1f;

  //init TBKTrracker
  TBKTracker *tbkTracker =
    (TBKTracker*)(void*)memRec[ALG_HANDLE_TBKTRACKER].base;
  tbkTracker->rect.left = 0.0f;
  tbkTracker->rect.right = 0.0f;
  tbkTracker->rect.top = 0.0f;
  tbkTracker->rect.bottom = 0.0f;
  tbkTracker->pre_width = 0;
  tbkTracker->pre_height = 0;
  tbkTracker->track_times = 0;
  tbkTracker->init = 0;
  tbkTracker->track_on = 0;
  tbkTracker->points_num = 1;
  tbkTracker->roi_store_size = 360000;
  tbkTracker->pre_resized_roi_frame = (uint8_t*)(void*)memRec[ALG_HANDLE_TBKTRACKER_PREROIFRAME].base;
  
  tbkTracker->points = (float*)(void*)memRec[ALG_HANDLE_TBKTRACKER_POINTS].base;

  algHandle->createParams = *createParams;

  algHandle->ivision = &SOF_MOMENTA_VISION_FXNS;

  algHandle->numMemRecs = MAX_NUM_MEMRECS;
  memcpy(algHandle->memRec, memRec, sizeof(IALG_MemRec)*algHandle->numMemRecs);

  /*-----------------------------------------------------------------------*/
  /* State to indicate that internal context of algorithm is not active    */
  /*-----------------------------------------------------------------------*/
  algHandle->algState = ALG_NOT_ACTIVE;

  return (status);
}

/**
----------------------------------------------------------------------------
*  @ingroup    SOF_MOMENTA_VISION_FXNS
*  @fn         sparse_optical_flow_algo_control
*  @brief      Function to accept any run time updates with recreation of object
*              Refer algControl function for details in ivision.h file
*  @param      handle   : Algorithm Instance handle
*  @param      cmd      : Command to be processed
*  @param      inParams : Input Parameters
*  @param      outParams: Output Parameters
*  @remarks    None
*  @return     IALG_EOK   - Successful
*              IALG_EFAIL - Unspecified error
----------------------------------------------------------------------------
*/
XDAS_Int32 sparse_optical_flow_algo_control(IVISION_Handle Handle,
                               IALG_Cmd cmd,
                               const IALG_Params *inParams,
                               IALG_Params *outParams)
{
  int32_t status = IALG_EOK;

  return (status);
}

/**
----------------------------------------------------------------------------
*  @ingroup    SOF_MOMENTA_VISION_FXNS
*  @fn         sparse_optical_flow_algo_process
*  @brief      This is the main processing function and should be called
*              for each frame.
*              Refer process function for details in ivision.h file
*
*  @param      handle : Algorithm Instance handle
*  @param      inBufs : Input Buffer Pointers descriptor
*  @param      outBufs: Output Buffer Pointers descriptor
*  @param      inArgs : Input Arguments
*  @param      outArgs: Output Arguments
*  @remarks    None
*  @return     IALG_EOK   - Successful
*              IALG_EFAIL - Unspecified error
----------------------------------------------------------------------------
*/
XDAS_Int32 sparse_optical_flow_algo_process(IVISION_Handle Handle,
                               IVISION_InBufs *inBufs,
                               IVISION_OutBufs *outBufs,
                               IVISION_InArgs *inArgs,
                               IVISION_OutArgs *outArgs)
{
  SOF_Handle algHandle = (SOF_Handle)(void*)(Handle);
  int32_t status = IALG_EOK;
  SOF_InArgs *algoInArgs = (SOF_InArgs*)(void*)inArgs;
  SOF_outArgs *algoOutArgs = (SOF_outArgs*)(void*)outArgs;
  float sof[4] = {1.1f, 2.2f, 3.3f, 4.4f};

  algHandle = algHandle;
  algoInArgs = algoInArgs;
  algoOutArgs = algoOutArgs;

  /*-----------------------------------------------------------------------*/
  /* Algorithm main processing                                             */
  /*-----------------------------------------------------------------------*/
  uint8_t* input_info = (uint8_t *)(void *)inBufs->bufDesc[0]->bufPlanes[0].buf;
  uint32_t frame_height = (uint32_t)(void *)inBufs->bufDesc[0]->bufPlanes[0].frameROI.height;
  printf("frame height is %d\n", frame_height);
  uint32_t frame_width = (uint32_t)(void *)inBufs->bufDesc[0]->bufPlanes[0].frameROI.width;
  printf("frame width is %d\n", frame_width);

  //Cache_inv(input_info, frame_width * frame_height, Cache_Type_ALL, TRUE);

  uint8_t* info_ptr = input_info;
  uint32_t ptr_y = 0;
  uint32_t ptr_x = 0;
  for(ptr_y = 0; ptr_y < frame_height; ptr_y++){
    for(ptr_x = 0; ptr_x < frame_width; ptr_x++){
      info_ptr++;
    }
  }

  //Cache_inv(info_ptr, sizeof(float) * 5, Cache_Type_ALL, TRUE);

  for(ptr_x = 0; ptr_x <3; ptr_x++)
      info_ptr++;
  uint8_t type = *info_ptr;
  info_ptr++;
  printf("type is : %d\n", type);

  //init input rect
  TBKRect InputRectValue = {0.0f,0.0f,0.0f,0.0f};
  TBKRect* InputRect = &InputRectValue;
  uint32_t* RectInfo = (uint32_t *)info_ptr;
  printf("Input rect is : ");
  InputRect->left = *RectInfo;
  printf("x1 : %f ", InputRect->left);
  RectInfo++;
  InputRect->right = *RectInfo;
  printf("x2 : %f ", InputRect->right);
  RectInfo++;
  InputRect->top = *RectInfo;
  printf("y1 : %f ", InputRect->top);
  RectInfo++;
  InputRect->bottom = *RectInfo;
  printf("y2 : %f\n", InputRect->bottom);
  //init tbk tracker
  //TBKTracker* tracker = (TBKTracker*)(void*)memRec[ALG_HANDLE_TBKTRACKER].base;
  TBKTracker* tracker = (TBKTracker*)(void*)algHandle->memRec[ALG_HANDLE_TBKTRACKER].base;
  tracker = tracker;

  //track_by_klt(tracker, input_info, InputRect, frame_width, frame_height);
  
  //test_warp_function();
  //test_getHmatirx();
  //test_homography();
  //test_calcHmatrix();
  //test_get_gradient(input_info, frame_width, frame_height);
  
  uint8_t res = optical_flow(tracker, input_info, InputRect, frame_width, frame_height, type);

  printf("res : %d\n", res);
  if(res == 0)
     memset(outBufs->bufDesc[2]->bufPlanes[0].buf, 0.0f, sizeof(float) * 4);
  else{
      if(res != 1)
          memset(outBufs->bufDesc[2]->bufPlanes[0].buf, 1.0f, sizeof(float) * 4);
      else
          memcpy(outBufs->bufDesc[2]->bufPlanes[0].buf, &sof, sizeof(float) * 4);
  }

  //match_template_alg_test(64, 72, 32, 48);
  //match_template_alg_test(128, 72, 96, 48);
  //match_template_alg_test(160, 72, 128, 48);
  //match_template_alg_test(256, 72, 224, 48);
  //match_template_alg_test(512, 72, 480, 48);


  return status;
}

/**
----------------------------------------------------------------------------
*  @ingroup    SOF_MOMENTA_VISION_FXNS
*  @fn         sparse_optical_flow_algo_activate
*  @brief      This is the function to do activation of algorithm.
*              All scratch memory  are restored in this
*              Refer algFree function for details in ialg.h file.
*  @param      handle : Algorithm Instance handle
*  @remarks    None
*  @return     None
----------------------------------------------------------------------------
*/
XDAS_Void  sparse_optical_flow_algo_activate(IALG_Handle handle)
{
  SOF_Handle algHandle = (SOF_Handle)(void*)(handle);
  int32_t i;

  /*----------------------------------------------------------------*/
  /* Do Activation only if it is deactivated or just after creation */
  /*----------------------------------------------------------------*/
  if(algHandle->algState == ALG_NOT_ACTIVE)
  {
    for(i = 1U; i < algHandle->numMemRecs; i++)
    {
      if(i == ALG_HANDLE_INT_MEMREC)
      {
        EDMA_UTILS_memcpy2D((void *)      algHandle->memRec[i].base,
                            (const void *)algHandle->memRec[i-1U].base,
                            (uint16_t)    algHandle->memRec[i].size,
                            (uint16_t)1,
                            (int16_t) 0,
                            (int16_t) 0);
      }
    }

    algHandle->algState = ALG_ACTIVE;
  }

  return;
}

/**
----------------------------------------------------------------------------
*  @ingroup    SOF_MOMENTA_VISION_FXNS
*  @fn         sparse_optical_flow_algo_deactivate
*  @brief      This is the function to do de-activation of algorithm.
*              All scratch memory are stoed to external memory in this
*              Refer algFree function for details in ialg.h file.
*  @param      handle : Algorithm Instance handle
*  @remarks    None
*  @return     None
----------------------------------------------------------------------------
*/
XDAS_Void  sparse_optical_flow_algo_deactivate(IALG_Handle handle)
{
  SOF_Handle algHandle = (SOF_Handle)(void*)(handle);
  int32_t i;

  /*----------------------------------------------------------------*/
  /* Do De-activation only if it is activated                       */
  /*----------------------------------------------------------------*/
  if(algHandle->algState == ALG_ACTIVE)
  {
    for(i = 1U; i < algHandle->numMemRecs; i++)
    {
      if(i == ALG_HANDLE_INT_MEMREC)
      {
        EDMA_UTILS_memcpy2D((void *)      algHandle->memRec[i-1U].base,
                            (const void *)algHandle->memRec[i].base,
                            (uint16_t)    algHandle->memRec[i].size,
                            (uint16_t)1,
                            (int16_t) 0,
                            (int16_t) 0);
      }
    }

    algHandle->algState = ALG_NOT_ACTIVE;
  }

  return;
}

/**
----------------------------------------------------------------------------
*  @ingroup    SOF_MOMENTA_VISION_FXNS
*  @fn         sparse_optical_flow_algo_free
*  @brief      This is the function which user can call to get back
*              the memory given to algo.
*              Refer algFree function for details in ialg.h file.
*  @param      handle : Algorithm Instance handle
*  @param      memTab : memory table populated by the Algorithm
*  @remarks    None
*  @return     IALG_EOK   - Successful
*              IALG_EFAIL - Unspecified error
----------------------------------------------------------------------------
*/
XDAS_Int32 sparse_optical_flow_algo_free(IALG_Handle handle, IALG_MemRec *memRec)
{
  int32_t status = IALG_EOK;
  SOF_Handle algHandle = (SOF_Handle)(void*)(handle);

  memcpy(memRec, algHandle->memRec, sizeof(IALG_MemRec)*algHandle->numMemRecs);

  return status;
}
