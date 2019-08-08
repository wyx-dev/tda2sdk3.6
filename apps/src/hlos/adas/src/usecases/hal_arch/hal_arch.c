/******************************************************************************

Copyright (c) [2018 - 2019] MOMENTA Incorporated. All rights reserved.

******************************************************************************/


/*******************************************************************************
 *
 * 文件: hal_arch.c
 *
 * 描述: 该文件提供HAL ARCH模块的接口函数
 *
 ******************************************************************************/


/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************/
#include "hal_arch.h"
#include <system_cfg.h>
#include <src/hlos/adas/include/chains.h>
#include <include/link_api/networkCtrl_api.h>


/*******************************************************************************
 *  Globals
 *******************************************************************************/
Chains_Ctrl gChains_usecaseCfg;


/*******************************************************************************
 *  函数名：hal_arch_init
 *  描述：系统hal层申请资源以及配置初始化参数
 *  输入：无
 *  输出：无
 *  返回：HAL_ARCH_OK 表示关闭系统arch资源成功，HAL_ARCH_EFAIL表示失败
 *  备注：HAL 层初始化工作第一个调用的接口
 *******************************************************************************/
int hal_arch_init(void)
{
	int ret = 0;
	Bool shutdownRemoteCores = FALSE;

	ret = System_init();
	if(ret < 0)
	{
		Vps_printf("\r\n### hal_arch_init ###  System_init ERROR !!!\r\n");
		ret = System_deInit(shutdownRemoteCores);
		if(ret < 0)
		{
			Vps_printf("\r\n### hal_arch_deinit ###  System_deInit ERROR !!!\r\n");
			return HAL_ARCH_EFAIL;
		}
		return HAL_ARCH_EFAIL;
	}

#ifndef QNX_BUILD
	ret = NetworkCtrl_init();
	if(ret < 0)
	{
		Vps_printf("\r\n### hal_arch_init ###  NetworkCtrl_init ERROR !!!\r\n");
		ret = NetworkCtrl_deInit();
		if(ret < 0)
		{
			Vps_printf("\r\n### hal_arch_deinit ###  NetworkCtrl_deInit ERROR !!!\r\n");
			return HAL_ARCH_EFAIL;
		}
		return HAL_ARCH_EFAIL;
	}
#endif

	ret = ChainsCommon_appCtrlCommonInit();
	if(ret < 0)
	{
		Vps_printf("\r\n### hal_arch_init ###  ChainsCommon_appCtrlCommonInit ERROR !!!\r\n");
		ret = ChainsCommon_appCtrlCommonDeInit();
		if(ret < 0)
		{
			Vps_printf("\r\n### hal_arch_deinit ###  ChainsCommon_appCtrlCommonDeInit ERROR !!!\r\n");
			return HAL_ARCH_EFAIL;
		}
		return HAL_ARCH_EFAIL;
	}

	Chains_Ctrl_Init(&gChains_usecaseCfg);

	/* allow remote prints to complete, before showing main menu */
	OSA_waitMsecs(500);

	return HAL_ARCH_OK;
}


/*******************************************************************************
 *  函数名：hal_arch_deinit
 *  描述：系统hal层关闭时释放初始化申请的资源
 *  输入：无
 *  输出：无
 *  返回：HAL_ARCH_OK 表示关闭系统arch资源成功，HAL_ARCH_EFAIL表示失败
 *  备注：一定要和hal_arch_init配套使用
 *******************************************************************************/
int hal_arch_deinit(void)
{
	int ret = 0;
	Bool shutdownRemoteCores = FALSE;

	ret = ChainsCommon_appCtrlCommonDeInit();
	if(ret < 0)
	{
		Vps_printf("\r\n### hal_arch_deinit ###  ChainsCommon_appCtrlCommonDeInit ERROR !!!\r\n");
		return HAL_ARCH_EFAIL;
	}

#ifndef QNX_BUILD
	ret = NetworkCtrl_deInit();
	if(ret < 0)
	{
		Vps_printf("\r\n### hal_arch_deinit ###  NetworkCtrl_deInit ERROR !!!\r\n");
		return HAL_ARCH_EFAIL;
	}
#endif

	ret = System_deInit(shutdownRemoteCores);
	if(ret < 0)
	{
		Vps_printf("\r\n### hal_arch_deinit ###  System_deInit ERROR !!!\r\n");
		return HAL_ARCH_EFAIL;
	}

	return HAL_ARCH_OK;
}


