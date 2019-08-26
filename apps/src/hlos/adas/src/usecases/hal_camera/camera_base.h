/******************************************************************************

Copyright (c) [2018 - 2019] MOMENTA Incorporated. All rights reserved.

******************************************************************************/


/*******************************************************************************
*
* 文件: camera_base.h
*
* 描述: 该文件提供HAL CAMERA模块非对外开放的信息，包含宏，TYPEDEF,接口声明
*
******************************************************************************/

#ifndef __CAMERA_BASE_H__
#define __CAMERA_BASE_H__

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "hal_camera.h"
#include <osa_mem_shared.h>
#include <src/hlos/adas/include/chains.h>
#include <src/hlos/common/chains_common.h>
#include <src/hlos/system/system_gbm_allocator.h>
#include "src/hlos/adas/src/usecases/hal_arch/hal_arch.h"
#include "src/hlos/adas/src/usecases/hal_multi_camera_display/chains_hal_multi_camera_display.h"
#include "src/hlos/adas/src/usecases/hal_single_camera_display/chains_hal_single_camera_display.h"
#include "src/hlos/adas/src/usecases/hal_multi_camera_save_local/chains_hal_multi_camera_save_local.h"
#include "src/hlos/adas/src/usecases/hal_single_camera_save_local/chains_hal_single_camera_save_local.h"
#include "src/hlos/adas/src/usecases/hal_single_cam_vpe_resize_save/chains_hal_single_cam_vpe_resize_save.h"
#include "src/hlos/adas/src/usecases/hal_single_cam_vpe_sad/chains_hal_single_cam_vpe_sad.h"
#include "src/hlos/adas/src/usecases/hal_offline_cam_vpe_sad/chains_hal_offline_cam_vpe_sad.h"
#include "src/hlos/adas/src/usecases/hal_offline_feed_cam_vpe_sad/chains_hal_offline_feed_cam_vpe_sad.h"


/*******************************************************************************
 *  DEFINES
 *******************************************************************************/
#define HAL_BUF_MAX_QUE_SIZE       (6)
#define HAL_GET_FRAME_TIMEOUT      (0x00)

/*******************************************************************************
 *  结构体：_hal_frame_t
 *  描述：定义HAL CAM 帧数据传输时用的结构体变量类型
 *  成员：
		input_count:生产帧数据的计数
		output_count:消费帧数据的计数
		frame_data：帧数据缓冲buffer区域
 *  备注：该结构用户两个任务处理帧数据时的通信机制，实际是一个循环buffer的缓冲区
 *******************************************************************************/
typedef struct hal_frame {
	int input_count;
	int output_count;
	hal_frame_data_t frame_data[HAL_BUF_MAX_QUE_SIZE];
}_hal_frame_t;


/*******************************************************************************
 *  结构体：_camera_save_local_obj
 *  描述：定义HAL CAM 高级抽闲结构体变量类型
 *  成员：
		hal_multi_camera_save_localAppObj:多路camera usecase 结构体指针
		hal_single_camera_save_local_AppObj:单路camera usecase 结构体指针
		hal_single_cam_vpe_resize_save_AppObj:单路camera vpe resize usecase 结构体指针
		outPutFrameRate:输出帧率
		outPutColorFormat:输出颜色格式
		outPutColorFormatCopyLen:输出颜色格式对应的数据长度
		outVpeResizeColorCopyLen[8]：vpe resize输出颜色格式对应的数据长度
		hal_camera_frame_file_suffix[5]：输出颜色格式的后缀名
		outVpeResizeColorSuffix[8][5]：vpe resize 输出颜色格式的后缀名
		resize_chnl_num：使用vpe resize的通道个数
		resize_config[8]：缩放配置结构体数组
		mutex:互斥锁
		cond:条件变量
 *  备注：该结构主要是为hal_camera_init服务
 *******************************************************************************/
typedef struct {
	Chains_hal_multi_camera_save_localAppObj  *hal_multi_camera_save_localAppObj;
	Chains_hal_single_camera_save_localAppObj *hal_single_camera_save_local_AppObj;
	Chains_hal_single_cam_vpe_resize_saveAppObj *hal_single_cam_vpe_resize_save_AppObj;
	Chains_hal_single_cam_vpe_sadAppObj *hal_single_cam_vpe_sad_AppObj;
	Chains_hal_offline_cam_vpe_sadAppObj *hal_offline_cam_vpe_sad_AppObj;
	Chains_hal_offline_feed_cam_vpe_sadAppObj *hal_offline_feed_cam_vpe_sad_AppObj;
	UInt32  outPutFrameRate;
	UInt32  outPutColorFormat;
	float  outPutColorFormatCopyLen;
	float  outVpeResizeColorCopyLen[HAL_CAMERA_VPE_RESIZE_MAX_NUM];
	UInt8 resize_chnl_num;
	hal_frame_resize_t resize_config[HAL_CAMERA_VPE_RESIZE_MAX_NUM];
	pthread_mutex_t mutex[4];
	pthread_cond_t cond[4];
	char *offlineDataFileName;
	unsigned long long offlineDataIndex;
	UInt8 hdmi_camera_flag;
	UInt8 encode_flag;
} _camera_save_local_obj;


/*******************************************************************************
 *  externs
 *******************************************************************************/
extern _camera_save_local_obj g_camera_save_local_obj;


#endif /*__CAMERA_BASE_H__*/
