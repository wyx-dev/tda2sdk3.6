/******************************************************************************
*
* Copyright (C) 2018-2019 Momenta Incorporated - https://www.momenta.ai/
*
* ALL RIGHTS RESERVED
*
******************************************************************************/

#include <src/hlos/links_a15/nullSrcIpcOut/nullSrcIpcOutLink_priv.h>
#include <src/hlos/links_a15/ipcInNull/ipcInNullLink_priv.h>
#include <src/hlos/common/chains_common.h>
#include "chains_hal_sparse_optical_flow_priv.h"
#include "hal_sparse_optical_flow.h"

/*******************************************************************************
* Macro define
*******************************************************************************/

/*******************************************************************************
 *  Data structures
 *******************************************************************************/

/*
    Sparse Optical Flow Chains
*/
typedef struct {
    chains_hal_sparse_optical_flowObj ucObj;
    HAL_SOF_CreateParams              createParams[HAL_SOF_PROC_ID_MAX];
    System_Buffer                     inBuffer[HAL_SOF_PROC_ID_MAX];
    System_VideoFrameBuffer           *pFrame[HAL_SOF_PROC_ID_MAX];
    unsigned char                     status[HAL_SOF_PROC_ID_MAX];
} chains_hal_sparse_optical_flowAppObj;

static chains_hal_sparse_optical_flowAppObj gChainsObjSOF;

/**
 *******************************************************************************
 *
 * \brief   Set Application parameters.
 *
 * \return  NONE
 *******************************************************************************
*/
Void chains_hal_sparse_optical_flow_SetAppPrms(unsigned int procID, chains_hal_sparse_optical_flowObj *pUcObj, Void *appObj)
{
    chains_hal_sparse_optical_flowAppObj *pObj = (chains_hal_sparse_optical_flowAppObj*)appObj;
    AlgorithmLink_SparseOpticalFlowCreateParams *pAlgSofPrm = &(pUcObj->Alg_SparseOpticalFlow_Prm[procID]);
    UInt32 sfmTrackPoints = 600;
    UInt8 i;

    /* Optical Flow Chains on DSP */
    pAlgSofPrm->imgFrameStartX = pObj->createParams[procID].startX;
    pAlgSofPrm->imgFrameStartY = pObj->createParams[procID].startY;
    pAlgSofPrm->imgFrameWidth = pObj->createParams[procID].width;
    pAlgSofPrm->imgFrameHeight = pObj->createParams[procID].height;

    pAlgSofPrm->enableRoi = TRUE;
    pAlgSofPrm->roiWidth = 1264;
    pAlgSofPrm->roiHeight = 222;
    pAlgSofPrm->roiStartX = 0;
    pAlgSofPrm->roiStartY = 330;

    pAlgSofPrm->numOutBuffers = 8;

    pAlgSofPrm->trackErrThr = 441;
    pAlgSofPrm->trackMinFlowQ4 = 0;
    pAlgSofPrm->trackNmsWinSize = 3;

    pAlgSofPrm->numLevels = 5;
    pAlgSofPrm->keyPointDetectMethod = 1;
    pAlgSofPrm->keyPointDetectInterval = 0;
    pAlgSofPrm->maxNumKeyPoints = sfmTrackPoints;
    pAlgSofPrm->maxPrevTrackPoints = 768;
    pAlgSofPrm->fast9Threshold = 80;
    pAlgSofPrm->scoreMethod = 0;
    pAlgSofPrm->harrisScaling = 1350;
    pAlgSofPrm->nmsThreshold = 1350;
    pAlgSofPrm->harrisScoreMethod = 1;
    pAlgSofPrm->harrisWindowSize = 2;
    pAlgSofPrm->suppressionMethod = 0;

    pAlgSofPrm->trackOffsetMethod = 0;

    pAlgSofPrm->maxItersLK[0] = 8;
    pAlgSofPrm->maxItersLK[1] = 7;
    pAlgSofPrm->maxItersLK[2] = 7;
    pAlgSofPrm->maxItersLK[3] = 6;
    pAlgSofPrm->maxItersLK[4] = 6;
    pAlgSofPrm->maxItersLK[5] = 10;
    pAlgSofPrm->maxItersLK[6] = 10;
    pAlgSofPrm->maxItersLK[7] = 10;

    for(i=0; i<PYRAMID_LK_TRACKER_TI_MAXLEVELS;i++)
    {
        pAlgSofPrm->minErrValue[i] = 51;
        pAlgSofPrm->searchRange[i] = 12;
    }

    pAlgSofPrm->compositeBufferInput = -1;

#ifndef ENABLE_SFM_INPUT_TEST
    pAlgSofPrm->enableRoi = FALSE;
#endif
}

/**
*  @brief      Initialize MOMENTA Sparse Optical Flow.
*
*  @param[in]  NONE
*
*  @retval     0 : success
*  @retval     others : failed
*
*/
int hal_sparse_optical_flow_init_process_level(void)
{
    int status = HAL_SOF_OK;

    memset(&gChainsObjSOF, 0, sizeof(chains_hal_sparse_optical_flowAppObj));

    return status;
}

/**
*  @brief      Create MOMENTA Sparse Optical Flow instance.
*              MOMENTA Sparse Optical Flow Links and Chains framework is started.
*
*  @param[out] handle          the handle for MOMENTA Sparse Optical Flow
*  @param[in]  inst_id         the id of the instance
*  @param[in]  init_prm        init parameters
*  @param[in]  prm_size        parameters size
*
*  @retval     0 : success
*  @retval     others : failed
*
*/
int hal_sparse_optical_flow_init_thread_level(
    hal_sparse_optical_flow_handle_t *handle,
    unsigned int inst_id,
    void *init_prm,
    unsigned int prm_size
    )
{
    int status = HAL_SOF_OK;
    HAL_SOF_CreateParams *createParams = (HAL_SOF_CreateParams *)init_prm;
    chains_hal_sparse_optical_flowAppObj *pObj = &gChainsObjSOF;

    if((!handle) || (inst_id >= HAL_SOF_PROC_ID_MAX)
        || (!init_prm) || (prm_size != sizeof(HAL_SOF_CreateParams)))
    {
        return HAL_SOF_EINVALID_PARAMS;
    }

    if(HAL_SOF_PROC_STATUS_BUSY == pObj->status[inst_id])
    {
        return HAL_SOF_EBUSY;
    }
    else
    {
        /* continue */
    }

    memcpy(&(pObj->createParams[inst_id]), createParams, sizeof(HAL_SOF_CreateParams));

    *handle = inst_id;
    pObj->status[inst_id] = HAL_SOF_PROC_STATUS_BUSY;

    status = chains_hal_sparse_optical_flow_Create(inst_id, &pObj->ucObj, pObj);
    OSA_assert(status == SYSTEM_LINK_STATUS_SOK);
    status = chains_hal_sparse_optical_flow_Start(inst_id, &pObj->ucObj);

    pObj->pFrame[inst_id] = OSA_memAllocSR(OSA_HEAPID_DDR_CACHED_SR1, sizeof(System_VideoFrameBuffer), 32);
    memset(pObj->pFrame[inst_id], 0, sizeof(System_VideoFrameBuffer));

    pObj->inBuffer[inst_id].bufType = SYSTEM_BUFFER_TYPE_VIDEO_FRAME;
    pObj->inBuffer[inst_id].chNum = 1;
    pObj->inBuffer[inst_id].payload = pObj->pFrame[inst_id];
    pObj->inBuffer[inst_id].payloadSize = sizeof(System_VideoFrameBuffer);

    status = (status == SYSTEM_LINK_STATUS_SOK) ? HAL_SOF_OK : HAL_SOF_EFAIL;

    return status;
}

/**
*  @brief      Deinitialize MOMENTA Sparse Optical Flow.
*
*  @param[in]  NONE
*
*  @retval     0 : success
*  @retval     others : failed
*
*/
int hal_sparse_optical_flow_deinit_process_level(void)
{
    int status = HAL_SOF_OK;

    memset(&gChainsObjSOF, 0, sizeof(chains_hal_sparse_optical_flowAppObj));

    return status;
}

/**
*  @brief      Destroy MOMENTA Sparse Optical Flow instance.
*              MOMENTA Sparse Optical Flow Links and Chains framework is deleted.
*
*  @param[in]  handle  the handle for MOMENTA Sparse Optical Flow
*
*  @retval     0 : success
*  @retval     others : failed
*
*/
int hal_sparse_optical_flow_deinit_thread_level(
    hal_sparse_optical_flow_handle_t handle
    )
{
    int status = HAL_SOF_OK;
    unsigned int procID = handle;
    chains_hal_sparse_optical_flowAppObj *pObj = &gChainsObjSOF;

    if(procID >= HAL_SOF_PROC_ID_MAX)
    {
        return HAL_SOF_EINVALID_PARAMS;
    }

    status = chains_hal_sparse_optical_flow_Stop(procID, &pObj->ucObj);
    OSA_assert(status == SYSTEM_LINK_STATUS_SOK);
    status = chains_hal_sparse_optical_flow_Delete(procID, &pObj->ucObj);
    OSA_assert(status == SYSTEM_LINK_STATUS_SOK);

    status = OSA_memFreeSR(OSA_HEAPID_DDR_CACHED_SR1, pObj->pFrame[procID], sizeof(System_VideoFrameBuffer));

    pObj->status[procID] = HAL_SOF_PROC_STATUS_IDLE;

    status = (status == SYSTEM_LINK_STATUS_SOK) ? HAL_SOF_OK : HAL_SOF_EFAIL;

    return status;
}

/**
*  @brief      Malloc continuous aligned physical memory
*
*  @param[out] data  Allocated memory
*  @param[in]  size  Memory size
*  @param[in]  align Align bytes value
*
*  @retval     0 : success
*  @retval     others : failed
*
*  @remarks
*   param[out] *data has been converted to virtual address.
*
*/
int hal_sparse_optical_flow_malloc_continuous_aligned(
    void **data,
    unsigned int size,
    unsigned int align
    )
{
    int status = HAL_SOF_OK;
    Void *addr = NULL;

    if((0 == size) || (!data))
    {
        return HAL_SOF_EINVALID_PARAMS;
    }

    addr = OSA_memAllocSR(OSA_HEAPID_DDR_CACHED_SR1, size, align);

    status = (NULL != addr) ? HAL_SOF_OK : HAL_SOF_EALLOC;

    *data = addr;

    return status;
}

/**
*  @brief      Cache wb
*
*  @param[out] data  wb cache
*  @param[in]  size  wb size
*
*  @retval     0 : success
*  @retval     others : failed
*
*/
int hal_sparse_optical_flow_cache_wb(
    void *data,
    unsigned int size
    )
{
    if(!data)
    {
        return HAL_SOF_EINVALID_PARAMS;
    }

    ChainsCommon_Osal_cacheWb((UInt32)data, size);

    return HAL_SOF_OK;
}

/**
*  @brief      Cache inv
*
*  @param[out] data  inv cache
*  @param[in]  size  inv size
*
*  @retval     0 : success
*  @retval     others : failed
*
*/
int hal_sparse_optical_flow_cache_inv(
    void *data,
    unsigned int size
    )
{
    if(!data)
    {
        return HAL_SOF_EINVALID_PARAMS;
    }

    ChainsCommon_Osal_cacheInv((UInt32)data, size);

    return HAL_SOF_OK;
}

/**
*  @brief      Free continuous aligned physical memory
*
*  @param[in]  data  Allocated memory
*  @param[in]  size  Memory size
*
*  @retval     0 : success
*  @retval     others : failed
*
*  @remarks
*   param[in] data should be virtual address.
*
*/
int hal_sparse_optical_flow_free_continuous_aligned(
    void *data,
    unsigned int size
    )
{
    int status = HAL_SOF_OK;

    if((0 == size) || (!data))
    {
        return HAL_SOF_EINVALID_PARAMS;
    }

    status = OSA_memFreeSR(OSA_HEAPID_DDR_CACHED_SR1, data, size);

    status = (status == SYSTEM_LINK_STATUS_SOK) ? HAL_SOF_OK : HAL_SOF_EFAIL;

    return status;
}

/**
*  @brief      Pass new data to MOMENTA Sparse Optical Flow.
*
*  @param[in]  handle  the handle for the Sparse Optical Flow algorithm
*  @param[in]  data    the new data to be processed.
*
*  @retval     0 : success
*  @retval     others : failed
*
*  @remarks
*   Please malloc continuous aligned physical memory for
*   param[in] data by hal_sparse_optical_flow_malloc_continuous_aligned.
*
*  @remarks
*   Do not release or modify param[in] data until get the output buffer.
*
*/
int hal_sparse_optical_flow_put_input_buffer(
    hal_sparse_optical_flow_handle_t handle,
    void *data
    )
{
    int status = HAL_SOF_OK;
    unsigned int procID = handle;
    chains_hal_sparse_optical_flowAppObj *pObj = &gChainsObjSOF;
    unsigned int offset =
        SYSTEM_LINK_ID_NULL_SRC_IPC_OUT_11 - SYSTEM_LINK_ID_NULL_SRC_IPC_OUT_0;

    if((procID >= HAL_SOF_PROC_ID_MAX) || (!data))
    {
        return HAL_SOF_EINVALID_PARAMS;
    }

    pObj->pFrame[procID]->bufAddr[0] = data;

    status = NullSrcIpcOutLink_PutBuffer(
                 procID * IPC_INSTANCE_INTERVAL + offset,
                 &(pObj->inBuffer[procID])
                 );

    status = (status == SYSTEM_LINK_STATUS_SOK) ? HAL_SOF_OK : HAL_SOF_EFAIL;

    return status;
}

/**
*  @brief      Wait until output buffer is ready
*
*  @param[in]  handle    the handle for the Sparse Optical Flow algorithm
*
*  @retval     0 : success
*  @retval     others : failed
*
*  @remarks
*   When output buffer is ready hal_sparse_optical_flow_output_cb will be called.
*
*/
int hal_sparse_optical_flow_wait_output_buffer(
    hal_sparse_optical_flow_handle_t handle
    )
{
    int status = HAL_SOF_OK;
    unsigned int procID = handle;
    System_Buffer *pSysBuffer = NULL;
    System_MetaDataBuffer *pMetaBuffer = NULL;
    HAL_SOF_OutputBuffer output;
    unsigned int offset =
        SYSTEM_LINK_ID_IPC_IN_NULL_11 - SYSTEM_LINK_ID_IPC_IN_NULL_0;

    if(procID >= HAL_SOF_PROC_ID_MAX)
    {
        return HAL_SOF_EINVALID_PARAMS;
    }

    memset(&output, 0, sizeof(HAL_SOF_OutputBuffer));

    status = IpcInNullLink_GetBuffer(
                 procID * IPC_INSTANCE_INTERVAL + offset,
                 (void**)&pSysBuffer,
                 OSA_TIMEOUT_FOREVER
                 );
    OSA_assert(status == SYSTEM_LINK_STATUS_SOK);

    pMetaBuffer = (System_MetaDataBuffer*)pSysBuffer->payload;
    output.data = pMetaBuffer->bufAddr[0];
    output.size = sizeof(float) * 4;
    hal_sparse_optical_flow_cache_inv(output.data, output.size);

    if(NULL != gChainsObjSOF.createParams[procID].handle)
    {
        gChainsObjSOF.createParams[procID].handle(procID, &output);
    }

    status = IpcInNullLink_FreeBuffer(
                 procID + offset,
                 (void*)pSysBuffer
                 );

    status = (status == SYSTEM_LINK_STATUS_SOK) ? HAL_SOF_OK : HAL_SOF_EFAIL;

    return status;
}


