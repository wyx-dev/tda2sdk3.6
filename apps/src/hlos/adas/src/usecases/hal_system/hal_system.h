/******************************************************************************

Copyright (c) [2018 - 2019] MOMENTA Incorporated. All rights reserved.

******************************************************************************/


/*******************************************************************************
 *
 * 文件: hal_system.h
 *
 * 描述: 该文件提供HAL SYSTEM模块的接口函数
 *
 ******************************************************************************/

#ifndef __HAL_SYSTEM_H__
#define __HAL_SYSTEM_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


/*******************************************************************************
*  DEFINES
*  描述：The return status of the HAL SYSTEM API.
*******************************************************************************/
#define HAL_SYS_OK              (0)
#define HAL_SYS_EFAIL           (-1)
#define HAL_SYS_ETIMEOUT        (-2)
#define HAL_SYS_EALLOC          (-3)
#define HAL_SYS_EBUSY           (-4)
#define HAL_SYS_EINVALID_PARAMS (-5)

#define HAL_SYS_CORE_NAME_MAX (10)

/*******************************************************************************
 *  枚举：hal_sys_process_id_e
 *  描述：该枚举成员表示配置系统处理器核心类型的序号，A15 IPU DSP EVE等
 *  备注：无
 *******************************************************************************/
typedef enum _hal_sys_process_id_e
{
	HAL_SYS_PROC_IPU_0 = 0,  //IPU1_0 is master
	HAL_SYS_PROC_IPU_1,
	HAL_SYS_PROC_IPU_2,
	HAL_SYS_PROC_IPU_3,
	HAL_SYS_PROC_IPU_4,
	HAL_SYS_PROC_IPU_5,
	HAL_SYS_PROC_IPU_6,
	HAL_SYS_PROC_IPU_7,

	HAL_SYS_PROC_CPU_0,
	HAL_SYS_PROC_CPU_1,
	HAL_SYS_PROC_CPU_2,
	HAL_SYS_PROC_CPU_3,
	HAL_SYS_PROC_CPU_4,
	HAL_SYS_PROC_CPU_5,
	HAL_SYS_PROC_CPU_6,
	HAL_SYS_PROC_CPU_7,

	HAL_SYS_PROC_DSP_0,
	HAL_SYS_PROC_DSP_1,
	HAL_SYS_PROC_DSP_2,
	HAL_SYS_PROC_DSP_3,
	HAL_SYS_PROC_DSP_4,
	HAL_SYS_PROC_DSP_5,
	HAL_SYS_PROC_DSP_6,
	HAL_SYS_PROC_DSP_7,

	HAL_SYS_PROC_EVE_0,
	HAL_SYS_PROC_EVE_1,
	HAL_SYS_PROC_EVE_2,
	HAL_SYS_PROC_EVE_3,
	HAL_SYS_PROC_EVE_4,
	HAL_SYS_PROC_EVE_5,
	HAL_SYS_PROC_EVE_6,
	HAL_SYS_PROC_EVE_7,

	HAL_SYS_PROC_MAX,
	HAL_SYS_PROC_INVALID,
} hal_sys_process_id_e;


/*******************************************************************************
 *	结构体：hal_system_perf_t
 *	描述：定义CPU使用率结构体
 *	成员：
		integer_value:百分数的整数部分
		fractional_value:百分数的小数部分
		core_name:处理器名称
		used:是否使用
 *	备注：
 *******************************************************************************/
typedef struct _hal_system_perf_t
{
	unsigned int integer_value;
	unsigned int fractional_value;
	char core_name[HAL_SYS_CORE_NAME_MAX];
	char used;
} hal_system_perf_t;


/*******************************************************************************
 *  结构体：hal_system_perf_all_t
 *  描述：定义所有CPU使用率结构体
 *  成员：
		perf:cpu使用率数组
 *  备注：
 *******************************************************************************/
typedef struct _hal_system_perf_all_t
{
	hal_system_perf_t perf[HAL_SYS_PROC_MAX];
} hal_system_perf_all_t;


/*******************************************************************************
 *  函数名：hal_system_get_perf
 *  描述：获取一个处理器的使用率
 *  输入：
 *		processer_id:处理器的ID
 *		perf:CPU使用率结构体指针
 *  输出：无
 *  返回：成功返回0, 失败返回<0
 *  备注：无
 *******************************************************************************/
int hal_system_get_perf(hal_sys_process_id_e processer_id, hal_system_perf_t *perf);


/*******************************************************************************
 *  hal_system_get_perf_all
 *  描述：获取系统所有处理器的使用率
 *  输入：
 *     perfall:所有CPU使用率结构体指针
 *  输出：无
 *  返回：成功返回0, 失败返回<0
 *  备注：无
 *******************************************************************************/
int hal_system_get_perf_all(hal_system_perf_all_t *perfall);


/*******************************************************************************
 *  hal_system_print_perf_all
 *  描述：打印系统所有处理器的使用率
 *  输入：
 *     perfall:所有CPU使用率结构体指针
 *  输出：无
 *  返回：无
 *  备注：无
 *******************************************************************************/
void hal_system_print_perf_all(hal_system_perf_all_t *perfall);

/*******************************************************************************
 *  hal_system_print_statistics_collector
 *  描述：打印系统内存吞吐率
 *  输入：无
 *  输出：无
 *  返回：无
 *  备注：无
 *******************************************************************************/
void hal_system_print_statistics_collector(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif //__HAL_SYSTEM_H__

