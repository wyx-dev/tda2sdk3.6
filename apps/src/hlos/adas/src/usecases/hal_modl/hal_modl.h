/******************************************************************************
*
* Copyright (C) 2018-2019 Momenta Incorporated - https://www.momenta.ai/
*
* ALL RIGHTS RESERVED
*
******************************************************************************/

/**
----------------------------------------------------------------------------
@file    hal_modl.h
@brief     This file defines the TIDL interface for MOMENTA deep learning.
@version 0.1 (Sep 2018) : Initial Code
----------------------------------------------------------------------------
*/

#ifndef _HAL_MODL_H_
#define _HAL_MODL_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
*  @brief The handle for the deep learning network.
*/
typedef unsigned int hal_modl_handle_t;

/**
*  @brief The return status of the API.
*/
#define HAL_MODL_OK              (0)
#define HAL_MODL_EFAIL           (-1)
#define HAL_MODL_ETIMEOUT        (-2)
#define HAL_MODL_EALLOC          (-3)
#define HAL_MODL_EBUSY           (-4)
#define HAL_MODL_EINVALID_PARAMS (-5)

/**
*  @struct HAL_MODL_OutputBuffer
*  @brief  This structure contains all the MOMENTA TIDL
*          output data parameters.
*  @param  size
*          output buffer size
*  @param  frame
*          MOMENTA TIDL output buffer
*  @remarks
*   The last 64 byte of frame is data Q
*/
typedef struct {
    unsigned int size;
    unsigned char *frame;
} HAL_MODL_OutputBuffer;

/**
*  @enum    eMODL_PROC_ID
*  @brief   This enumerator defines
*           the Process ID of MOMENTA Deep Learning.
*/
typedef enum {
    MODL_PROC_ID_EVE1_INST0 = 0,
    MODL_PROC_ID_EVE1_INST1 = 1,
    MODL_PROC_ID_EVE1_INST2 = 2,
    MODL_PROC_ID_EVE1_INST3 = 3,
    MODL_PROC_ID_EVE2_INST0 = 4,
    MODL_PROC_ID_EVE2_INST1 = 5,
    MODL_PROC_ID_EVE2_INST2 = 6,
    MODL_PROC_ID_EVE2_INST3 = 7,
    MODL_PROC_ID_EVE3_INST0 = 8,
    MODL_PROC_ID_EVE3_INST1 = 9,
    MODL_PROC_ID_EVE3_INST2 = 10,
    MODL_PROC_ID_EVE3_INST3 = 11,
    MODL_PROC_ID_EVE4_INST0 = 12,
    MODL_PROC_ID_EVE4_INST1 = 13,
    MODL_PROC_ID_EVE4_INST2 = 14,
    MODL_PROC_ID_EVE4_INST3 = 15,
    MODL_PROC_ID_MAX
} eMODL_PROC_ID;

/**
*  @enum    eMODL_PROC_STATUS
*  @brief   This enumerator defines
*           the status of the Process.
*/
typedef enum {
    MODL_PROC_STATUS_IDLE = 0,
    MODL_PROC_STATUS_BUSY = 1,
    MODL_PROC_STATUS_MAX
} eMODL_PROC_STATUS;

/**
*  @brief      Call back for handling MOMENTA TIDL output data.
*
*  @param[in]  handle    the handle for the deep learning network
*  @param[in]  output    The MOMENTA TIDL output buffer
*  @param[in]  user_arg  user data set by hal_modl_wait_output_buffer
*
*  @retval     NONE
*
*/
typedef void (*hal_modl_output_cb)(
    hal_modl_handle_t handle,
    HAL_MODL_OutputBuffer *output,
    void *user_arg
    );

/**
*  @struct HAL_MODL_Network
*  @brief  This structure contains all the parameters
*          which MOMENTA TIDL needs at net create time.
*  @param  currCoreId
*          Id of the core (EVE/DSP) on which TIDL is running
*  @param  currLayersGroupId
*          Id of the each layer (EVE/DSP) on which that layer is 
*          running
*  @param  pNetwork
*          This structure define the parameters CNN/Deep learning net
*          in TIDL
*  @param  handle
*          Call back for handling MOMENTA TIDL output data
*/
typedef struct {
    unsigned int currCoreId;
    unsigned int currLayersGroupId;
    void *pNetwork;
    hal_modl_output_cb handle;
} HAL_MODL_Network;

/**
*  @brief      Initialize MOMENTA TIDL.
*
*  @param[in]  NONE
*
*  @retval     0 : success
*  @retval     others : failed
*
*/
int hal_modl_init_process_level(void);

/**
*  @brief      NOP
*
*  @param[in]  NONE
*
*  @retval     0 : success
*  @retval     others : failed
*
*/
int hal_modl_init_thread_level(void);

/**
*  @brief      Deinitialize MOMENTA TIDL.
*
*  @param[in]  NONE
*
*  @retval     0 : success
*  @retval     others : failed
*
*/
int hal_modl_deinit_process_level(void);

/**
*  @brief      NOP
*
*  @param[in]  NONE
*
*  @retval     0 : success
*  @retval     others : failed
*
*/
int hal_modl_deinit_thread_level(void);

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
    );

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
    );

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
    );

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
    );

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
    );

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
    );

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
    );

#ifdef __cplusplus
}
#endif // __cplusplus

#endif //_HAL_MODL_H_
