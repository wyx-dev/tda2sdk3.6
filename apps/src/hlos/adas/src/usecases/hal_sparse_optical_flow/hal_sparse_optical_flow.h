/******************************************************************************
*
* Copyright (C) 2018-2019 Momenta Incorporated - https://www.momenta.ai/
*
* ALL RIGHTS RESERVED
*
******************************************************************************/

/**
----------------------------------------------------------------------------
@file    hal_sparse_optical_flow.h
@brief     This file defines the interface for using MOMENTA
           Sparse Optical Flow algorithm.
@version 0.1 (Nov 2018) : Initial Code
----------------------------------------------------------------------------
*/

#ifndef _HAL_SPARSE_OPTICAL_FLOW_H_
#define _HAL_SPARSE_OPTICAL_FLOW_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
*  @brief The handle for the Sparse Optical Flow algorithm
*/
typedef unsigned int hal_sparse_optical_flow_handle_t;

/**
*  @brief The return status of the API.
*/
#define HAL_SOF_OK              (0)
#define HAL_SOF_EFAIL           (-1)
#define HAL_SOF_ETIMEOUT        (-2)
#define HAL_SOF_EALLOC          (-3)
#define HAL_SOF_EBUSY           (-4)
#define HAL_SOF_EINVALID_PARAMS (-5)

/**
*  @struct HAL_SOF_OutputBuffer
*  @brief  This structure contains all the MOMENTA Sparse Optical Flow
*          output data parameters.
*  @param  size
*          output buffer size
*  @param  data
*          MOMENTA Sparse Optical Flow output buffer
*/
typedef struct {
    unsigned int size;
    unsigned char *data;
} HAL_SOF_OutputBuffer;

/**
*  @enum    eHAL_SOF_PROC_ID
*  @brief   This enumerator defines
*           the Process ID of MOMENTA Sparse Optical Flow.
*/
typedef enum {
    HAL_SOF_PROC_ID_DSP1_INST0 = 0,
    HAL_SOF_PROC_ID_DSP2_INST0 = 1,
    HAL_SOF_PROC_ID_MAX
} eHAL_SOF_PROC_ID;

/**
*  @enum    eHAL_SOF_PROC_STATUS
*  @brief   This enumerator defines
*           the status of the Process.
*/
typedef enum {
    HAL_SOF_PROC_STATUS_IDLE = 0,
    HAL_SOF_PROC_STATUS_BUSY = 1,
    HAL_SOF_PROC_STATUS_MAX
} eHAL_SOF_PROC_STATUS;

/**
*  @brief      Call back for handling MOMENTA Sparse Optical Flow output data.
*
*  @param[in]  handle    the handle for the Sparse Optical Flow algorithm
*  @param[in]  output    The MOMENTA Sparse Optical Flow output buffer
*
*  @retval     NONE
*
*/
typedef void (*hal_sparse_optical_flow_output_cb)(
    hal_sparse_optical_flow_handle_t handle,
    HAL_SOF_OutputBuffer *output
    );

/**
*  @struct HAL_SOF_CreateParams
*  @brief  This structure contains all the parameters
*          which MOMENTA Sparse Optical Flow needs at thread level init time.
*  @param  startX
*          Start X of actual video relative to start of input buffer
*  @param  startY
*          Start Y of actual video relative to start of input buffer
*  @param  width
*          Width of the input frame
*  @param  height
*          Height of the input frame
*  @param  handle
*          Call back for handling MOMENTA Sparse Optical Flow output data
*/
typedef struct {
    unsigned int startX;
    unsigned int startY;
    unsigned int width;
    unsigned int height;
    hal_sparse_optical_flow_output_cb handle;
} HAL_SOF_CreateParams;

/**
*  @brief      Initialize MOMENTA Sparse Optical Flow.
*
*  @param[in]  NONE
*
*  @retval     0 : success
*  @retval     others : failed
*
*/
int hal_sparse_optical_flow_init_process_level(void);

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
    );

/**
*  @brief      Deinitialize MOMENTA Sparse Optical Flow.
*
*  @param[in]  NONE
*
*  @retval     0 : success
*  @retval     others : failed
*
*/
int hal_sparse_optical_flow_deinit_process_level(void);

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
    );

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
int hal_sparse_optical_flow_cache_wb(
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
int hal_sparse_optical_flow_free_continuous_aligned(
    void *data,
    unsigned int size
    );

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
    );

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
    );

#ifdef __cplusplus
}
#endif // __cplusplus

#endif //_HAL_SPARSE_OPTICAL_FLOW_H_
