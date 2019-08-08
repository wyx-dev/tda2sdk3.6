/******************************************************************************
*
* Copyright (C) 2019-2020 Momenta Incorporated - https://www.momenta.ai/
*
* ALL RIGHTS RESERVED
*
******************************************************************************/

/**
----------------------------------------------------------------------------
*  @file    warp_affine_algo.c
*  @brief     This file defines the XDAIS/iVISION functions for
*             Warp Affine Algorithm.
*  @version 0.1 (Feb 2019) : Initial version
----------------------------------------------------------------------------
*/

#include "warp_affine_algo_int.h"
#include "warp_affine_dsp.h"

Int32 Vps_printf(const char *format, ...);

/**
* IVISION interface of the algorithm
*/
const IVISION_Fxns WA_MOMENTA_VISION_FXNS =
{
  {
    &warp_affine_algo_activate,    /* IALG_Fxns.implementationId */
    &warp_affine_algo_activate,    /* IALG_Fxns.algActivate */
    &warp_affine_algo_alloc,       /* IALG_Fxns.algAlloc */
    NULL,                          /* IALG_Fxns.algControl */
    &warp_affine_algo_deactivate,  /* IALG_Fxns.algDeactivate */
    &warp_affine_algo_free,        /* IALG_Fxns.algFree */
    &warp_affine_algo_init,        /* IALG_Fxns.algInit */
    NULL,                          /* IALG_Fxns.algMoved */
    &warp_affine_algo_numAlloc     /* IALG_Fxns.algNumAlloc */
  },
  &warp_affine_algo_process,       /* IVISION_Fxns.algProcess */
  &warp_affine_algo_control        /* IVISION_Fxns.algControl */
};

/**
----------------------------------------------------------------------------
*  @ingroup    WA_MOMENTA_VISION_FXNS
*  @fn         warp_affine_algo_numAlloc
*  @brief      Function to return the number of memory table needed by algorithm
*  @remarks    None
*  @return     Number of memory table
----------------------------------------------------------------------------
*/
XDAS_Int32 warp_affine_algo_numAlloc(void)
{
  return MAX_NUM_MEMRECS;
}

/**
----------------------------------------------------------------------------
*  @ingroup    WA_MOMENTA_VISION_FXNS
*  @fn         warp_affine_algo_alloc
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
XDAS_Int32 warp_affine_algo_alloc(const IALG_Params *params,
                             IALG_Fxns **parentFxns,
                             IALG_MemRec memRec[])
{
  int32_t status = IALG_EOK;

  memRec[ALG_HANDLE_MEMREC].size              = sizeof(WA_Obj);
  memRec[ALG_HANDLE_MEMREC].space             = IALG_EXTERNAL;
  memRec[ALG_HANDLE_MEMREC].attrs             = IALG_PERSIST;
  memRec[ALG_HANDLE_MEMREC].alignment         = 128;

  memRec[ALG_HANDLE_INT_MEMREC].size          = ALIGN_SIZE(sizeof(WA_Obj), 8);
  memRec[ALG_HANDLE_INT_MEMREC].space         = IALG_DARAM1;
  memRec[ALG_HANDLE_INT_MEMREC].attrs         = IALG_SCRATCH;
  memRec[ALG_HANDLE_INT_MEMREC].alignment     = 128;

  return (status);
}

/**
----------------------------------------------------------------------------
*  @ingroup    WA_MOMENTA_VISION_FXNS
*  @fn         warp_affine_algo_init
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
XDAS_Int32 warp_affine_algo_init(IALG_Handle handle,
                            const IALG_MemRec memRec[],
                            IALG_Handle parent,
                            const IALG_Params *params)
{
  int32_t status = IALG_EOK;
  const WA_CreateParams *createParams =
    (const WA_CreateParams*)(const void*)params;

  WA_Handle algHandle =
    (WA_Handle)(void*)memRec[ALG_HANDLE_MEMREC].base;

  algHandle->createParams = *createParams;

  algHandle->ivision = &WA_MOMENTA_VISION_FXNS;

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
*  @ingroup    WA_MOMENTA_VISION_FXNS
*  @fn         warp_affine_algo_control
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
XDAS_Int32 warp_affine_algo_control(IVISION_Handle Handle,
                               IALG_Cmd cmd,
                               const IALG_Params *inParams,
                               IALG_Params *outParams)
{
  int32_t status = IALG_EOK;

  return (status);
}

/**
----------------------------------------------------------------------------
*  @ingroup    WA_MOMENTA_VISION_FXNS
*  @fn         warp_affine_algo_process
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
XDAS_Int32 warp_affine_algo_process(IVISION_Handle Handle,
                               IVISION_InBufs *inBufs,
                               IVISION_OutBufs *outBufs,
                               IVISION_InArgs *inArgs,
                               IVISION_OutArgs *outArgs)
{
  WA_Handle algHandle = (WA_Handle)(void*)(Handle);
  int32_t status = IALG_EOK;
  WA_InArgs *algoInArgs = (WA_InArgs*)(void*)inArgs;
  WA_OutArgs *algoOutArgs = (WA_OutArgs*)(void*)outArgs;

  algHandle = algHandle;
  algoInArgs = algoInArgs;
  algoOutArgs = algoOutArgs;

  /*-----------------------------------------------------------------------*/
  /* Algorithm main processing                                             */
  /*-----------------------------------------------------------------------*/
  warp_affine_dsp(inBufs->bufDesc[0]->bufPlanes[0].buf, outBufs->bufDesc[0]->bufPlanes[0].buf);

  return status;
}

/**
----------------------------------------------------------------------------
*  @ingroup    WA_MOMENTA_VISION_FXNS
*  @fn         warp_affine_algo_activate
*  @brief      This is the function to do activation of algorithm.
*              All scratch memory  are restored in this
*              Refer algFree function for details in ialg.h file.
*  @param      handle : Algorithm Instance handle
*  @remarks    None
*  @return     None
----------------------------------------------------------------------------
*/
XDAS_Void  warp_affine_algo_activate(IALG_Handle handle)
{
  WA_Handle algHandle = (WA_Handle)(void*)(handle);
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
*  @ingroup    WA_MOMENTA_VISION_FXNS
*  @fn         warp_affine_algo_deactivate
*  @brief      This is the function to do de-activation of algorithm.
*              All scratch memory are stoed to external memory in this
*              Refer algFree function for details in ialg.h file.
*  @param      handle : Algorithm Instance handle
*  @remarks    None
*  @return     None
----------------------------------------------------------------------------
*/
XDAS_Void  warp_affine_algo_deactivate(IALG_Handle handle)
{
  WA_Handle algHandle = (WA_Handle)(void*)(handle);
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
*  @ingroup    WA_MOMENTA_VISION_FXNS
*  @fn         warp_affine_algo_free
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
XDAS_Int32 warp_affine_algo_free(IALG_Handle handle, IALG_MemRec *memRec)
{
  int32_t status = IALG_EOK;
  WA_Handle algHandle = (WA_Handle)(void*)(handle);

  memcpy(memRec, algHandle->memRec, sizeof(IALG_MemRec)*algHandle->numMemRecs);

  return status;
}
