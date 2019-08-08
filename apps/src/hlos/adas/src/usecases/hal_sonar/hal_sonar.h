/******************************************************************************

Copyright (c) [2018 - 2019] MOMENTA Incorporated. All rights reserved.

******************************************************************************/


/*******************************************************************************
 *
 * 文件: hal_sonar.h
 *
 * 描述: 该文件提供HAL SONAR模块的接口函数以及相应的结构体变量类型
 *
 ******************************************************************************/

#ifndef __HAL_SONAR_H__
#define __HAL_SONAR_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************/
#include <stdint.h>
#include <sys/time.h>
#include <pthread.h>
#include "hal_can.h"


/*******************************************************************************
*  DEFINES
*  描述：The return status of the HAL SONAR API.
*******************************************************************************/
#define HAL_SONAR_OK              (0)
#define HAL_SONAR_EFAIL           (-1)
#define HAL_SONAR_ETIMEOUT        (-2)
#define HAL_SONAR_EALLOC          (-3)
#define HAL_SONAR_EBUSY           (-4)
#define HAL_SONAR_EINVALID_PARAMS (-5)

#define arch_thread_create(tid, attr, thread_fun, arg) pthread_create(tid, attr, thread_fun, arg)
#define arch_thread_cancel(tid)                        pthread_cancel(tid)
#define arch_thread_join(tid, ptr)                     pthread_join(tid, ptr)
#define arch_rwlock_init(rwlock, rwattr)               pthread_rwlock_init(rwlock, rwattr)
#define arch_rwlock_rdlock(rwlock)                     pthread_rwlock_rdlock(rwlock)
#define arch_rwlock_unlock(rwlock)                     pthread_rwlock_unlock(rwlock)
#define arch_rwlock_wrlock(rwlock)                     pthread_rwlock_wrlock(rwlock)
#define arch_rwlock_destroy(rwlock)                    pthread_rwlock_destroy(rwlock)


/*******************************************************************************
*  TYPEDEFS
*******************************************************************************/
typedef pthread_t        arch_thread; 
typedef pthread_rwlock_t arch_rwlock_t;


/*******************************************************************************
 *  结构体:hal_sonar_data_t
 *  描述：定义HAL SONAR CAN帧数据结构体变量类型
 *  成员：
		fol_cm: front offcut left   前向左边角雷达
		fcl_cm: front center left   前向左中间雷达
		fcr_cm: front center right  前向右中间雷达
		for_cm: front offcut right  前向右边角雷达
		rol_cm: rear offcut left    后向左边角雷达
		rcl_cm: rear center left    后向左中间雷达
		rcr_cm: rear center right   后向右中间雷达
		ror_cm: rear offcut right   后向右边角雷达
		sfl_cm: side front left     侧面左前雷达
		sfr_cm: side front right    侧面右前雷达
		srl_cm: side rear left      侧面左后雷达
		srr_cm: side rear right     侧面右后雷达
 *  备注：无
 *******************************************************************************/
typedef struct _hal_sonar_data_t{
    unsigned short fol_cm;
    unsigned short fcl_cm;
    unsigned short fcr_cm;
    unsigned short for_cm;
    unsigned short rol_cm;
    unsigned short rcl_cm;
    unsigned short rcr_cm;
    unsigned short ror_cm;
    unsigned short sfl_cm;
    unsigned short sfr_cm;
    unsigned short srl_cm;
    unsigned short srr_cm;
    struct timeval fol_ts;
    struct timeval fcl_ts;
    struct timeval for_ts;
    struct timeval fcr_ts;
    struct timeval rol_ts;
    struct timeval rcl_ts;
    struct timeval ror_ts;
    struct timeval rcr_ts;
    struct timeval sfl_ts;
    struct timeval sfr_ts;
    struct timeval srl_ts;
    struct timeval srr_ts;
}hal_sonar_data_t;


/*******************************************************************************
 *  结构体:hal_sonar_t
 *  描述：定义HAL SONAR 结构体变量类型
 *  成员：
		can: CAN结构指针，用于确定CAN的来源
		data: CAN data CAN数据
		read_thread: 读CAN消息线程
		data_rw_lock: CAN 数据读写锁
 *  备注：无
 *******************************************************************************/
typedef struct _hal_sonar_t{
    hal_can_t *can;
    hal_sonar_data_t data;
    arch_thread read_thread;
    arch_rwlock_t data_rw_lock;
}hal_sonar_t;


/*******************************************************************************
 *  结构体:hal_sonar_params_t
 *  描述：定义HAL SONAR 参数数据结构体变量类型
 *  成员：
		can_msg_id : can 报文 ID
		start_bit: can 报文 起始位
		len: can 报文长度
		is_big_endian: 是否是大端格式
		is_float: 是否是小数
		factor: 系数
		offset: 偏移量 val = data*factor+offset
 *  备注：无
 *******************************************************************************/
typedef struct _hal_sonar_params_t {
    uint16_t can_msg_id; //can 报文 ID
    uint16_t start_bit;  //can 报文 起始位
    uint16_t len;        //can 报文长度
    uint16_t is_big_endian; //是否是大端格式
    uint16_t is_float;   //是否是小数
    float factor;        //系数
    uint16_t offset;     //偏移量 val = data*factor+offset
}hal_sonar_params_t;


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
int hal_sonar_init(hal_sonar_t *sonar_dev, hal_can_t *can_dev);


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
int hal_sonar_get_data(hal_sonar_t *sonar_dev, hal_sonar_params_t *sonar_params,hal_sonar_data_t *sonar_data);


/*******************************************************************************
 *  函数名：hal_sonar_deinit
 *  描述：hal层关闭声呐设备时申请资源以及配置初始化参数
 *  输入：
 *      sonar：声呐设备结构体
 *  输出：无
 *  返回：成功返回HAL_SONAR_OK, 失败返回HAL_SONAR_EFAIL
 *  备注：无
 *******************************************************************************/
int hal_sonar_deinit(hal_sonar_t* sonar);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif //__HAL_SONAR_H__


