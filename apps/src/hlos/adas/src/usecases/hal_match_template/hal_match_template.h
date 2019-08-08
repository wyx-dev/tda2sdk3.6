/******************************************************************************
*
* Copyright (C) 2018-2019 Momenta Incorporated - https://www.momenta.ai/
*
* ALL RIGHTS RESERVED
*
******************************************************************************/

/**
----------------------------------------------------------------------------
@file    hal_match_template.h
@brief     This file defines the interface for using MOMENTA
           Match Template algorithm.
@version 0.1 (Nov 2018) : Initial Code
----------------------------------------------------------------------------
*/

#ifndef _HAL_MATCH_TEMPLATE_H_
#define _HAL_MATCH_TEMPLATE_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
*  @brief The handle for the Match Template algorithm
*/
typedef unsigned int hal_match_template_handle_t;

/**
*  @brief The return status of the API.
*/
#define HAL_MT_OK              (0)
#define HAL_MT_EFAIL           (-1)
#define HAL_MT_ETIMEOUT        (-2)
#define HAL_MT_EALLOC          (-3)
#define HAL_MT_EBUSY           (-4)
#define HAL_MT_EINVALID_PARAMS (-5)

/**
*  @struct HAL_MT_OutputBuffer
*  @brief  This structure contains all the MOMENTA Match Template
*          output data parameters.
*  @param  size
*          output buffer size
*  @param  data
*          MOMENTA Match Template output buffer
*/
typedef struct {
    unsigned int size;
    unsigned char *data;
} HAL_MT_OutputBuffer;

/**
*  @enum    eHAL_MT_PROC_ID
*  @brief   This enumerator defines
*           the Process ID of MOMENTA Match Template.
*/
typedef enum {
    HAL_MT_PROC_ID_DSP1_INST0 = 0,
    HAL_MT_PROC_ID_DSP2_INST0 = 1,
    HAL_MT_PROC_ID_MAX
} eHAL_MT_PROC_ID;

/**
*  @enum    eHAL_MT_PROC_STATUS
*  @brief   This enumerator defines
*           the status of the Process.
*/
typedef enum {
    HAL_MT_PROC_STATUS_IDLE = 0,
    HAL_MT_PROC_STATUS_BUSY = 1,
    HAL_MT_PROC_STATUS_MAX
} eHAL_MT_PROC_STATUS;

/**
*  @brief      Call back for handling MOMENTA Match Template output data.
*
*  @param[in]  handle    the handle for the Match Template algorithm
*  @param[in]  output    The MOMENTA Match Template output buffer
*
*  @retval     NONE
*
*/
typedef void (*hal_match_template_output_cb)(
    hal_match_template_handle_t handle,
    HAL_MT_OutputBuffer *output
    );

/**
*  @struct HAL_MT_CreateParams
*  @brief  This structure contains all the parameters
*          which MOMENTA Match Template needs at thread level init time.
*  @param  width
*          Width of the input frame
*  @param  height
*          Height of the input frame
*  @param  pitch
*          Pitch of the input frame
*  @param  patch_width
*          Processing region patch width
*  @param  patch_height
*          Processing region patch height
*  @param  search_width
*          Processing region search width
*  @param  search_height
*          Processing region search height
*  @param  handle
*          Call back for handling MOMENTA Match Template output data
*/
typedef struct {
    unsigned int width;
    unsigned int height;
    unsigned int pitch;
    unsigned int patch_width;
    unsigned int patch_height;
    unsigned int search_width;
    unsigned int search_height;
    hal_match_template_output_cb handle;
} HAL_MT_CreateParams;

/**
*  @brief      Initialize MOMENTA Match Template.
*
*  @param[in]  NONE
*
*  @retval     0 : success
*  @retval     others : failed
*
*/
int hal_match_template_init_process_level(void);

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
    );

/**
*  @brief      Deinitialize MOMENTA Match Template.
*
*  @param[in]  NONE
*
*  @retval     0 : success
*  @retval     others : failed
*
*/
int hal_match_template_deinit_process_level(void);

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
int hal_match_template_malloc_continuous_aligned(
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
int hal_match_template_cache_wb(
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
int hal_match_template_free_continuous_aligned(
    void *data,
    unsigned int size
    );

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
    );

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
    );

#ifdef __cplusplus
}
#endif // __cplusplus

#endif //_HAL_MATCH_TEMPLATE_H_
