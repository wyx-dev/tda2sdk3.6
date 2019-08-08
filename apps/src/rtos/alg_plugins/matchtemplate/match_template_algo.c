/******************************************************************************
*
* Copyright (C) 2018-2019 Momenta Incorporated - https://www.momenta.ai/
*
* ALL RIGHTS RESERVED
*
******************************************************************************/

/**
----------------------------------------------------------------------------
*  @file    match_template_algo.c
*  @brief     This file defines the XDAIS/iVISION functions for
*             Match Template Algorithm.
*  @version 0.1 (Nov 2018) : Initial version
----------------------------------------------------------------------------
*/

#include "match_template_algo_int.h"
#include "match_template_dsp.h"

Int32 Vps_printf(const char *format, ...);

/**
* IVISION interface of the algorithm
*/
const IVISION_Fxns MT_MOMENTA_VISION_FXNS =
{
  {
    &match_template_algo_activate,    /* IALG_Fxns.implementationId */
    &match_template_algo_activate,    /* IALG_Fxns.algActivate */
    &match_template_algo_alloc,       /* IALG_Fxns.algAlloc */
    NULL,                             /* IALG_Fxns.algControl */
    &match_template_algo_deactivate,  /* IALG_Fxns.algDeactivate */
    &match_template_algo_free,        /* IALG_Fxns.algFree */
    &match_template_algo_init,        /* IALG_Fxns.algInit */
    NULL,                             /* IALG_Fxns.algMoved */
    &match_template_algo_numAlloc     /* IALG_Fxns.algNumAlloc */
  },
  &match_template_algo_process,       /* IVISION_Fxns.algProcess */
  &match_template_algo_control        /* IVISION_Fxns.algControl */
};

/**
----------------------------------------------------------------------------
*  @ingroup    MT_MOMENTA_VISION_FXNS
*  @fn         match_template_algo_numAlloc
*  @brief      Function to return the number of memory table needed by algorithm
*  @remarks    None
*  @return     Number of memory table
----------------------------------------------------------------------------
*/
XDAS_Int32 match_template_algo_numAlloc(void)
{
  return MAX_NUM_MEMRECS;
}

/**
----------------------------------------------------------------------------
*  @ingroup    MT_MOMENTA_VISION_FXNS
*  @fn         match_template_algo_alloc
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
XDAS_Int32 match_template_algo_alloc(const IALG_Params *params,
                             IALG_Fxns **parentFxns,
                             IALG_MemRec memRec[])
{
  int32_t status = IALG_EOK;

  memRec[ALG_HANDLE_MEMREC].size              = sizeof(MT_Obj);
  memRec[ALG_HANDLE_MEMREC].space             = IALG_EXTERNAL;
  memRec[ALG_HANDLE_MEMREC].attrs             = IALG_PERSIST;
  memRec[ALG_HANDLE_MEMREC].alignment         = 128;

  memRec[ALG_HANDLE_INT_MEMREC].size          = ALIGN_SIZE(sizeof(MT_Obj), 8);
  memRec[ALG_HANDLE_INT_MEMREC].space         = IALG_DARAM1;
  memRec[ALG_HANDLE_INT_MEMREC].attrs         = IALG_SCRATCH;
  memRec[ALG_HANDLE_INT_MEMREC].alignment     = 128;

  const MT_CreateParams *createParams =
       (const MT_CreateParams*)(const void*)params;
  uint32_t resizedSize = (createParams->imgWidth) * (createParams->imgHeight) / 4;
  uint32_t patchSize = (createParams->patchWidth) * (createParams->patchHeight);
  uint32_t searchSize = (createParams->searchWidth) * (createParams->searchHeight);

  memRec[ALG_HANDLE_RESIZED_IMG].size         = ALIGN_SIZE(sizeof(uint8_t)*resizedSize, 8);
  memRec[ALG_HANDLE_RESIZED_IMG].space        = IALG_EXTERNAL;
  memRec[ALG_HANDLE_RESIZED_IMG].attrs         = IALG_SCRATCH;
  memRec[ALG_HANDLE_RESIZED_IMG].alignment     = 128;

  memRec[ALG_HANDLE_TEMPLATE_U08].size         = ALIGN_SIZE(sizeof(uint8_t)*patchSize, 8);
  memRec[ALG_HANDLE_TEMPLATE_U08].space        = IALG_EXTERNAL;
  memRec[ALG_HANDLE_TEMPLATE_U08].attrs         = IALG_SCRATCH;
  memRec[ALG_HANDLE_TEMPLATE_U08].alignment     = 128;

  memRec[ALG_HANDLE_TEMPLATE_I16].size         = ALIGN_SIZE(sizeof(int16_t)*patchSize, 8);
  memRec[ALG_HANDLE_TEMPLATE_I16].space        = IALG_EXTERNAL;
  memRec[ALG_HANDLE_TEMPLATE_I16].attrs         = IALG_SCRATCH;
  memRec[ALG_HANDLE_TEMPLATE_I16].alignment     = 128;

  memRec[ALG_HANDLE_SEARCH_IMG].size         = ALIGN_SIZE(sizeof(uint8_t)*searchSize, 8);
  memRec[ALG_HANDLE_SEARCH_IMG].space        = IALG_EXTERNAL;
  memRec[ALG_HANDLE_SEARCH_IMG].attrs         = IALG_SCRATCH;
  memRec[ALG_HANDLE_SEARCH_IMG].alignment     = 128;

  return (status);
}

/**
----------------------------------------------------------------------------
*  @ingroup    MT_MOMENTA_VISION_FXNS
*  @fn         match_template_algo_init
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
XDAS_Int32 match_template_algo_init(IALG_Handle handle,
                            const IALG_MemRec memRec[],
                            IALG_Handle parent,
                            const IALG_Params *params)
{
  int32_t status = IALG_EOK;
  const MT_CreateParams *createParams =
    (const MT_CreateParams*)(const void*)params;

  MT_Handle algHandle =
    (MT_Handle)(void*)memRec[ALG_HANDLE_MEMREC].base;

  algHandle->createParams = *createParams;

  algHandle->ivision = &MT_MOMENTA_VISION_FXNS;

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
*  @ingroup    MT_MOMENTA_VISION_FXNS
*  @fn         match_template_algo_control
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
XDAS_Int32 match_template_algo_control(IVISION_Handle Handle,
                               IALG_Cmd cmd,
                               const IALG_Params *inParams,
                               IALG_Params *outParams)
{
  int32_t status = IALG_EOK;

  return (status);
}

/**
----------------------------------------------------------------------------
*  @ingroup    MT_MOMENTA_VISION_FXNS
*  @fn         match_template_algo_process
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
XDAS_Int32 match_template_algo_process(IVISION_Handle Handle,
                               IVISION_InBufs *inBufs,
                               IVISION_OutBufs *outBufs,
                               IVISION_InArgs *inArgs,
                               IVISION_OutArgs *outArgs)
{
  MT_Handle algHandle = (MT_Handle)(void*)(Handle);
  int32_t status = IALG_EOK;
  MT_InArgs *algoInArgs = (MT_InArgs*)(void*)inArgs;
  MT_OutArgs *algoOutArgs = (MT_OutArgs*)(void*)outArgs;

  uint8_t *prm = NULL;
  uint32_t *test = NULL;
  int32_t mt[4] = {0, 0, 0, 0};

  algHandle = algHandle;
  algoInArgs = algoInArgs;
  algoOutArgs = algoOutArgs;

  /*-----------------------------------------------------------------------*/
  /* Algorithm main processing                                             */
  /*-----------------------------------------------------------------------*/

  prm = inBufs->bufDesc[0]->bufPlanes[0].buf;
  //Cache_inv(prm, algHandle->createParams.imgWidth * algHandle->createParams.imgHeight, Cache_Type_ALL, TRUE);

  prm += algHandle->createParams.imgWidth * algHandle->createParams.imgHeight;
  //Cache_inv(prm, sizeof(uint32_t) * 3, Cache_Type_ALL, TRUE);

  test = (uint32_t*)prm;
  uint32_t lastFoeX = test[0];
  uint32_t lastFoeY = test[1];
  uint32_t initialized = test[2];

  // TODO: remove magic number
  // paramters
  uint32_t imgWidth = algHandle->createParams.imgWidth;
  uint32_t imgHeight = algHandle->createParams.imgHeight;
  uint32_t tempImgWidth = algHandle->createParams.patchWidth;
  uint32_t tempImgHeight = algHandle->createParams.patchHeight;
  uint32_t searchWidth = algHandle->createParams.searchWidth;
  uint32_t searchHeight = algHandle->createParams.searchHeight;
  uint32_t resizedWidth = imgWidth/4, resizedHeight=imgHeight;

  // data Pointers
  uint8_t* tempImgU08 = (uint8_t*)(void*)algHandle->memRec[ALG_HANDLE_TEMPLATE_U08].base;
  int16_t* tempImgI16 = (int16_t*)(void*)algHandle->memRec[ALG_HANDLE_TEMPLATE_I16].base;
  uint8_t* resizedImg = (uint8_t*)(void*)algHandle->memRec[ALG_HANDLE_RESIZED_IMG].base;
  uint8_t* searchImg = (uint8_t*)(void*)algHandle->memRec[ALG_HANDLE_SEARCH_IMG].base;

  if (initialized == 0) {
      // Resize current image
      //Vps_printf("the first image!\n");
      uint8_t* thisImg = inBufs->bufDesc[0]->bufPlanes[0].buf;
      MT_resize_image_h4(thisImg, imgWidth, imgHeight, imgWidth, resizedImg, resizedWidth, resizedHeight, resizedWidth);
  } else {
      // 1. Get remote patch on last image
      MT_crop_image(resizedImg, resizedWidth, resizedHeight, resizedWidth,
                    tempImgU08, tempImgWidth, tempImgHeight, tempImgWidth,
                    lastFoeX/4 - tempImgWidth/2, lastFoeY - tempImgHeight/2);

      // 2. Resize current image
      uint8_t* thisImg = inBufs->bufDesc[0]->bufPlanes[0].buf;
      MT_resize_image_h4(thisImg, imgWidth, imgHeight, imgWidth, resizedImg, resizedWidth, resizedHeight, resizedWidth);

      // 3. Get target image for searching
      MT_crop_image(resizedImg, resizedWidth, resizedHeight, resizedWidth,
                    searchImg, searchWidth, searchHeight, searchWidth,
                    lastFoeX/4 - searchWidth/2, lastFoeY - searchHeight/2);

      // 4. Matching template
      int32_t maxLocScore[3];
      int32_t state = MT_match_template_dsp(searchImg, searchWidth, searchHeight, searchWidth,
                                            tempImgU08, tempImgI16, tempImgWidth, tempImgHeight, tempImgWidth,
                                            maxLocScore);
      //Vps_printf("maxLoc: %u, %u, score=%u\n", maxLocScore[0], maxLocScore[1], maxLocScore[2]);

      // output
      mt[0] = (maxLocScore[0] - ((int32_t)searchWidth/2 - (int32_t)tempImgWidth/2)) * 4;
      mt[1] = maxLocScore[1] - ((int32_t)searchHeight/2 - (int32_t)tempImgHeight/2);
      mt[2] = maxLocScore[2];
      mt[3] = state;
  }

  memcpy(outBufs->bufDesc[0]->bufPlanes[0].buf, &mt, sizeof(int32_t) * 4);

  return status;
}

/**
----------------------------------------------------------------------------
*  @ingroup    MT_MOMENTA_VISION_FXNS
*  @fn         match_template_algo_activate
*  @brief      This is the function to do activation of algorithm.
*              All scratch memory  are restored in this
*              Refer algFree function for details in ialg.h file.
*  @param      handle : Algorithm Instance handle
*  @remarks    None
*  @return     None
----------------------------------------------------------------------------
*/
XDAS_Void  match_template_algo_activate(IALG_Handle handle)
{
  MT_Handle algHandle = (MT_Handle)(void*)(handle);
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
*  @ingroup    MT_MOMENTA_VISION_FXNS
*  @fn         match_template_algo_deactivate
*  @brief      This is the function to do de-activation of algorithm.
*              All scratch memory are stoed to external memory in this
*              Refer algFree function for details in ialg.h file.
*  @param      handle : Algorithm Instance handle
*  @remarks    None
*  @return     None
----------------------------------------------------------------------------
*/
XDAS_Void  match_template_algo_deactivate(IALG_Handle handle)
{
  MT_Handle algHandle = (MT_Handle)(void*)(handle);
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
*  @ingroup    MT_MOMENTA_VISION_FXNS
*  @fn         match_template_algo_free
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
XDAS_Int32 match_template_algo_free(IALG_Handle handle, IALG_MemRec *memRec)
{
  int32_t status = IALG_EOK;
  MT_Handle algHandle = (MT_Handle)(void*)(handle);

  memcpy(memRec, algHandle->memRec, sizeof(IALG_MemRec)*algHandle->numMemRecs);

  return status;
}
