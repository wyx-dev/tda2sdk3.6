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
#include "itidl_ti_linux.h"
#include "chains_hal_modl_priv.h"
#include "hal_modl_common.h"
#include "hal_modl.h"

/*******************************************************************************
* Macro define
*******************************************************************************/

/* OCMC */
#define OCMC_1_BASE_ADDRESS          (0x40300000U)
#define OCMC_1_SIZE                  (512 * 1024)

#define OCMC_2_BASE_ADDRESS          (0x40400000U)
#define OCMC_2_SIZE                  (1024 * 1024)

#define OCMC_3_BASE_ADDRESS          (0x40500000U)
#define OCMC_3_SIZE                  (1024 * 1024)

/*******************************************************************************
 *  Data structures
 *******************************************************************************/

/*
    Deep Learning Chains
*/
typedef struct {
    chains_hal_modlObj      ucObj;
    TIDL_CreateParams       *pAlgCreateParams[MODL_PROC_ID_MAX];
    HAL_MODL_Network        modlNet[MODL_PROC_ID_MAX];
    System_Buffer           inBuffer[MODL_PROC_ID_MAX];
    System_VideoFrameBuffer *pFrame[MODL_PROC_ID_MAX];
    unsigned char           netStatus[MODL_PROC_ID_MAX];
} chains_hal_modlAppObj;

static chains_hal_modlAppObj gChainsObjModl;

static unsigned int ocmcAddrTbl[MAX_TIDL_CORE_COUNT] = {
    OCMC_1_BASE_ADDRESS,
    OCMC_2_BASE_ADDRESS,
    OCMC_3_BASE_ADDRESS,
    OCMC_3_BASE_ADDRESS + (OCMC_3_SIZE >> 1)
};

/**
 *******************************************************************************
 *
 * \brief   Set Application parameters.
 *
 * \return  NONE
 *******************************************************************************
*/
Void chains_hal_modl_SetAppPrms(unsigned int procID, chains_hal_modlObj *pUcObj, Void *appObj)
{
    chains_hal_modlAppObj *pObj = (chains_hal_modlAppObj*)appObj;

    /* TIDL on EVE */
    pUcObj->Alg_tidl_Prm[procID].cacheInvInBuf = 1;
    pUcObj->Alg_tidl_Prm[procID].cacheWbOutBuf = 1;
    pUcObj->Alg_tidl_Prm[procID].pAlgCreateParams = pObj->pAlgCreateParams[procID];
    pUcObj->Alg_tidl_Prm[procID].curCoreId = 1;
    pUcObj->Alg_tidl_Prm[procID].currLayersGroupId = 1;
    pUcObj->Alg_tidl_Prm[procID].ocmcAddr = ocmcAddrTbl[procID / MAX_TIDL_INSTANCE_COUNT];
}

/**
*  @brief      Initialize MOMENTA TIDL.
*
*  @param[in]  NONE
*
*  @retval     0 : success
*  @retval     others : failed
*
*/
int hal_modl_init_process_level(void)
{
    int status = HAL_MODL_OK;

    memset(&gChainsObjModl, 0, sizeof(chains_hal_modlAppObj));

    status = System_linkControl(
                    (UInt32)SYSTEM_LINK_ID_IPU1_0,
                    SYSTEM_COMMON_CMD_OCMC_RAMS_INIT,
                    NULL,
                    0,
                    (Bool)TRUE
                    );

    status = (status == SYSTEM_LINK_STATUS_SOK) ? HAL_MODL_OK : HAL_MODL_EFAIL;

    return status;
}

/**
*  @brief      NOP
*
*  @param[in]  NONE
*
*  @retval     0 : success
*  @retval     others : failed
*
*/
int hal_modl_init_thread_level(void)
{
    return HAL_MODL_OK;
}

/**
*  @brief      Deinitialize MOMENTA TIDL.
*
*  @param[in]  NONE
*
*  @retval     0 : success
*  @retval     others : failed
*
*/
int hal_modl_deinit_process_level(void)
{
    int status = HAL_MODL_OK;

    memset(&gChainsObjModl, 0, sizeof(chains_hal_modlAppObj));

    return status;
}

/**
*  @brief      NOP
*
*  @param[in]  NONE
*
*  @retval     0 : success
*  @retval     others : failed
*
*/
int hal_modl_deinit_thread_level(void)
{
    return HAL_MODL_OK;
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
int hal_modl_malloc_continuous_aligned(
    void **data,
    unsigned int size,
    unsigned int align
    )
{
    int status = HAL_MODL_OK;
    Void *addr = NULL;

    if((0 == size) || (!data))
    {
        return HAL_MODL_EINVALID_PARAMS;
    }

    addr = OSA_memAllocSR(OSA_HEAPID_DDR_CACHED_SR1, size, align);

    status = (NULL != addr) ? HAL_MODL_OK : HAL_MODL_EALLOC;

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
int hal_modl_cache_wb(
    void *data,
    unsigned int size
    )
{
    if(!data)
    {
        return HAL_MODL_EINVALID_PARAMS;
    }

    ChainsCommon_Osal_cacheWb((UInt32)data, size);

    return HAL_MODL_OK;
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
int hal_modl_cache_inv(
    void *data,
    unsigned int size
    )
{
    if(!data)
    {
        return HAL_MODL_EINVALID_PARAMS;
    }

    ChainsCommon_Osal_cacheInv((UInt32)data, size);

    return HAL_MODL_OK;
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
int hal_modl_free_continuous_aligned(
    void *data,
    unsigned int size
    )
{
    int status = HAL_MODL_OK;

    if((0 == size) || (!data))
    {
        return HAL_MODL_EINVALID_PARAMS;
    }

    status = OSA_memFreeSR(OSA_HEAPID_DDR_CACHED_SR1, data, size);

    status = (status == SYSTEM_LINK_STATUS_SOK) ? HAL_MODL_OK : HAL_MODL_EFAIL;

    return status;
}

/**
*  @brief      Create MOMENTA TIDL network.
*              MOMENTA TIDL Links and Chains framework is started.
*
*  @param[out] handle          the handle for the deep learning network
*  @param[in]  inst_id         the id of the instance
*  @param[in]  tidl_net        the deep learning network(sTIDL_Network_t)(NET.BIN)
*  @param[in]  tidl_net_size   network size : sizeof(sTIDL_Network_t)
*  @param[in]  tidl_param      network parameters(PRM.BIN)
*  @param[in]  tidl_param_size parameters size
*
*  @retval     0 : success
*  @retval     others : failed
*
*  @remarks
*   param[out] *handle is specified as inst_id for now.
*
*  @remarks
*   Please malloc continuous aligned physical memory for
*   tidl_net and tidl_param by hal_modl_malloc_continuous_aligned.
*
*  @remarks
*   It's OK to set tidl_param = NULL and tidl_param_size = 0
*   if there is no parameter setting.
*
*  @remarks
*   Support two network instance running on one EVE.
*   Each MODL_PROC_ID_XX instance could be created only once.
*   HAL_MODL_EBUSY will be returned
*   when repeat creating network using the same MODL_PROC_ID_XX.
*
*  @remarks
*   TIDL_CreateParams default setting:
*   currCoreId         SYSTEM_PROC_EVE1/2/3/4(ignore user setting fow now)
*   currLayersGroupId  0(ignore user setting fow now)
*   l1MemSize          21 KB(l1 > l2 to be confirmed)
*   l2MemSize          8 KB
*   l3MemSize          320 KB
*   quantHistoryParam1 0
*   quantHistoryParam2 0
*   quantMargin        0
*
*/
int hal_modl_create_net(
    hal_modl_handle_t *handle,
    unsigned int inst_id,
    void *tidl_net,
    unsigned int tidl_net_size,
    void *tidl_param,
    unsigned int tidl_param_size
    )
{
    int status = HAL_MODL_OK;
    sTIDL_Network_t *pNetSrc = NULL;
    sTIDL_Network_t *pNetDst = NULL;
    HAL_MODL_Network *modlNet = (HAL_MODL_Network *)tidl_net;
    chains_hal_modlAppObj *pObj = &gChainsObjModl;

    if((!handle) || (inst_id >= MODL_PROC_ID_MAX) || (!tidl_net))
    {
        return HAL_MODL_EINVALID_PARAMS;
    }

    if(MODL_PROC_STATUS_BUSY == pObj->netStatus[inst_id])
    {
        return HAL_MODL_EBUSY;
    }
    else
    {
        /* continue */
    }

    pNetSrc = (sTIDL_Network_t *)(modlNet->pNetwork);
    pObj->modlNet[inst_id].currCoreId = modlNet->currCoreId;
    pObj->modlNet[inst_id].currLayersGroupId = modlNet->currLayersGroupId;
    pObj->modlNet[inst_id].handle = modlNet->handle;

    if((tidl_net_size != sizeof(HAL_MODL_Network)) || (!pNetSrc))
    {
        return HAL_MODL_EFAIL;
    }
    else
    {
        *handle = inst_id;
        pObj->netStatus[inst_id] = MODL_PROC_STATUS_BUSY;
    }

    /* Allocate the Algorithm create structure */
    pObj->pAlgCreateParams[inst_id] = OSA_memAllocPhysSR(OSA_HEAPID_DDR_CACHED_SR1, sizeof(TIDL_CreateParams), 32);
    pObj->pAlgCreateParams[inst_id] = (TIDL_CreateParams *)OSA_memPhys2Virt((unsigned int)pObj->pAlgCreateParams[inst_id], OSA_MEM_REGION_TYPE_AUTO);
    memset(pObj->pAlgCreateParams[inst_id], 0, sizeof(TIDL_CreateParams));

    /* Read the network */
    pNetDst = &pObj->pAlgCreateParams[inst_id]->net;
    memcpy(pNetDst, pNetSrc, sizeof(sTIDL_Network_t));

    /* Allocate memory for the net */
    status = hal_modl_AllocNetParamsMem(pNetDst);
    OSA_assert(status == SYSTEM_LINK_STATUS_SOK);
    status = hal_modl_NetParamsMemPhys2Virt(pNetDst);
    OSA_assert(status == SYSTEM_LINK_STATUS_SOK);

    /* Fill the new parameters */
    if((0 != tidl_param_size) && (NULL != tidl_param))
    {
        status = hal_modl_FillNetParamsMem(pNetDst, tidl_param);
        OSA_assert(status == SYSTEM_LINK_STATUS_SOK);
    }
    status = hal_modl_NetParamsMemVirt2Phys(pNetDst);
    OSA_assert(status == SYSTEM_LINK_STATUS_SOK);
    ChainsCommon_Osal_cacheWb((UInt32)pObj->pAlgCreateParams[inst_id], sizeof(TIDL_CreateParams));
    pObj->pAlgCreateParams[inst_id] = (TIDL_CreateParams *)OSA_memVirt2Phys((unsigned int)pObj->pAlgCreateParams[inst_id], OSA_MEM_REGION_TYPE_AUTO);

    status = chains_hal_modl_Create(inst_id, &pObj->ucObj, pObj);
    OSA_assert(status == SYSTEM_LINK_STATUS_SOK);
    status = chains_hal_modl_Start(inst_id, &pObj->ucObj);

    pObj->pFrame[inst_id] = OSA_memAllocSR(OSA_HEAPID_DDR_CACHED_SR1, sizeof(System_VideoFrameBuffer), 32);
    memset(pObj->pFrame[inst_id], 0, sizeof(System_VideoFrameBuffer));

    pObj->inBuffer[inst_id].bufType = SYSTEM_BUFFER_TYPE_VIDEO_FRAME;
    pObj->inBuffer[inst_id].chNum = 1;
    pObj->inBuffer[inst_id].payload = pObj->pFrame[inst_id];
    pObj->inBuffer[inst_id].payloadSize = sizeof(System_VideoFrameBuffer);

    status = (status == SYSTEM_LINK_STATUS_SOK) ? HAL_MODL_OK : HAL_MODL_EFAIL;

    return status;
}

/**
*  @brief      Destroy MOMENTA TIDL network.
*              MOMENTA TIDL Links and Chains framework is deleted.
*
*  @param[in]  handle  the handle for the deep learning network
*
*  @retval     0 : success
*  @retval     others : failed
*
*/
int hal_modl_destroy_net(
    hal_modl_handle_t handle
    )
{
    int status = HAL_MODL_OK;
    unsigned int procID = handle;
    chains_hal_modlAppObj *pObj = &gChainsObjModl;
    sTIDL_Network_t *pVirtNet;

    if(procID >= MODL_PROC_ID_MAX)
    {
        return HAL_MODL_EINVALID_PARAMS;
    }

    pVirtNet = (sTIDL_Network_t*)ChainsCommon_Osal_getVirtAddr((UInt32)&pObj->pAlgCreateParams[procID]->net);

    status = chains_hal_modl_Stop(procID, &pObj->ucObj);
    OSA_assert(status == SYSTEM_LINK_STATUS_SOK);
    status = chains_hal_modl_Delete(procID, &pObj->ucObj);
    OSA_assert(status == SYSTEM_LINK_STATUS_SOK);

    /* Free the net memory */
    status = hal_modl_FreeNetParamsMem(pVirtNet);
    OSA_assert(status == SYSTEM_LINK_STATUS_SOK);

    /* Free the Algorithm create structure */
    ChainsCommon_Osal_memFree(pObj->pAlgCreateParams[procID], sizeof(TIDL_CreateParams));

    status = OSA_memFreeSR(OSA_HEAPID_DDR_CACHED_SR1, pObj->pFrame[procID], sizeof(System_VideoFrameBuffer));

    pObj->netStatus[procID] = MODL_PROC_STATUS_IDLE;

    status = (status == SYSTEM_LINK_STATUS_SOK) ? HAL_MODL_OK : HAL_MODL_EFAIL;

    return status;
}

/**
*  @brief      Pass new data to MOMENTA TIDL.
*
*  @param[in]  handle  the handle for the deep learning network
*  @param[in]  data    the new data to be processed.
*
*  @retval     0 : success
*  @retval     others : failed
*
*  @remarks
*   param[in] data format should fit the NET.BIN
*
*   Multiple frame input is supported.
*
*   Each input frame should pad 4 pixel data according to TIDL document.
*
*   16x4 bytes data Q information should be padded right after multiple input frames.
*   Set each data Q = 256(Big-Endian) if scaling factor is not needed.
*
*   Input data size = width*height*channel*number + data Q(16x4 bytes)
*
*  @remarks
*   Please malloc continuous aligned physical memory for
*   param[in] data by hal_modl_malloc_continuous_aligned.
*
*  @remarks
*   Do not release or modify param[in] data until get the output buffer.
*
*/
int hal_modl_put_input_buffer(
    hal_modl_handle_t handle,
    void *data
    )
{
    int status = HAL_MODL_OK;
    unsigned int procID = handle;
    chains_hal_modlAppObj *pObj = &gChainsObjModl;

    if((procID >= MODL_PROC_ID_MAX) || (!data))
    {
        return HAL_MODL_EINVALID_PARAMS;
    }

    Vps_printf("Hal modl input: proc id:%d!\n", procID);
    pObj->pFrame[procID]->bufAddr[0] = data;

    status = NullSrcIpcOutLink_PutBuffer(procID, &(pObj->inBuffer[procID]));

    status = (status == SYSTEM_LINK_STATUS_SOK) ? HAL_MODL_OK : HAL_MODL_EFAIL;

    return status;
}

/**
*  @brief      Wait until output buffer is ready
*
*  @param[in]  handle    the handle for the deep learning network
*  @param[in]  user_arg  user data used by hal_modl_output_cb
*
*  @retval     0 : success
*  @retval     others : failed
*
*  @remarks
*   When output buffer is ready hal_modl_output_cb will be called.
*
*/
int hal_modl_wait_output_buffer(
    hal_modl_handle_t handle,
    void *user_arg
    )
{
    int status = HAL_MODL_OK;
    unsigned int procID = handle;
    System_Buffer *pSysBuffer = NULL;
    System_MetaDataBuffer *pMetaBuffer = NULL;
    HAL_MODL_OutputBuffer output;
    int timeout_count = 0;

    if(procID >= MODL_PROC_ID_MAX)
    {
        return HAL_MODL_EINVALID_PARAMS;
    }

    memset(&output, 0, sizeof(HAL_MODL_OutputBuffer));

    while(1)
    {
        status = IpcInNullLink_GetBuffer(procID, (void**)&pSysBuffer, OSA_TIMEOUT_NONE);

        if(SYSTEM_LINK_STATUS_SOK == status)
        {
            break;
        }
        else
        {
            usleep(200);
            timeout_count++;
            if (timeout_count >= 5000)
            {
                Vps_printf("!!!!!!!!!!!Hal modl timeout!!!!!!: proc id:%d!\n", procID);
                OSA_assert(0);
            }
        }
    }

    pMetaBuffer = (System_MetaDataBuffer*)pSysBuffer->payload;
    output.frame = pMetaBuffer->bufAddr[0];
    output.size = pMetaBuffer->metaBufSize[0];
    hal_modl_cache_inv(output.frame, output.size);

    Vps_printf("Hal modl get result: proc id:%d!\n", procID);
    if(NULL != gChainsObjModl.modlNet[procID].handle)
    {
        gChainsObjModl.modlNet[procID].handle(procID, &output, user_arg);
    }

    status = IpcInNullLink_FreeBuffer(procID, (void*)pSysBuffer);

    status = (status == SYSTEM_LINK_STATUS_SOK) ? HAL_MODL_OK : HAL_MODL_EFAIL;

    return status;
}


