/******************************************************************************
*
* Copyright (C) 2019-2020 Momenta Incorporated - https://www.momenta.ai/
*
* ALL RIGHTS RESERVED
*
******************************************************************************/

#include <src/hlos/links_a15/nullSrcIpcOut/nullSrcIpcOutLink_priv.h>
#include <src/hlos/links_a15/ipcInNull/ipcInNullLink_priv.h>
#include <src/hlos/common/chains_common.h>
#include "chains_hal_warp_affine_priv.h"
#include "hal_warp_affine.h"

/*******************************************************************************
* Macro define
*******************************************************************************/

/*******************************************************************************
 *  Data structures
 *******************************************************************************/

/*
    Warp Affine Chains
*/
typedef struct {
    chains_hal_warp_affineObj ucObj;
    HAL_WA_CreateParams       createParams[HAL_WA_PROC_ID_MAX];
    System_Buffer             inBuffer[HAL_WA_PROC_ID_MAX];
    System_VideoFrameBuffer   *pFrame[HAL_WA_PROC_ID_MAX];
    unsigned char             status[HAL_WA_PROC_ID_MAX];
} chains_hal_warp_affineAppObj;

static chains_hal_warp_affineAppObj gChainsObjWA;

/**
 *******************************************************************************
 *
 * \brief   Set Application parameters.
 *
 * \return  NONE
 *******************************************************************************
*/
Void chains_hal_warp_affine_SetAppPrms(unsigned int procID, chains_hal_warp_affineObj *pUcObj, Void *appObj)
{
    //chains_hal_warp_affineAppObj *pObj = (chains_hal_warp_affineAppObj*)appObj;
    AlgorithmLink_WarpAffineCreateParams *pAlgWaPrm = &(pUcObj->Alg_WarpAffine_Prm[procID]);

    /* Warp Affine Chains on DSP */
    pAlgWaPrm->numOutBuffers = 1;
}

/**
*  @brief      Initialize MOMENTA Warp Affine.
*
*  @param[in]  NONE
*
*  @retval     0 : success
*  @retval     others : failed
*
*/
int hal_warp_affine_init_process_level(void)
{
    int status = HAL_WA_OK;

    memset(&gChainsObjWA, 0, sizeof(chains_hal_warp_affineAppObj));

    return status;
}

/**
*  @brief      Create MOMENTA Warp Affine instance.
*              MOMENTA Warp Affine Links and Chains framework is started.
*
*  @param[out] handle          the handle for MOMENTA Warp Affine
*  @param[in]  inst_id         the id of the instance
*  @param[in]  init_prm        init parameters
*  @param[in]  prm_size        parameters size
*
*  @retval     0 : success
*  @retval     others : failed
*
*/
int hal_warp_affine_init_thread_level(
    hal_warp_affine_handle_t *handle,
    unsigned int inst_id,
    void *init_prm,
    unsigned int prm_size
    )
{
    int status = HAL_WA_OK;
    HAL_WA_CreateParams *createParams = (HAL_WA_CreateParams *)init_prm;
    chains_hal_warp_affineAppObj *pObj = &gChainsObjWA;

    if((!handle) || (inst_id >= HAL_WA_PROC_ID_MAX)
        || (!init_prm) || (prm_size != sizeof(HAL_WA_CreateParams)))
    {
        return HAL_WA_EINVALID_PARAMS;
    }

    if(HAL_WA_PROC_STATUS_BUSY == pObj->status[inst_id])
    {
        return HAL_WA_EBUSY;
    }
    else
    {
        /* continue */
    }

    memcpy(&(pObj->createParams[inst_id]), createParams, sizeof(HAL_WA_CreateParams));

    *handle = inst_id;
    pObj->status[inst_id] = HAL_WA_PROC_STATUS_BUSY;

    status = chains_hal_warp_affine_Create(inst_id, &pObj->ucObj, pObj);
    OSA_assert(status == SYSTEM_LINK_STATUS_SOK);
    status = chains_hal_warp_affine_Start(inst_id, &pObj->ucObj);

    pObj->pFrame[inst_id] = OSA_memAllocSR(OSA_HEAPID_DDR_CACHED_SR1, sizeof(System_VideoFrameBuffer), 32);
    memset(pObj->pFrame[inst_id], 0, sizeof(System_VideoFrameBuffer));

    pObj->inBuffer[inst_id].bufType = SYSTEM_BUFFER_TYPE_VIDEO_FRAME;
    pObj->inBuffer[inst_id].chNum = 1;
    pObj->inBuffer[inst_id].payload = pObj->pFrame[inst_id];
    pObj->inBuffer[inst_id].payloadSize = sizeof(System_VideoFrameBuffer);

    status = (status == SYSTEM_LINK_STATUS_SOK) ? HAL_WA_OK : HAL_WA_EFAIL;

    return status;
}

/**
*  @brief      Deinitialize MOMENTA Warp Affine.
*
*  @param[in]  NONE
*
*  @retval     0 : success
*  @retval     others : failed
*
*/
int hal_warp_affine_deinit_process_level(void)
{
    int status = HAL_WA_OK;

    memset(&gChainsObjWA, 0, sizeof(chains_hal_warp_affineAppObj));

    return status;
}

/**
*  @brief      Destroy MOMENTA Warp Affine instance.
*              MOMENTA Warp Affine Links and Chains framework is deleted.
*
*  @param[in]  handle  the handle for MOMENTA Warp Affine
*
*  @retval     0 : success
*  @retval     others : failed
*
*/
int hal_warp_affine_deinit_thread_level(
    hal_warp_affine_handle_t handle
    )
{
    int status = HAL_WA_OK;
    unsigned int procID = handle;
    chains_hal_warp_affineAppObj *pObj = &gChainsObjWA;

    if(procID >= HAL_WA_PROC_ID_MAX)
    {
        return HAL_WA_EINVALID_PARAMS;
    }

    status = chains_hal_warp_affine_Stop(procID, &pObj->ucObj);
    OSA_assert(status == SYSTEM_LINK_STATUS_SOK);
    status = chains_hal_warp_affine_Delete(procID, &pObj->ucObj);
    OSA_assert(status == SYSTEM_LINK_STATUS_SOK);

    status = OSA_memFreeSR(OSA_HEAPID_DDR_CACHED_SR1, pObj->pFrame[procID], sizeof(System_VideoFrameBuffer));

    pObj->status[procID] = HAL_WA_PROC_STATUS_IDLE;

    status = (status == SYSTEM_LINK_STATUS_SOK) ? HAL_WA_OK : HAL_WA_EFAIL;

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
int hal_warp_affine_malloc_continuous_aligned(
    void **data,
    unsigned int size,
    unsigned int align
    )
{
    int status = HAL_WA_OK;
    Void *addr = NULL;

    if((0 == size) || (!data))
    {
        return HAL_WA_EINVALID_PARAMS;
    }

    addr = OSA_memAllocSR(OSA_HEAPID_DDR_CACHED_SR1, size, align);

    status = (NULL != addr) ? HAL_WA_OK : HAL_WA_EALLOC;

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
int hal_warp_affine_cache_wb(
    void *data,
    unsigned int size
    )
{
    if(!data)
    {
        return HAL_WA_EINVALID_PARAMS;
    }

    ChainsCommon_Osal_cacheWb((UInt32)data, size);

    return HAL_WA_OK;
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
int hal_warp_affine_cache_inv(
    void *data,
    unsigned int size
    )
{
    if(!data)
    {
        return HAL_WA_EINVALID_PARAMS;
    }

    ChainsCommon_Osal_cacheInv((UInt32)data, size);

    return HAL_WA_OK;
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
int hal_warp_affine_free_continuous_aligned(
    void *data,
    unsigned int size
    )
{
    int status = HAL_WA_OK;

    if((0 == size) || (!data))
    {
        return HAL_WA_EINVALID_PARAMS;
    }

    status = OSA_memFreeSR(OSA_HEAPID_DDR_CACHED_SR1, data, size);

    status = (status == SYSTEM_LINK_STATUS_SOK) ? HAL_WA_OK : HAL_WA_EFAIL;

    return status;
}

/**
*  @brief      virtual memory address to physical memory address
*
*  @param[in]  virt_addr  virtual memory address
*
*  @retval     physical memory address
*
*  @remarks
*
*/
unsigned int hal_warp_affine_virt2phys(
    void *virt_addr
    )
{
    if(!virt_addr)
    {
        return HAL_WA_EINVALID_PARAMS;
    }

    return OSA_memVirt2Phys((unsigned int)virt_addr, OSA_MEM_REGION_TYPE_AUTO);
}

/**
*  @brief      Pass new data to MOMENTA Warp Affine.
*
*  @param[in]  handle  the handle for the Warp Affine algorithm
*  @param[in]  data    the new data to be processed.
*
*  @retval     0 : success
*  @retval     others : failed
*
*  @remarks
*   Please malloc continuous aligned physical memory for
*   param[in] data by hal_warp_affine_malloc_continuous_aligned.
*
*  @remarks
*   Do not release or modify param[in] data until get the output buffer.
*
*/
int hal_warp_affine_put_input_buffer(
    hal_warp_affine_handle_t handle,
    void *data
    )
{
    int status = HAL_WA_OK;
    unsigned int procID = handle;
    chains_hal_warp_affineAppObj *pObj = &gChainsObjWA;
    unsigned int offset =
        SYSTEM_LINK_ID_NULL_SRC_IPC_OUT_16 - SYSTEM_LINK_ID_NULL_SRC_IPC_OUT_0;

    if((procID >= HAL_WA_PROC_ID_MAX) || (!data))
    {
        return HAL_WA_EINVALID_PARAMS;
    }

    pObj->pFrame[procID]->bufAddr[0] = data;

    status = NullSrcIpcOutLink_PutBuffer(
                 procID + offset,
                 &(pObj->inBuffer[procID])
                 );

    status = (status == SYSTEM_LINK_STATUS_SOK) ? HAL_WA_OK : HAL_WA_EFAIL;

    return status;
}

/**
*  @brief      Wait until output buffer is ready
*
*  @param[in]  handle    the handle for the Warp Affine algorithm
*
*  @retval     0 : success
*  @retval     others : failed
*
*  @remarks
*   When output buffer is ready hal_warp_affine_output_cb will be called.
*
*/
int hal_warp_affine_wait_output_buffer(
    hal_warp_affine_handle_t handle
    )
{
    int status = HAL_WA_OK;
    unsigned int procID = handle;
    System_Buffer *pSysBuffer = NULL;
    System_MetaDataBuffer *pMetaBuffer = NULL;
    HAL_WA_OutputBuffer output;
    unsigned int offset =
        SYSTEM_LINK_ID_IPC_IN_NULL_16 - SYSTEM_LINK_ID_IPC_IN_NULL_0;

    if(procID >= HAL_WA_PROC_ID_MAX)
    {
        return HAL_WA_EINVALID_PARAMS;
    }

    memset(&output, 0, sizeof(HAL_WA_OutputBuffer));

    Vps_printf("******************WA:A15 start to wait!\n");

    while(1)
    {
        status = IpcInNullLink_GetBuffer(
                     procID + offset,
                     (void**)&pSysBuffer,
                     OSA_TIMEOUT_NONE
                     );

        if(SYSTEM_LINK_STATUS_SOK == status)
        {
            break;
        }
        else
        {
            usleep(100);
        }
    }

    Vps_printf("******************WA:A15 get the result!\n");

    pMetaBuffer = (System_MetaDataBuffer*)pSysBuffer->payload;
    output.data = pMetaBuffer->bufAddr[0];
    output.size = pMetaBuffer->metaBufSize[0];
    hal_warp_affine_cache_inv(output.data, output.size);

    if(NULL != gChainsObjWA.createParams[procID].handle)
    {
        gChainsObjWA.createParams[procID].handle(procID, &output);
    }

    status = IpcInNullLink_FreeBuffer(
                 procID + offset,
                 (void*)pSysBuffer
                 );

    status = (status == SYSTEM_LINK_STATUS_SOK) ? HAL_WA_OK : HAL_WA_EFAIL;

    return status;
}


