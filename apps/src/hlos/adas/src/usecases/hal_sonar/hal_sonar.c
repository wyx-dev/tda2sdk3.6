/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************/
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <netinet/in.h>
#include <src/hlos/adas/include/chains.h>
#include <src/hlos/common/chains_common.h>
#include "hal_sonar.h"


static void *_sonar_read(void *self)
{
	int j = 0;
	ssize_t nbytes = 0;
	hal_can_msg_t msg = {0};
	hal_sonar_t *sonar = (hal_sonar_t *)self;
	hal_sonar_data_t *data = &sonar->data;

	while (1)
	{
		nbytes = hal_can_read_msg(sonar->can, &msg, 1);
		if (nbytes < 0)
		{
			hal_can_deinit(sonar->can);
		}
		else if (nbytes < 1)
		{
			//do nothing;
		}
		else
		{
			for(j = 0; j < nbytes; j++)
			{
				hal_can_read_msg(sonar->can, &msg, 1);
				arch_rwlock_wrlock(&sonar->data_rw_lock);
				if (msg.can_id == 0xd5)
				{
					data->fol_cm = (uint16_t)msg.data[0] << 8 | msg.data[1];
					data->fcl_cm = (uint16_t)msg.data[2] << 8 | msg.data[3];
					data->fcr_cm = (uint16_t)msg.data[4] << 8 | msg.data[5];
					data->for_cm = (uint16_t)msg.data[6] << 8 | msg.data[7];
					data->fol_ts = msg.timestamp;
					data->fcl_ts = msg.timestamp;
					data->fcr_ts = msg.timestamp;
					data->for_ts = msg.timestamp;
				}
				else if (msg.can_id == 0xd4)
				{
					data->rol_cm = (uint16_t)msg.data[0] << 8 | msg.data[1];
					data->rcl_cm = (uint16_t)msg.data[2] << 8 | msg.data[3];
					data->rcr_cm = (uint16_t)msg.data[4] << 8 | msg.data[5];
					data->ror_cm = (uint16_t)msg.data[6] << 8 | msg.data[7];
					data->rol_ts = msg.timestamp;
					data->rcl_ts = msg.timestamp;
					data->rcr_ts = msg.timestamp;
					data->ror_ts = msg.timestamp;
				}
				else if (msg.can_id == 0xd2)
				{
					data->sfl_cm = (uint16_t)msg.data[0] << 8 | msg.data[1];
					data->sfr_cm = (uint16_t)msg.data[2] << 8 | msg.data[3];
					data->srl_cm = (uint16_t)msg.data[4] << 8 | msg.data[5];
					data->srr_cm = (uint16_t)msg.data[6] << 8 | msg.data[7];
					data->sfl_ts = msg.timestamp;
					data->sfr_ts = msg.timestamp;
					data->srl_ts = msg.timestamp;
					data->srr_ts = msg.timestamp;
				}
				arch_rwlock_unlock(&sonar->data_rw_lock);
			}
		}
		usleep(10000);
	}

	return NULL;
}


/*******************************************************************************
 *  函数名：hal_sonar_init
 *  描述：hal层打开声呐设备时申请资源以及配置初始化参数
 *  输入：
 *      sonar_dev：声呐设备结构体
 * 		can_dev：canbus总线结构体
 *  输出：无
 *  返回：成功返回HAL_SONAR_OK, 失败返回HAL_SONAR_EFAIL
 *  备注：无
 *******************************************************************************/
int hal_sonar_init(hal_sonar_t *sonar_dev, hal_can_t *can_dev)
{

	assert(sonar_dev != NULL);
	assert(can_dev != NULL);

	if(can_dev->protocol.read == NULL)
	{
		return HAL_SONAR_EFAIL;
	}
	else
	{
		sonar_dev->can = can_dev;
		arch_rwlock_init(&sonar_dev->data_rw_lock, NULL);
		arch_thread_create(&sonar_dev->read_thread, NULL, _sonar_read, sonar_dev);
	}

	return HAL_SONAR_OK;
}


/*******************************************************************************
 *  函数名：hal_sonar_get_data
 *  描述：hal层打开声呐设备时申请资源以及配置初始化参数
 *  输入：
 *      sonar_dev：声呐设备结构体
 * 		sonar_params：声呐设备配置参数（暂未启用）
 *  输出：
 * 		sonar_data：获取到的声呐数据
 *  返回：成功返回HAL_SONAR_OK, 失败返回HAL_SONAR_EFAIL
 *  备注：无
 *******************************************************************************/
int hal_sonar_get_data(hal_sonar_t *sonar_dev, hal_sonar_params_t *sonar_params,hal_sonar_data_t *sonar_data)
{
	assert(sonar_dev != NULL);
	assert(sonar_data != NULL);

	arch_rwlock_rdlock(&sonar_dev->data_rw_lock);

	memcpy(sonar_data, &sonar_dev->data, sizeof(hal_sonar_data_t));

	arch_rwlock_unlock(&sonar_dev->data_rw_lock);

	return HAL_SONAR_OK;
}


/*******************************************************************************
 *  函数名：hal_sonar_deinit
 *  描述：hal层关闭声呐设备时申请资源以及配置初始化参数
 *  输入：
 *      sonar：声呐设备结构体
 *  输出：无
 *  返回：成功返回HAL_SONAR_OK, 失败返回HAL_SONAR_EFAIL
 *  备注：无
 *******************************************************************************/
int hal_sonar_deinit(hal_sonar_t* sonar)
{
	assert(sonar != NULL);
	assert(sonar->can != NULL);

	arch_thread_cancel(sonar->read_thread);

	arch_thread_join(sonar->read_thread, NULL);

	arch_rwlock_destroy(&sonar->data_rw_lock);

	return HAL_SONAR_OK;
}
