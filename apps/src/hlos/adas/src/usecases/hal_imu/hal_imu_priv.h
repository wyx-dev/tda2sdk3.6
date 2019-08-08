/******************************************************************************

Copyright (c) [2018 - 2019] MOMENTA Incorporated. All rights reserved.

******************************************************************************/


/*******************************************************************************
 *
 * 文件: hal_imu_priv.h
 *
 * 描述: 该文件提供HAL IMU模块的私有数据和接口
 *
 ******************************************************************************/

#ifndef __HAL_IMU_PRIV_H__
#define __HAL_IMU_PRIV_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************/
#include <src/hlos/adas/include/chains.h>
#include <src/hlos/common/chains_common.h>


/*******************************************************************************
*  DEFINES
*  描述：The return status of the HAL ARCH API.
*******************************************************************************/
#define IMU_RAW_DATA_MAX       (12)
#define IMU_CHANNEL_NUM_MAX    (6)


/*******************************************************************************
 *  结构体：hal_imu_raw_data_t
 *  描述：定义HAL IMU数据结构体变量类型
 *  成员：
        index:顺序号
		data:imu数据缓冲
		len:imu数据的长度
		channel:数据对应的camera通道
		time:数据对应的时间戳
 *  备注:该结构是从camera拿到每一个IMU数据的抽象
 *******************************************************************************/
typedef struct _hal_imu_raw_data_t{
	unsigned int index;
	unsigned char data[IMU_RAW_DATA_MAX];	//imu data
	unsigned int channel;	//通道
	unsigned long long time;	//时间戳
}hal_imu_raw_data_t;

extern unsigned int g_imu_num;
#endif
