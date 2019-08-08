/******************************************************************************
*
* Copyright (C) 2019-2020 Momenta Incorporated - https://www.momenta.ai/
*
* ALL RIGHTS RESERVED
*
******************************************************************************/

/**
----------------------------------------------------------------------------
@file    hal_match_template_classic.h
@brief     This file defines the interface for using MOMENTA
           Match Template Classic algorithm.
@version 0.1 (Nov 2018) : Initial Code
----------------------------------------------------------------------------
*/

#ifndef _HAL_MATCH_TEMPLATE_CLASSIC_H_
#define _HAL_MATCH_TEMPLATE_CLASSIC_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
*  @brief The handle for the Match Template Classic algorithm
*/
typedef unsigned int hal_match_template_classic_handle_t;

/**
*  @brief The return status of the API.
*/
#define HAL_MTC_OK              (0)
#define HAL_MTC_EFAIL           (-1)
#define HAL_MTC_ETIMEOUT        (-2)
#define HAL_MTC_EALLOC          (-3)
#define HAL_MTC_EBUSY           (-4)
#define HAL_MTC_EINVALID_PARAMS (-5)

#define MAX_RECT_NUM         (6)
#define ORIGIN_IMG_WIDTH     (1280)
#define ORIGIN_IMG_HEIGHT    (720)

#define MAX_TEMPL_SHORT_EDGE      (48)
#define MAX_TEMPL_LONG_EDGE       (64)
#define MAX_SEARCH_SIZE_RATIO     (9)
#define SEARCH_IMG_WIDTH_ALIGN    (4)

#define MAX_SEARCH_IMG_SIZE    (MAX_TEMPL_LONG_EDGE*MAX_TEMPL_SHORT_EDGE*MAX_SEARCH_SIZE_RATIO)
#define MAX_OUT_SCORE_SIZE  (MAX_TEMPL_LONG_EDGE*2+1)*(MAX_TEMPL_SHORT_EDGE*2+1)
#define SCRACH_BUFFER_SIZE  (MAX_OUT_SCORE_SIZE*20+40)
#define TOTAL_L2_BUFFER    (MAX_SEARCH_IMG_SIZE + MAX_OUT_SCORE_SIZE*sizeof(float) + MAX_TEMPL_LONG_EDGE * MAX_TEMPL_SHORT_EDGE * 3)

typedef enum
{
    MODE_W32H64_JUMP = 0x0,
    MODE_W48H48_JUMP = 0x1,
    MODE_W64H48_JUMP = 0x2,

    MODE_W32H64_NOJUMP = 0x10,
    MODE_W48H48_NOJUMP = 0x11,
    MODE_W64H48_NOJUMP = 0x12
}MatchMode;

typedef struct
{
  int left;
  int top;
  int right;
  int bottom;
}Rect;

typedef struct
{
  uint8_t* pre_image;
  uint8_t* cur_image;
  int pad_num;
  Rect templ_pad[MAX_RECT_NUM];
  Rect cur_pad[MAX_RECT_NUM];
  int match_mode[MAX_RECT_NUM];
}MtInParam;

typedef struct
{
  Rect new_rect;
  float value;
}MatchResult;

typedef struct
{
  int result_num;
  MatchResult result[MAX_RECT_NUM];
}MtOutResult;

/**
*  @struct HAL_MTC_OutputBuffer
*  @brief  This structure contains all the MOMENTA Match Template Classic
*          output data parameters.
*  @param  size
*          output buffer size
*  @param  data
*          MOMENTA Match Template Classic output buffer
*/
typedef struct {
    unsigned int size;
    unsigned char *data;
} HAL_MTC_OutputBuffer;

/**
*  @enum    eHAL_MTC_PROC_ID
*  @brief   This enumerator defines
*           the Process ID of MOMENTA Match Template Classic.
*/
typedef enum {
    HAL_MTC_PROC_ID_DSP1_INST0 = 0,
    HAL_MTC_PROC_ID_DSP2_INST0 = 1,
    HAL_MTC_PROC_ID_MAX
} eHAL_MTC_PROC_ID;

/**
*  @enum    eHAL_MTC_PROC_STATUS
*  @brief   This enumerator defines
*           the status of the Process.
*/
typedef enum {
    HAL_MTC_PROC_STATUS_IDLE = 0,
    HAL_MTC_PROC_STATUS_BUSY = 1,
    HAL_MTC_PROC_STATUS_MAX
} eHAL_MTC_PROC_STATUS;

/**
*  @brief      Call back for handling MOMENTA Match Template Classic output data.
*
*  @param[in]  handle    the handle for the Match Template Classic algorithm
*  @param[in]  output    The MOMENTA Match Template Classic output buffer
*
*  @retval     NONE
*
*/
typedef void (*hal_match_template_classic_output_cb)(
    hal_match_template_classic_handle_t handle,
    HAL_MTC_OutputBuffer *output
    );

/**
*  @struct HAL_MTC_CreateParams
*  @brief  This structure contains all the parameters
*          which MOMENTA Match Template Classic needs at thread level init time.
*  @param  handle
*          Call back for handling MOMENTA Match Template Classic output data
*/
typedef struct {
    hal_match_template_classic_output_cb handle;
} HAL_MTC_CreateParams;

/**
*  @struct HAL_MTC_MatchParam
*  @brief  This structure contains all the parameters
*          which match template classic algorithm needs at runtime.
*  @param  imgWidth
*          original input image width
*  @param  imgHeight
*          original input image height
*  @param  imgPitch
*          original input image pitch
*  @param  tempImgWidth
*          template image width
*  @param  tempImgHeight
*          template image height
*  @param  tempImgPitch
*          template image pitch
*  @param  scaledTempImgVar
*          template image variance
*  @param  method
*          method use in template matching, must be 0 for now
*  @param  outScorePitch
*          pitch of outScore buffer (elements, not bytes)
*  @param  xDirJump
*          jump in x direction while template search in original input image
*  @param  yDirJump
*          jump in y direction while template search in original input image
*  @param  reserve
*          reserve for alignment
*/
typedef struct {
    int imgWidth;
    int imgHeight;
    int imgPitch;
    int tempImgWidth;
    int tempImgHeight;
    int tempImgPitch;
    float scaledTempImgVar;
    int method;
    int outScorePitch;
    unsigned char xDirJump;
    unsigned char yDirJump;
    unsigned char reserve[2];
} HAL_MTC_MatchParam;

/**
*  @brief      Initialize MOMENTA Match Template Classic.
*
*  @param[in]  NONE
*
*  @retval     0 : success
*  @retval     others : failed
*
*/
int hal_match_template_classic_init_process_level(void);

/**
*  @brief      Create MOMENTA Match Template Classic instance.
*              MOMENTA Match Template Classic Links and Chains framework is started.
*
*  @param[out] handle          the handle for MOMENTA Match Template Classic
*  @param[in]  inst_id         the id of the instance
*  @param[in]  init_prm        init parameters
*  @param[in]  prm_size        parameters size
*
*  @retval     0 : success
*  @retval     others : failed
*
*/
int hal_match_template_classic_init_thread_level(
    hal_match_template_classic_handle_t *handle,
    unsigned int inst_id,
    void *init_prm,
    unsigned int prm_size
    );

/**
*  @brief      Deinitialize MOMENTA Match Template Classic.
*
*  @param[in]  NONE
*
*  @retval     0 : success
*  @retval     others : failed
*
*/
int hal_match_template_classic_deinit_process_level(void);

/**
*  @brief      Destroy MOMENTA Match Template Classic instance.
*              MOMENTA Match Template Classic Links and Chains framework is deleted.
*
*  @param[in]  handle  the handle for MOMENTA Match Template Classic
*
*  @retval     0 : success
*  @retval     others : failed
*
*/
int hal_match_template_classic_deinit_thread_level(
    hal_match_template_classic_handle_t handle
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
int hal_match_template_classic_malloc_continuous_aligned(
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
int hal_match_template_classic_cache_wb(
    void *data,
    unsigned int size
    );


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
unsigned int hal_match_template_classic_virt2phys(
    void *virt_addr
    );


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
int hal_match_template_classic_cache_inv(
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
int hal_match_template_classic_free_continuous_aligned(
    void *data,
    unsigned int size
    );

/**
*  @brief      Pass new data to MOMENTA Match Template Classic.
*
*  @param[in]  handle  the handle for the Match Template Classic algorithm
*  @param[in]  data    the new data to be processed.
*
*  @retval     0 : success
*  @retval     others : failed
*
*  @remarks
*   Please malloc continuous aligned physical memory for
*   param[in] data by hal_match_template_classic_malloc_continuous_aligned.
*
*  @remarks
*   Do not release or modify param[in] data until get the output buffer.
*
*/
int hal_match_template_classic_put_input_buffer(
    hal_match_template_classic_handle_t handle,
    void *data
    );

/**
*  @brief      Wait until output buffer is ready
*
*  @param[in]  handle    the handle for the Match Template Classic algorithm
*
*  @retval     0 : success
*  @retval     others : failed
*
*  @remarks
*   When output buffer is ready hal_match_template_classic_output_cb will be called.
*
*/
int hal_match_template_classic_wait_output_buffer(
    hal_match_template_classic_handle_t handle
    );

#ifdef __cplusplus
}
#endif // __cplusplus

#endif //_HAL_MATCH_TEMPLATE_CLASSIC_H_
