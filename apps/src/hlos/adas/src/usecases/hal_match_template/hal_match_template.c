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
#include "chains_hal_match_template_priv.h"
#include "hal_match_template.h"

/*******************************************************************************
* Macro define
*******************************************************************************/

/*******************************************************************************
 *  Data structures
 *******************************************************************************/

/*
    Match Template Chains
*/
typedef struct {
    chains_hal_match_templateObj ucObj;
    HAL_MT_CreateParams          createParams[HAL_MT_PROC_ID_MAX];
    System_Buffer                inBuffer[HAL_MT_PROC_ID_MAX];
    System_VideoFrameBuffer      *pFrame[HAL_MT_PROC_ID_MAX];
    unsigned char                status[HAL_MT_PROC_ID_MAX];
} chains_hal_match_templateAppObj;

static chains_hal_match_templateAppObj gChainsObjMT;

/**
 *******************************************************************************
 *
 * \brief   Set Application parameters.
 *
 * \return  NONE
 *******************************************************************************
*/
Void chains_hal_match_template_SetAppPrms(unsigned int procID, chains_hal_match_templateObj *pUcObj, Void *appObj)
{
    chains_hal_match_templateAppObj *pObj = (chains_hal_match_templateAppObj*)appObj;
    AlgorithmLink_MatchTemplateCreateParams *pAlgMtPrm = &(pUcObj->Alg_MatchTemplate_Prm[procID]);

    /* Match Template Chains on DSP */
    pAlgMtPrm->imgFrameWidth = pObj->createParams[procID].width;
    pAlgMtPrm->imgFrameHeight = pObj->createParams[procID].height;
    pAlgMtPrm->imgFramePitch = pObj->createParams[procID].pitch;

    pAlgMtPrm->patchWidth = pObj->createParams[procID].patch_width;
    pAlgMtPrm->patchHeight = pObj->createParams[procID].patch_height;
    pAlgMtPrm->searchWidth = pObj->createParams[procID].search_width;
    pAlgMtPrm->searchHeight = pObj->createParams[procID].search_height;

    pAlgMtPrm->numOutBuffers = 1;
}

/**
*  @brief      Initialize MOMENTA Match Template.
*
*  @param[in]  NONE
*
*  @retval     0 : success
*  @retval     others : failed
*
*/
int hal_match_template_init_process_level(void)
{
    int status = HAL_MT_OK;

    memset(&gChainsObjMT, 0, sizeof(chains_hal_match_templateAppObj));

    return status;
}

/**
*  @brief      Create MOMENTA Match Template instance.
*              MOMENTA Match Template Links and Chains framework is started.
*
*  @param[out] handle          the handle for MOMENTA Match Template
*  @param[in]  inst_id         the id of the instance
*  @param[in]  init_prm        init parameters
*  @param[in]  prm_size        parameters size
*
*  @retval     0 : success
*  @retval     others : failed
*
*/
int hal_match_template_init_thread_level(
    hal_match_template_handle_t *handle,
    unsigned int inst_id,
    void *init_prm,
    unsigned int prm_size
    )
{
    int status = HAL_MT_OK;
    HAL_MT_CreateParams *createParams = (HAL_MT_CreateParams *)init_prm;
    chains_hal_match_templateAppObj *pObj = &gChainsObjMT;

    if((!handle) || (inst_id >= HAL_MT_PROC_ID_MAX)
        || (!init_prm) || (prm_size != sizeof(HAL_MT_CreateParams)))
    {
        return HAL_MT_EINVALID_PARAMS;
    }

    if(HAL_MT_PROC_STATUS_BUSY == pObj->status[inst_id])
    {
        return HAL_MT_EBUSY;
    }
    else
    {
        /* continue */
    }

    memcpy(&(pObj->createParams[inst_id]), createParams, sizeof(HAL_MT_CreateParams));

    *handle = inst_id;
    pObj->status[inst_id] = HAL_MT_PROC_STATUS_BUSY;

    status = chains_hal_match_template_Create(inst_id, &pObj->ucObj, pObj);
    OSA_assert(status == SYSTEM_LINK_STATUS_SOK);
    status = chains_hal_match_template_Start(inst_id, &pObj->ucObj);

    pObj->pFrame[inst_id] = OSA_memAllocSR(OSA_HEAPID_DDR_CACHED_SR1, sizeof(System_VideoFrameBuffer), 32);
    memset(pObj->pFrame[inst_id], 0, sizeof(System_VideoFrameBuffer));

    pObj->inBuffer[inst_id].bufType = SYSTEM_BUFFER_TYPE_VIDEO_FRAME;
    pObj->inBuffer[inst_id].chNum = 1;
    pObj->inBuffer[inst_id].payload = pObj->pFrame[inst_id];
    pObj->inBuffer[inst_id].payloadSize = sizeof(System_VideoFrameBuffer);

    status = (status == SYSTEM_LINK_STATUS_SOK) ? HAL_MT_OK : HAL_MT_EFAIL;

    return status;
}

/**
*  @brief      Deinitialize MOMENTA Match Template.
*
*  @param[in]  NONE
*
*  @retval     0 : success
*  @retval     others : failed
*
*/
int hal_match_template_deinit_process_level(void)
{
    int status = HAL_MT_OK;

    memset(&gChainsObjMT, 0, sizeof(chains_hal_match_templateAppObj));

    return status;
}

/**
*  @brief      Destroy MOMENTA Match Template instance.
*              MOMENTA Match Template Links and Chains framework is deleted.
*
*  @param[in]  handle  the handle for MOMENTA Match Template
*
*  @retval     0 : success
*  @retval     others : failed
*
*/
int hal_match_template_deinit_thread_level(
    hal_match_template_handle_t handle
    )
{
    int status = HAL_MT_OK;
    unsigned int procID = handle;
    chains_hal_match_templateAppObj *pObj = &gChainsObjMT;

    if(procID >= HAL_MT_PROC_ID_MAX)
    {
        return HAL_MT_EINVALID_PARAMS;
    }

    status = chains_hal_match_template_Stop(procID, &pObj->ucObj);
    OSA_assert(status == SYSTEM_LINK_STATUS_SOK);
    status = chains_hal_match_template_Delete(procID, &pObj->ucObj);
    OSA_assert(status == SYSTEM_LINK_STATUS_SOK);

    status = OSA_memFreeSR(OSA_HEAPID_DDR_CACHED_SR1, pObj->pFrame[procID], sizeof(System_VideoFrameBuffer));

    pObj->status[procID] = HAL_MT_PROC_STATUS_IDLE;

    status = (status == SYSTEM_LINK_STATUS_SOK) ? HAL_MT_OK : HAL_MT_EFAIL;

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
int hal_match_template_malloc_continuous_aligned(
    void **data,
    unsigned int size,
    unsigned int align
    )
{
    int status = HAL_MT_OK;
    Void *addr = NULL;

    if((0 == size) || (!data))
    {
        return HAL_MT_EINVALID_PARAMS;
    }

    addr = OSA_memAllocSR(OSA_HEAPID_DDR_CACHED_SR1, size, align);

    status = (NULL != addr) ? HAL_MT_OK : HAL_MT_EALLOC;

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
int hal_match_template_cache_wb(
    void *data,
    unsigned int size
    )
{
    if(!data)
    {
        return HAL_MT_EINVALID_PARAMS;
    }

    ChainsCommon_Osal_cacheWb((UInt32)data, size);

    return HAL_MT_OK;
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
int hal_match_template_cache_inv(
    void *data,
    unsigned int size
    )
{
    if(!data)
    {
        return HAL_MT_EINVALID_PARAMS;
    }

    ChainsCommon_Osal_cacheInv((UInt32)data, size);

    return HAL_MT_OK;
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
int hal_match_template_free_continuous_aligned(
    void *data,
    unsigned int size
    )
{
    int status = HAL_MT_OK;

    if((0 == size) || (!data))
    {
        return HAL_MT_EINVALID_PARAMS;
    }

    status = OSA_memFreeSR(OSA_HEAPID_DDR_CACHED_SR1, data, size);

    status = (status == SYSTEM_LINK_STATUS_SOK) ? HAL_MT_OK : HAL_MT_EFAIL;

    return status;
}

/**
*  @brief      Pass new data to MOMENTA Match Template.
*
*  @param[in]  handle  the handle for the Match Template algorithm
*  @param[in]  data    the new data to be processed.
*
*  @retval     0 : success
*  @retval     others : failed
*
*  @remarks
*   Please malloc continuous aligned physical memory for
*   param[in] data by hal_match_template_malloc_continuous_aligned.
*
*  @remarks
*   Do not release or modify param[in] data until get the output buffer.
*
*/
int hal_match_template_put_input_buffer(
    hal_match_template_handle_t handle,
    void *data
    )
{
    int status = HAL_MT_OK;
    unsigned int procID = handle;
    chains_hal_match_templateAppObj *pObj = &gChainsObjMT;
    unsigned int offset =
        SYSTEM_LINK_ID_NULL_SRC_IPC_OUT_2 - SYSTEM_LINK_ID_NULL_SRC_IPC_OUT_0;

    if((procID >= HAL_MT_PROC_ID_MAX) || (!data))
    {
        return HAL_MT_EINVALID_PARAMS;
    }

    pObj->pFrame[procID]->bufAddr[0] = data;

    status = NullSrcIpcOutLink_PutBuffer(
                 procID + offset,
                 &(pObj->inBuffer[procID])
                 );

    Vps_printf("******************REMOTE MTC:A15 start to wait!\n");
    status = (status == SYSTEM_LINK_STATUS_SOK) ? HAL_MT_OK : HAL_MT_EFAIL;

    return status;
}

/**
*  @brief      Wait until output buffer is ready
*
*  @param[in]  handle    the handle for the Match Template algorithm
*
*  @retval     0 : success
*  @retval     others : failed
*
*  @remarks
*   When output buffer is ready hal_match_template_output_cb will be called.
*
*/
int hal_match_template_wait_output_buffer(
    hal_match_template_handle_t handle
    )
{
    int status = HAL_MT_OK;
    unsigned int procID = handle;
    System_Buffer *pSysBuffer = NULL;
    System_MetaDataBuffer *pMetaBuffer = NULL;
    HAL_MT_OutputBuffer output;
    unsigned int offset =
        SYSTEM_LINK_ID_IPC_IN_NULL_2 - SYSTEM_LINK_ID_IPC_IN_NULL_0;

    if(procID >= HAL_MT_PROC_ID_MAX)
    {
        return HAL_MT_EINVALID_PARAMS;
    }

    memset(&output, 0, sizeof(HAL_MT_OutputBuffer));

    status = IpcInNullLink_GetBuffer(
                 procID + offset,
                 (void**)&pSysBuffer,
                 OSA_TIMEOUT_FOREVER
                 );
    OSA_assert(status == SYSTEM_LINK_STATUS_SOK);

    pMetaBuffer = (System_MetaDataBuffer*)pSysBuffer->payload;
    output.data = pMetaBuffer->bufAddr[0];
    output.size = sizeof(unsigned int) * 4;
    hal_match_template_cache_inv(output.data, output.size);

    if(NULL != gChainsObjMT.createParams[procID].handle)
    {
        gChainsObjMT.createParams[procID].handle(procID, &output);
    }

    Vps_printf("******************REMOTE MTC:A15 proc done!\n");  
      
    status = IpcInNullLink_FreeBuffer(
                 procID + offset,
                 (void*)pSysBuffer
                 );

    status = (status == SYSTEM_LINK_STATUS_SOK) ? HAL_MT_OK : HAL_MT_EFAIL;

    return status;
}


