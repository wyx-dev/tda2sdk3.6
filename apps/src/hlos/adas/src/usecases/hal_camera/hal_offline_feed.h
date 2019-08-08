/******************************************************************************

Copyright (c) [2018 - 2019] MOMENTA Incorporated. All rights reserved.

******************************************************************************/


/*******************************************************************************
 *
 * 文件: hal_camera.h
 *
 * 描述: 该文件提供HAL CAMERA模块的接口函数以及相应的结构体变量类型
 *
 ******************************************************************************/

#ifndef __HAL_OFFLINE_FEED_H__
#define __HAL_OFFLINE_FEED_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
#include "../hal_can/hal_can.h"

#define HAL_FEED_OK                 0
#define HAL_FEED_ERROR              -1
#define HAL_FEED_NEED_LOAD_IMAGES   -2
#define HAL_FEED_CAN_FILE_END       -3

typedef enum _hal_offline_feed_framerate_e
{
	HAL_FEED_FRAMERATE_15FPS = 0,
	HAL_FEED_FRAMERATE_20FPS,
	HAL_FEED_FRAMERATE_25FPS,
	HAL_FEED_FRAMERATE_30FPS,
	HAL_FEED_FRAMERATE_INVALID
}hal_offline_feed_framerate_e;

int hal_cam_offline_feed_pause_signal(void);
int hal_cam_offline_feed_restart_signal(void);

int hal_offline_feed_cam_init(const char *file_name, int group_index, int channel_num, int framerate, hal_frame_format_e colorformat, HAL_FRMDATA_CB func);
int hal_offline_feed_cam_deinit();
void * hal_offline_feed_image_process(void* ptr);

int hal_offline_feed_can_read_msg(hal_can_t *can, hal_can_msg_t *msg, int n);

void hal_create_feed_thread();

#ifdef __cplusplus
}
#endif // __cplusplus

#endif //__HAL_OFFLINE_FEED_H__

