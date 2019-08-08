/******************************************************************************

Copyright (c) [2018 - 2019] MOMENTA Incorporated. All rights reserved.

******************************************************************************/


/*******************************************************************************
 *
 * 文件: hal_imu.h
 *
 * 描述: 该文件提供HAL IMU模块的接口函数
 *
 ******************************************************************************/

#ifndef __HAL_IMU_H__
#define __HAL_IMU_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************/
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/time.h>

/*******************************************************************************
*  DEFINES
*  描述：The return status of the HAL IMU API.
*******************************************************************************/
#define HAL_IMU_OK              (0)
#define HAL_IMU_EFAIL           (-1)
#define HAL_IMU_ETIMEOUT        (-2)
#define HAL_IMU_EALLOC          (-3)
#define HAL_IMU_EBUSY           (-4)
#define HAL_IMU_EINVALID_PARAMS (-5)

#define IMU_DATA_MAX (12)
#define IMU_GET_DATA_TIMEOUT      (0)

/*******************************************************************************
 *  结构体：hal_imu_data_t
 *  描述：定义HAL IMU数据结构体变量类型
 *  成员：
        index:顺序号
		buffer:imu数据指针
		len:imu数据的长度
		channel:数据对应的camera通道
		orgtime:原始时间戳
		time:数据对应的时间戳
 *  备注：该结构是从camera拿到每一个IMU数据的抽象
 *******************************************************************************/
typedef struct _hal_imu_data_t{
	union
	{
		unsigned char buffer[IMU_DATA_MAX];
		struct
		{
			unsigned short gyro_x;
			unsigned short gyro_y;
			unsigned short gyro_z;
			unsigned short acc_x;
			unsigned short acc_y;
			unsigned short acc_z;
		};
	};
	unsigned int index;
	unsigned int len;		//imu buf size
	unsigned int channel;	//通道
	unsigned long long orgtime;
	struct timeval time;	//时间戳
}hal_imu_data_t;


/*******************************************************************************
 *  函数指针：HAL_IMUDATA_CB
 *  描述：定义函数指针变量类型
 *  输入：
 *    imudata: 为hal_imu_data_t结构体指针
 *  输出：从底层获取imu数据来初始化 imudata结构成员
 *  返回：无
 *  备注：如果调用者使用自定义callback来接收帧数据，
 *     可以自定义一个该函数指针类型的函数，并在hal_imu_init中注册
 *******************************************************************************/
typedef void (*HAL_IMUDATA_CB)(hal_imu_data_t *imudata);


/*******************************************************************************
 *  函数名：hal_imu_init
 *  描述：IMU初始化函数
 *  输入：无
 *  输出：无
 *  返回：HAL_IMU_OK 表示IMU初始化成功，HAL_IMU_EFAIL表示失败
 *  备注：该函数须在hal_camera_init后调用。
 *******************************************************************************/
int hal_imu_init(int imu_num);


/*******************************************************************************
 *  函数名：hal_imu_deinit
 *  描述：IMU关闭函数
 *  输入：无
 *  输出：无
 *  返回：HAL_IMU_OK 表示关闭IMU成功，HAL_IMU_EFAIL表示失败
 *  备注：一定要和hal_imu_init配套使用
 *******************************************************************************/
int hal_imu_deinit(void);


/*******************************************************************************
 *  函数名：hal_get_imudata
 *  描述：对外的hal层接口,实现hal层获取imu的数据;
 *  输入：
 *    imudata: 为hal_imu_data_t结构体指针
 *    timeout: 为获取帧数据时的等待时间
 *  输出：获取imu数据的状态
 *  返回：成功返回0, 失败返回-1
 *  备注：一般会被调用者的线程函数服务函数调用
 *******************************************************************************/
int hal_get_imu_data(hal_imu_data_t *imudata, unsigned int timeout);


/*******************************************************************************
 *  函数名：hal_set_imu_chan_num
 *  描述：设置重力传感器的通道序号，用于确定读取那个通道的IMU数据
 *  输入：
 *    num: IMU通道序号
 *  输出：无
 *  返回：成功返回零，失败返回小于零
 *  备注：无
 *******************************************************************************/
int hal_set_imu_chan_num(int num);


#ifdef __cplusplus
}
#endif // __cplusplus

#endif //__HAL_IMU_H__

