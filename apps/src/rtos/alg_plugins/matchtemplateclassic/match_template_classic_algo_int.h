/******************************************************************************
*
* Copyright (C) 2019-2020 Momenta Incorporated - https://www.momenta.ai/
*
* ALL RIGHTS RESERVED
*
******************************************************************************/

/**
----------------------------------------------------------------------------
*  @file    match_template_classic_algo_int.h
*  @brief     This file defines process function and interface structures for
*             Match Template Classic Algorithm.
*  @version 0.1 (Apr 2019) : Initial version
----------------------------------------------------------------------------
*/

#ifndef MATCH_TEMPLATE_CLASSIC_ALGO_INT_H
#define MATCH_TEMPLATE_CLASSIC_ALGO_INT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <xdais_types.h>
#include <ivision.h>
#include "match_template_classic_algo.h"
#include "edma_utils_memcpy.h"

#define ALIGN_SIZE(x,y)       ((((x) + ((y)-1)) / (y)) * (y))

/**
*  @enum   eMemrecs
*  @brief  Memory records for Match Template Classic Algorithm
*/
typedef enum
{
  /* Memory records for handle */
  /* Alg Handle should be first entry in this enum table
     as IALG mandates it */
  ALG_HANDLE_MEMREC,
  ALG_HANDLE_INT_MEMREC,

  /* Add your algorithm related Memory records */

  MAX_NUM_MEMRECS
} eMemrecs;

/**
*  @enum   eAlgState
*  @brief  State of application
*/
typedef enum
{
  ALG_NOT_ACTIVE,
  ALG_ACTIVE
} eAlgState;

/**
*  @struct MTC_Obj
*  @brief  This structure is the main handle of applet
*  @param  ivision
*          All public function pointers
*  @param  algState
*          State of algorithm to indicate
*  @param  numMemRecs
*          Number of memory records
*  @param  memRec
*          Array of memory records
*  @param  createParams
*          Create time parameters
*  @param  sample
*          add your algorithm related parameters
*/
typedef struct
{
  const IVISION_Fxns  *ivision;
  uint8_t             algState;
  uint32_t            numMemRecs;
  IALG_MemRec         memRec[MAX_NUM_MEMRECS];
  MTC_CreateParams    createParams;
  uint32_t            sample;
} MTC_Obj;

typedef MTC_Obj *MTC_Handle;

/*--------------------------------------------------------*/
/* IALG functions                                         */
/* Refer XDAIS ialg.h file for details on these functions */
/*--------------------------------------------------------*/
XDAS_Int32 match_template_classic_algo_numAlloc(void);
XDAS_Int32 match_template_classic_algo_alloc(const IALG_Params *params,
                       IALG_Fxns **parentFxns, IALG_MemRec *memRec);
XDAS_Int32 match_template_classic_algo_init(IALG_Handle handle,
                      const IALG_MemRec *memRec, IALG_Handle parent,
                      const IALG_Params *params);
XDAS_Void  match_template_classic_algo_activate(IALG_Handle handle);
XDAS_Void  match_template_classic_algo_deactivate(IALG_Handle handle);
XDAS_Int32 match_template_classic_algo_free(IALG_Handle handle, IALG_MemRec *memRec);

XDAS_Int32 match_template_classic_algo_control(IVISION_Handle Handle, IALG_Cmd cmd,
                         const IALG_Params *inParams, IALG_Params *outParams);

XDAS_Int32 match_template_classic_algo_process(IVISION_Handle Handle, IVISION_InBufs *inBufs,
                         IVISION_OutBufs *outBufs, IVISION_InArgs *inArgs,
                         IVISION_OutArgs *outArgs);

/* Get Pointer helper functions */
static inline IALG_MemRec *get_IALG_MemRec_pointer(IALG_MemRec arr[], int32_t offset);

static inline IALG_MemRec *get_IALG_MemRec_pointer(IALG_MemRec arr[], int32_t offset)
{
    return &arr[offset];
}

#endif /*MATCH_TEMPLATE_CLASSIC_ALGO_INT_H */
