/******************************************************************************

Copyright (c) [2018 - 2019] MOMENTA Incorporated. All rights reserved.

******************************************************************************/


/*******************************************************************************
 *
 * 文件: hal_imu.c
 *
 * 描述: 该文件提供HAL IMU模块的接口函数
 *
 ******************************************************************************/


/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************/
#include "hal_imu.h"
#include "hal_imu_priv.h"

#define IMU_BUF_MAX_QUE_SIZE   (64)


/*******************************************************************************
 *  结构体：_hal_imu_t
 *  描述：定义HAL IMU数据传输时用的结构体变量类型
 *  成员：
		input_count:生产imu数据的计数
		output_count:消费imu数据的计数
		frame_data：imu数据缓冲buffer区域
 *  备注：该结构用户两个任务处理帧数据时的通信机制，实际是一个循环buffer的缓冲区
 *******************************************************************************/
typedef struct hal_imu {
	int input_count;
	int output_count;
	hal_imu_data_t imu_data[IMU_BUF_MAX_QUE_SIZE];
}_hal_imu_t;


/*******************************************************************************
 *  Globals
 *******************************************************************************/
static unsigned int g_sensor_num = 0;
static _hal_imu_t g_imu_data[4];
unsigned int g_imu_num = 0;
HAL_IMUDATA_CB g_imu_cb = NULL;
pthread_mutex_t imu_mutex[4];
pthread_cond_t	imu_cond[4];


static void _get_imudata_from_null(hal_imu_data_t *imudata)
{
	pthread_mutex_lock(&imu_mutex[imudata->channel]);
	memcpy(&g_imu_data[imudata->channel].imu_data[g_imu_data[imudata->channel].input_count++%IMU_BUF_MAX_QUE_SIZE],imudata,sizeof(hal_imu_data_t));
	pthread_cond_signal(&imu_cond[imudata->channel]);
	pthread_mutex_unlock(&imu_mutex[imudata->channel]);
}


/*******************************************************************************
 *  函数名：hal_set_imu_chan_num
 *  描述：设置重力传感器的通道序号，用于确定读取那个通道的IMU数据
 *  输入：
 *    num: IMU通道序号
 *  输出：无
 *  返回：成功返回零，失败返回小于零
 *  备注：无
 *******************************************************************************/
int hal_set_imu_chan_num(int num)
{
	if(num > g_sensor_num)
	{
		Vps_printf("\r\n### hal_set_imu_channum ### The imu channel number is out of range !!!\r\n");
		return HAL_IMU_EFAIL;
	}

	g_imu_num = num;

	System_linkControl(SYSTEM_LINK_ID_APP_CTRL,
									APP_CTRL_LINK_CMD_IMU_SET_NUM,
									&g_imu_num,
									sizeof(int),
									TRUE);
	return HAL_IMU_OK;
}


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
int hal_get_imu_data(hal_imu_data_t *imudata, unsigned int timeout)
{
	struct timeval now;
	struct timespec abstime;

	if(imudata == NULL)
	{
		Vps_printf("\r\n### hal_get_imudata ### param imudata NULL!!!\r\n");
		return HAL_IMU_EFAIL;
	}

	if(imudata->channel > (g_imu_num-1))
	{
		Vps_printf("\r\n### hal_get_imudata ### channel=%d out of range !!!\r\n", imudata->channel);
		return HAL_IMU_EFAIL;
	}

	gettimeofday(&now, NULL);
	int nsec = now.tv_usec * 1000 + (timeout % 1000) * 1000000;
	abstime.tv_nsec = nsec % 1000000000;
	abstime.tv_sec = now.tv_sec + nsec / 1000000000 + timeout / 1000;

	pthread_mutex_lock(&imu_mutex[imudata->channel]);

	/*
	*  如果timeout等于 0表示永久阻塞方式
	*/
	if(IMU_GET_DATA_TIMEOUT == timeout)
	{
		pthread_cond_wait(&imu_cond[imudata->channel], &imu_mutex[imudata->channel]);
	}
	else
	{
		pthread_cond_timedwait(&imu_cond[imudata->channel], &imu_mutex[imudata->channel], &abstime);
	}

	if(g_imu_data[imudata->channel].output_count != g_imu_data[imudata->channel].input_count)
	{
			if(g_imu_data[imudata->channel].input_count - g_imu_data[imudata->channel].output_count >= IMU_BUF_MAX_QUE_SIZE)
			{
				Vps_printf("\r\n### hal_get_imudata ### OVER FLOW %d \r\n",g_imu_data[imudata->channel].input_count - g_imu_data[imudata->channel].output_count);
				g_imu_data[imudata->channel].output_count = g_imu_data[imudata->channel].input_count - IMU_BUF_MAX_QUE_SIZE + 2;
			}

			imudata->len = g_imu_data[imudata->channel].imu_data[g_imu_data[imudata->channel].output_count%IMU_BUF_MAX_QUE_SIZE].len;
			imudata->time.tv_sec = g_imu_data[imudata->channel].imu_data[g_imu_data[imudata->channel].output_count%IMU_BUF_MAX_QUE_SIZE].time.tv_sec;
			imudata->time.tv_usec = g_imu_data[imudata->channel].imu_data[g_imu_data[imudata->channel].output_count%IMU_BUF_MAX_QUE_SIZE].time.tv_usec;
			memcpy(imudata->buffer, g_imu_data[imudata->channel].imu_data[g_imu_data[imudata->channel].output_count%IMU_BUF_MAX_QUE_SIZE].buffer, imudata->len);
			imudata->orgtime = g_imu_data[imudata->channel].imu_data[g_imu_data[imudata->channel].output_count%IMU_BUF_MAX_QUE_SIZE].orgtime;
			imudata->index = g_imu_data[imudata->channel].imu_data[g_imu_data[imudata->channel].output_count%IMU_BUF_MAX_QUE_SIZE].index;
			g_imu_data[imudata->channel].output_count++;
	}
	else
	{
		Vps_printf("\r\n### hal_get_imudata ### wait channel[%d] timeout !!!\r\n",imudata->channel);
		pthread_mutex_unlock(&imu_mutex[imudata->channel]);
		return HAL_IMU_EFAIL;
	}

	pthread_mutex_unlock(&imu_mutex[imudata->channel]);

	return HAL_IMU_OK;
}

/*******************************************************************************
 *  函数名：hal_imu_init
 *  描述：IMU初始化函数
 *  输入：无
 *  输出：无
 *  返回：HAL_IMU_OK 表示IMU初始化成功，HAL_IMU_EFAIL表示失败
 *  备注：该函数须在hal_camera_init后调用。
 *******************************************************************************/
int hal_imu_init(int imu_num)
{
	int i;
	
	g_sensor_num = imu_num;
	for(i=0; i < imu_num; i++)
	{
		if(pthread_mutex_init(&imu_mutex[i], NULL))
		{
			Vps_printf("\r\n### hal_imu_init ### pthread_mutex_init[%d] Error !\r\n",i);
			return HAL_IMU_EFAIL;
		}

		if(pthread_cond_init(&imu_cond[i], NULL))
		{
			Vps_printf("\r\n### hal_imu_init ### pthread_cond_init[%d] Error !\r\n",i);
			return HAL_IMU_EFAIL;
		}
	}
	memset(g_imu_data,0,sizeof(_hal_imu_t)*4);
	/*Init Hal IMU CallBack Register*/
	g_imu_cb = _get_imudata_from_null;
	hal_set_imu_chan_num(1);

	return HAL_IMU_OK;
}


/*******************************************************************************
 *  函数名：hal_imu_deinit
 *  描述：IMU关闭函数
 *  输入：无
 *  输出：无
 *  返回：HAL_IMU_OK 表示关闭IMU成功，HAL_IMU_EFAIL表示失败
 *  备注：一定要和hal_imu_init配套使用
 *******************************************************************************/
int hal_imu_deinit(void)
{
	//TODO:add something for deinit
	hal_set_imu_chan_num(0);

	return HAL_IMU_OK;
}
