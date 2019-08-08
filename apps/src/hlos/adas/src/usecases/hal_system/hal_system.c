/******************************************************************************

Copyright (c) [2018 - 2019] MOMENTA Incorporated. All rights reserved.

******************************************************************************/


/*******************************************************************************
 *
 * 文件: hal_system.c
 *
 * 描述: 该文件提供HAL SYSTEM模块的接口函数
 *
 ******************************************************************************/


/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************/
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>

#include <src/hlos/osa/include/osa.h>
#include <include/link_api/system.h>
#include <include/link_api/system_common.h>
#include <include/link_api/systemLink_common.h>
#include <links_fw/src/hlos/system/system_pvrscope_if.h>
#include <src/include/link_stats_monitor.h>

#include "hal_system.h"

/*******************************************************************************
 *  Globals
 *******************************************************************************/
Int32 ChainsCommon_statCollectorPrint();

static int init_flag = 0;

static char *ti_system_proc_name[HAL_SYS_PROC_MAX] =
{
	[HAL_SYS_PROC_IPU_0] = "NA",
	[HAL_SYS_PROC_IPU_1] = "NA",
	[HAL_SYS_PROC_IPU_2] = "IPU2",
	[HAL_SYS_PROC_IPU_3] = "NA",
	[HAL_SYS_PROC_IPU_4] = "NA",
	[HAL_SYS_PROC_IPU_5] = "NA",
	[HAL_SYS_PROC_IPU_6] = "NA",
	[HAL_SYS_PROC_IPU_7] = "NA",
	[HAL_SYS_PROC_CPU_0] = "A15",
	[HAL_SYS_PROC_CPU_1] = "NA",
	[HAL_SYS_PROC_CPU_2] = "NA",
	[HAL_SYS_PROC_CPU_3] = "NA",
	[HAL_SYS_PROC_CPU_4] = "NA",
	[HAL_SYS_PROC_CPU_5] = "NA",
	[HAL_SYS_PROC_CPU_6] = "NA",
	[HAL_SYS_PROC_CPU_7] = "NA",
	[HAL_SYS_PROC_DSP_0] = "DSP1",
	[HAL_SYS_PROC_DSP_1] = "DSP2",
	[HAL_SYS_PROC_DSP_2] = "NA",
	[HAL_SYS_PROC_DSP_3] = "NA",
	[HAL_SYS_PROC_DSP_4] = "NA",
	[HAL_SYS_PROC_DSP_5] = "NA",
	[HAL_SYS_PROC_DSP_6] = "NA",
	[HAL_SYS_PROC_DSP_7] = "NA",
	[HAL_SYS_PROC_EVE_0] = "EVE1",
	[HAL_SYS_PROC_EVE_1] = "EVE2",
	[HAL_SYS_PROC_EVE_2] = "EVE3",
	[HAL_SYS_PROC_EVE_3] = "EVE4",
	[HAL_SYS_PROC_EVE_4] = "NA",
	[HAL_SYS_PROC_EVE_5] = "NA",
	[HAL_SYS_PROC_EVE_6] = "NA",
	[HAL_SYS_PROC_EVE_7] = "NA"
};

static int ti_proc_map[HAL_SYS_PROC_MAX] =
{
	[HAL_SYS_PROC_IPU_0] = -1,
	[HAL_SYS_PROC_IPU_1] = -1,
	[HAL_SYS_PROC_IPU_2] = SYSTEM_PROC_IPU2,
	[HAL_SYS_PROC_IPU_3] = -1,
	[HAL_SYS_PROC_IPU_4] = -1,
	[HAL_SYS_PROC_IPU_5] = -1,
	[HAL_SYS_PROC_IPU_6] = -1,
	[HAL_SYS_PROC_IPU_7] = -1,
	[HAL_SYS_PROC_CPU_0] = SYSTEM_PROC_A15_0,
	[HAL_SYS_PROC_CPU_1] = -1,
	[HAL_SYS_PROC_CPU_2] = -1,
	[HAL_SYS_PROC_CPU_3] = -1,
	[HAL_SYS_PROC_CPU_4] = -1,
	[HAL_SYS_PROC_CPU_5] = -1,
	[HAL_SYS_PROC_CPU_6] = -1,
	[HAL_SYS_PROC_CPU_7] = -1,
	[HAL_SYS_PROC_DSP_0] = SYSTEM_PROC_DSP1,
	[HAL_SYS_PROC_DSP_1] = SYSTEM_PROC_DSP2,
	[HAL_SYS_PROC_DSP_2] = -1,
	[HAL_SYS_PROC_DSP_3] = -1,
	[HAL_SYS_PROC_DSP_4] = -1,
	[HAL_SYS_PROC_DSP_5] = -1,
	[HAL_SYS_PROC_DSP_6] = -1,
	[HAL_SYS_PROC_DSP_7] = -1,
	[HAL_SYS_PROC_EVE_0] = SYSTEM_PROC_EVE1,
	[HAL_SYS_PROC_EVE_1] = SYSTEM_PROC_EVE2,
	[HAL_SYS_PROC_EVE_2] = SYSTEM_PROC_EVE3,
	[HAL_SYS_PROC_EVE_3] = SYSTEM_PROC_EVE4,
	[HAL_SYS_PROC_EVE_4] = -1,
	[HAL_SYS_PROC_EVE_5] = -1,
	[HAL_SYS_PROC_EVE_6] = -1,
	[HAL_SYS_PROC_EVE_7] = -1
};


static void _system_perf_init(void)
{
	unsigned int arch_proc_id, link_id;

	if (init_flag)
	{
		return;
	}

	for (arch_proc_id = 0; arch_proc_id < SYSTEM_PROC_MAX; arch_proc_id++)
	{
		if (System_isProcEnabled(arch_proc_id) == FALSE
		        || arch_proc_id == System_getSelfProcId())
		{
			continue;
		}

		link_id = SYSTEM_MAKE_LINK_ID(arch_proc_id, SYSTEM_LINK_ID_PROCK_LINK_ID);
		System_linkControl(
		    link_id,
		    SYSTEM_COMMON_CMD_CPU_LOAD_CALC_START,
		    NULL,
		    0,
		    TRUE
		);
	}

	init_flag = 1;
}


static void get_a15_cpu_perf(hal_system_perf_t *perf)
{
	FILE *fd;
	unsigned int usr, sys, nic, idle, io, irq, sirq;
	char buf[256];

	system("top -n 1 | grep CPU: > /tmp/top.txt");
	fd = fopen("/tmp/top.txt", "r");
	fgets(buf, sizeof(buf), fd);
	sscanf(buf, "CPU:  %u%% usr  %u%% sys   %u%% nic   %u%% idle   %u%% io   %u%% irq   %u%% sirq",
	       &usr, &sys, &nic, &idle, &io, &irq, &sirq);
	//Vps_printf("usr=%u, sys=%u, nic=%u, idle=%u, io=%u, irq=%u, sirq=%u\n",
	//	usr, sys, nic, idle, io, irq, sirq);
	perf->integer_value = 100 - idle;
	perf->fractional_value = 0;
	fclose(fd);
}


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
int hal_system_get_perf(hal_sys_process_id_e processer_id, hal_system_perf_t *perf)
{
	Utils_SystemLoadStats loadStats[SYSTEM_PROC_MAX];
	UInt32 arch_proc_id;

	if (processer_id >= HAL_SYS_PROC_MAX)
	{
		Vps_printf("\r\n### hal_system_get_perf ### The processer_id out of range!!!\r\n");
		return HAL_SYS_EFAIL;
	}

	_system_perf_init();
	arch_proc_id = ti_proc_map[processer_id];

	if (arch_proc_id == -1)
	{
		Vps_printf("\r\n### hal_system_get_perf ### The proc id=0x%x is a unsupported processer !!!\r\n", processer_id);
		return HAL_SYS_EFAIL;
	}

	if (SYSTEM_PROC_A15_0 == ti_proc_map[processer_id])
	{
		get_a15_cpu_perf(perf);
		strncpy(perf->core_name, ti_system_proc_name[processer_id], HAL_SYS_CORE_NAME_MAX);
		perf->used = 1;
		return HAL_SYS_OK;
	}

	memset(loadStats, 0, sizeof(Utils_SystemLoadStats)*SYSTEM_PROC_MAX);
	System_linkControl(
	    SYSTEM_LINK_ID_IPU1_0,
	    SYSTEM_LINK_CMD_GET_CORE_PRF_LOAD,
	    loadStats,
	    sizeof(Utils_SystemLoadStats)*SYSTEM_PROC_MAX,
	    TRUE);
	perf->integer_value = loadStats[ti_proc_map[processer_id]].totalLoadParams.integerValue;
	perf->fractional_value = loadStats[ti_proc_map[processer_id]].totalLoadParams.fractionalValue;
	strncpy(perf->core_name, ti_system_proc_name[processer_id], HAL_SYS_CORE_NAME_MAX);
	perf->used = 1;

	return HAL_SYS_OK;
}


/*******************************************************************************
 *  hal_system_get_perf_all
 *  描述：获取系统所有处理器的使用率
 *  输入：
 *     perfall:所有CPU使用率结构体指针
 *  输出：无
 *  返回：成功返回0, 失败返回<0
 *  备注：无
 *******************************************************************************/
int hal_system_get_perf_all(hal_system_perf_all_t *perfall)
{
	Utils_SystemLoadStats loadStats[SYSTEM_PROC_MAX];
	unsigned int i;

	_system_perf_init();
	memset(loadStats, 0, sizeof(Utils_SystemLoadStats)*SYSTEM_PROC_MAX);
	System_linkControl(
	    SYSTEM_LINK_ID_IPU1_0,
	    SYSTEM_LINK_CMD_GET_CORE_PRF_LOAD,
	    loadStats,
	    sizeof(Utils_SystemLoadStats)*SYSTEM_PROC_MAX,
	    TRUE);

	for (i = 0; i < HAL_SYS_PROC_MAX; i++)
	{
		if (-1 == ti_proc_map[i])
		{
			perfall->perf[i].integer_value = 0;
			perfall->perf[i].fractional_value = 0;
			perfall->perf[i].used = 0;
			strncpy(perfall->perf[i].core_name, ti_system_proc_name[i], HAL_SYS_CORE_NAME_MAX);
			continue;
		}

		if (SYSTEM_PROC_A15_0 == ti_proc_map[i])
		{
			get_a15_cpu_perf(&(perfall->perf[i]));
			perfall->perf[i].used = 1;
			strncpy(perfall->perf[i].core_name, ti_system_proc_name[i], HAL_SYS_CORE_NAME_MAX);
			continue;
		}

		perfall->perf[i].integer_value = loadStats[ti_proc_map[i]].totalLoadParams.integerValue;
		perfall->perf[i].fractional_value = loadStats[ti_proc_map[i]].totalLoadParams.fractionalValue;
		strncpy(perfall->perf[i].core_name, ti_system_proc_name[i], HAL_SYS_CORE_NAME_MAX);
		perfall->perf[i].used = 1;
	}

	return HAL_SYS_OK;
}

/*******************************************************************************
 *  hal_system_print_perf_all
 *  描述：打印系统所有处理器的使用率
 *  输入：
 *     perfall:所有CPU使用率结构体指针
 *  输出：无
 *  返回：无
 *  备注：无
 *******************************************************************************/
void hal_system_print_perf_all(hal_system_perf_all_t *perfall)
{
	int i;

	if (perfall == NULL)
	{
		return;
	}

	Vps_printf("List of all CPUs:\n");

	for (i = 0; i < HAL_SYS_PROC_MAX; i++)
	{
		if (perfall->perf[i].used)
		{
			Vps_printf("ID:%2d, name:%4s, percent:%02d.%d%%\n", i, perfall->perf[i].core_name, perfall->perf[i].integer_value, perfall->perf[i].fractional_value);
		}
	}
}

/*******************************************************************************
 *  hal_system_print_statistics_collector
 *  描述：打印系统内存吞吐率
 *  输入：无
 *  输出：无
 *  返回：无
 *  备注：无
 *******************************************************************************/
void hal_system_print_statistics_collector(void)
{
	ChainsCommon_statCollectorPrint();
}


