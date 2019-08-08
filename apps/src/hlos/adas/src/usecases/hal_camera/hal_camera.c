/******************************************************************************

Copyright (c) [2018 - 2019] MOMENTA Incorporated. All rights reserved.

******************************************************************************/


/*******************************************************************************
 *
 * 文件: hal_camera.c
 *
 * 描述: 该文件提供HAL CAMERA模块的接口函数
 *
 ******************************************************************************/

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************/
#include "hal_camera.h"
#include "camera_base.h"
#include "iniparser.h"
#include "hal_offline_feed.h"
/*******************************************************************************
 *  Globals
 *******************************************************************************/
static _hal_frame_t g_frm_data[4];
HAL_FRMDATA_CB g_frame_cb = NULL;
_camera_save_local_obj g_camera_save_local_obj =
{
	.resize_chnl_num = 0,
	.hdmi_camera_flag = 0,
};

hal_run_mode_e run_mode;
extern char offline_image_path[];
extern char offline_candump_file_path[];
extern FILE *offline_filelist_fp;
extern hal_offline_feed_framerate_e feed_framerate;
extern pthread_mutex_t feed_mutex;

//#define __DEBUG
#ifdef __DEBUG
#define HAL_DEBUG(args...)				\
 	do {						\
			Vps_printf(args);		\
	} while(0)
#else
#define HAL_DEBUG(args...) do { } while(0)
#endif

static void _cam_save_local_get_frame_from_null(hal_frame_data_t *frmdata)
{
	pthread_mutex_lock(&g_camera_save_local_obj.mutex[frmdata->channel]);
	memcpy(&g_frm_data[frmdata->channel].frame_data[g_frm_data[frmdata->channel].input_count++ % HAL_BUF_MAX_QUE_SIZE], frmdata, sizeof(hal_frame_data_t));
	pthread_cond_signal(&g_camera_save_local_obj.cond[frmdata->channel]);
	pthread_mutex_unlock(&g_camera_save_local_obj.mutex[frmdata->channel]);
}


static void _cam_save_local_set_frame_cb(HAL_FRMDATA_CB fun)
{
	g_frame_cb = fun;
}


static int _config_file_parser(const char *file_name)
{
	dictionary  *ini;
	const char  *s;

	if (file_name == NULL)
	{
		return  HAL_CAMERA_EINVALID_PARAMS;
	}

	ini = iniparser_load(file_name);
	if (ini == NULL)
	{
		return  HAL_CAMERA_UNKNOWN;
	}

	s = iniparser_getstring(ini, "sensor-params-set:capture-name", NULL);
	if (s == NULL)
	{
		Vps_printf("\r\n### _config_file_parser ### capture-name error !!!\r\n");
		return HAL_CAMERA_UNKNOWN;
	}
	else
	{
		if (0 == strcmp("OV10635", s))
		{
			gChains_usecaseCfg.captureSrc = CHAINS_CAPTURE_SRC_OV10635;
		}
		else if (0 == strcmp("OV2718_XC7027", s))
		{
			gChains_usecaseCfg.captureSrc = CHAINS_CAPTURE_SRC_XC7027;
		}
		else if (0 == strcmp("OV10640_OV490", s))
		{
			gChains_usecaseCfg.captureSrc = CHAINS_CAPTURE_SRC_OV490_OV10640;
		}
		else if (0 == strcmp("HDMI_CAMERA", s))
		{
			gChains_usecaseCfg.captureSrc = CHAINS_CAPTURE_SRC_HDMI_720P;
			g_camera_save_local_obj.hdmi_camera_flag = 1;
		}
		else
		{
			gChains_usecaseCfg.captureSrc = CHAINS_CAPTURE_SRC_OV10635;
		}

		Vps_printf("capture-name:     [%s]\n", s ? s : "UNDEF");
	}

	s = iniparser_getstring(ini, "sensor-params-set:resolution", NULL);
	Vps_printf("resolution:     [%s]\n", s ? s : "UNDEF");
	s = iniparser_getstring(ini, "sensor-params-set:offline-image-path", NULL);
	if (s)
		strcpy(offline_image_path, s);
	else
		strcpy(offline_image_path, "./images");

	s = iniparser_getstring(ini, "sensor-params-set:offline-candump-file", NULL);
	if (s)
		strcpy(offline_candump_file_path, s);
	else
		strcpy(offline_candump_file_path, "./candump.log");

	s = iniparser_getstring(ini, "sensor-params-set:run-mode", NULL);
	if (s)
	{
		if (!strcmp(s, "online"))
			run_mode = HAL_MODE_ONLINE;
		else if (!strcmp(s, "offline_feed"))
			run_mode = HAL_MODE_OFFLINE_FEED;
		else
			run_mode = HAL_MODE_ONLINE;
	}
	else
		run_mode = HAL_MODE_ONLINE;

	iniparser_freedict(ini);

	return HAL_CAMERA_OK;
}

static void _cam_set_framerate(int framerate)
{
	System_linkControl(SYSTEM_LINK_ID_APP_CTRL,
	                   APP_CTRL_LINK_CMD_CAM_SET_FRAMERATE,
	                   &framerate,
	                   sizeof(int),
	                   TRUE);
}

/*******************************************************************************
*  函数名：hal_camera_display
*  描述：实现camera数据投屏显示by HDMI
*  输入：
*    sensor_num: 为测试camera预览时camera的个数，目前支持1路和4路
*  输出：无
*  返回：成功返回0, 失败返回<0
*  备注：该接口调用后会直接投屏显示，也其他的HAL接口没有任何的调用依赖关系
*******************************************************************************/
int hal_camera_test_display(int sensor_num)
{
	if (sensor_num != 1 && sensor_num != 4)
	{
		Vps_printf("\r\n### hal_camera_test_display ### param sensor_num ERROR !!! Only support 1 and 4 channel camera\r\n");
		return HAL_CAMERA_EFAIL;
	}

	gChains_usecaseCfg.numLvdsCh = sensor_num;

	if (sensor_num == 4)
	{
		chains_hal_multi_camera_display(&gChains_usecaseCfg);
	}
	else
	{
		chains_hal_single_camera_display(&gChains_usecaseCfg);
	}

	return HAL_CAMERA_OK;
}

/*******************************************************************************
 *  函数名：hal_camera_set_resize
 *  描述：对外的hal层接口,实现hal层对图像缩放配置;
 *  输入：
 *    resize_config: 为hal_frame_resize_t结构体指针，其中成员是配置的缩放信息
 *    resize_num: resize 使用vpe的路数；固定最大支持8路,最小1路；
 *  输出：无
 *  返回：成功返回0, 失败返回<0
 *  备注：无
 *******************************************************************************/
int hal_camera_set_resize(hal_frame_resize_t *resize_config, unsigned int resize_num)
{
	int i = 0;

	if (NULL == resize_config)
	{
		printf("\r\n### ERROR !!! hal_camera_set_resize ### check resize_config !!! \r\n");
		g_camera_save_local_obj.resize_chnl_num = 0;
		return HAL_CAMERA_EINVALID_PARAMS;
	}

	if (resize_num > HAL_CAMERA_VPE_RESIZE_MAX_NUM || resize_num <= 0)
	{
		printf("\r\n### ERROR !!! hal_camera_set_resize ### resize_num max is %d !!! \r\n", HAL_CAMERA_VPE_RESIZE_MAX_NUM);
		g_camera_save_local_obj.resize_chnl_num = 0;
		return HAL_CAMERA_EINVALID_PARAMS;
	}

	g_camera_save_local_obj.resize_chnl_num = resize_num;

	//printf("\r\n### Enter ### hal_camera_set_resize ### !!!\r\n");
	for (i = 0; i < resize_num; i++)
	{
		g_camera_save_local_obj.resize_config[i].output_width  = resize_config[i].output_width;
		g_camera_save_local_obj.resize_config[i].output_height = resize_config[i].output_height;
		g_camera_save_local_obj.resize_config[i].start_x       = resize_config[i].start_x;
		g_camera_save_local_obj.resize_config[i].start_y       = resize_config[i].start_y;
		g_camera_save_local_obj.resize_config[i].end_x         = resize_config[i].end_x;
		g_camera_save_local_obj.resize_config[i].end_y         = resize_config[i].end_y;
		g_camera_save_local_obj.resize_config[i].is_use_vpe     = resize_config[i].is_use_vpe;
		g_camera_save_local_obj.resize_config[i].colorformat   = resize_config[i].colorformat;
	}

	return HAL_CAMERA_OK;
}


/*******************************************************************************
 *  函数名：hal_camera_get_frame
 *  描述：对外的hal层接口,实现hal层获取camera的帧数据;
 *  输入：
 *    frmdata: 为hal_frame_data_t结构体指针，需要输入channel成员
 *    timeout: 为获取帧数据时的等待时间
 *  输出：
      frmdata：图像帧数据
 *  返回：成功返回0, 失败返回<0
 *  备注：一般会被调用者的线程函数服务函数调用
 *******************************************************************************/
int hal_camera_get_frame(hal_frame_data_t *frmdata, unsigned int timeout)
{
	int i = 0;
	struct timeval now;
	struct timespec abstime;

	if (frmdata == NULL)
	{
		Vps_printf("\r\n### hal_camera_get_frame ### param frmdata NULL!!!\r\n");
		return HAL_CAMERA_EFAIL;
	}

	gettimeofday(&now, NULL);
	int nsec = now.tv_usec * 1000 + (timeout % 1000) * 1000000;
	abstime.tv_nsec = nsec % 1000000000;
	abstime.tv_sec = now.tv_sec + nsec / 1000000000 + timeout / 1000;

	pthread_mutex_lock(&g_camera_save_local_obj.mutex[frmdata->channel]);

	/*
	*  如果timeout等于 0表示阻塞方式
	*/
	if (HAL_GET_FRAME_TIMEOUT == timeout)
	{
		pthread_cond_wait(&g_camera_save_local_obj.cond[frmdata->channel], &g_camera_save_local_obj.mutex[frmdata->channel]);
	}
	else
	{
		pthread_cond_timedwait(&g_camera_save_local_obj.cond[frmdata->channel], &g_camera_save_local_obj.mutex[frmdata->channel], &abstime);
	}

	if (g_frm_data[frmdata->channel].output_count != g_frm_data[frmdata->channel].input_count)
	{
		if (g_frm_data[frmdata->channel].input_count - g_frm_data[frmdata->channel].output_count >= HAL_BUF_MAX_QUE_SIZE)
		{
			Vps_printf("\r\n### hal_camera_get_frame ### OVER FLOW %d \r\n", g_frm_data[frmdata->channel].input_count - g_frm_data[frmdata->channel].output_count);
			g_frm_data[frmdata->channel].output_count = g_frm_data[frmdata->channel].input_count - HAL_BUF_MAX_QUE_SIZE + 2;
		}

		frmdata->timestamp.tv_sec = g_frm_data[frmdata->channel].frame_data[g_frm_data[frmdata->channel].output_count % HAL_BUF_MAX_QUE_SIZE].timestamp.tv_sec;
		frmdata->timestamp.tv_usec = g_frm_data[frmdata->channel].frame_data[g_frm_data[frmdata->channel].output_count % HAL_BUF_MAX_QUE_SIZE].timestamp.tv_usec;
		frmdata->hw_timestamp = g_frm_data[frmdata->channel].frame_data[g_frm_data[frmdata->channel].output_count % HAL_BUF_MAX_QUE_SIZE].hw_timestamp;

		if (g_camera_save_local_obj.resize_chnl_num == 0)
		{
			frmdata->len = g_frm_data[frmdata->channel].frame_data[g_frm_data[frmdata->channel].output_count % HAL_BUF_MAX_QUE_SIZE].len;
			memcpy(frmdata->buffer,
			       g_frm_data[frmdata->channel].frame_data[g_frm_data[frmdata->channel].output_count % HAL_BUF_MAX_QUE_SIZE].buffer,
			       frmdata->len);
		}

		if (g_camera_save_local_obj.resize_chnl_num > 0)
		{
			for (i = 0; i < g_camera_save_local_obj.resize_chnl_num; i++)
			{
				frmdata->resize_buf_len[i] = g_frm_data[frmdata->channel].frame_data[g_frm_data[frmdata->channel].output_count % HAL_BUF_MAX_QUE_SIZE].resize_buf_len[i];
				memcpy(frmdata->resize_buf[i],
				       g_frm_data[frmdata->channel].frame_data[g_frm_data[frmdata->channel].output_count % HAL_BUF_MAX_QUE_SIZE].resize_buf[i],
				       frmdata->resize_buf_len[i]);
			}
		}

		g_frm_data[frmdata->channel].output_count++;
	}
	else
	{
		Vps_printf("\r\n### hal_camera_get_frame ### wait channel[%d] timeout !!!\r\n", frmdata->channel);
		pthread_mutex_unlock(&g_camera_save_local_obj.mutex[frmdata->channel]);
		return HAL_CAMERA_EFAIL;
	}

	pthread_mutex_unlock(&g_camera_save_local_obj.mutex[frmdata->channel]);

	return HAL_CAMERA_OK;
}

/*******************************************************************************
 *  函数名：hal_camera_deinit
 *  描述：hal层关闭camera设备时释放初始化申请的资源,确保camera正常退出
 *  输入：
 *     channel_num:release camera source channel numbers
 *  输出：无
 *  返回：成功返回0, 失败返回<0
 *  备注：无
 *******************************************************************************/
int hal_camera_deinit(int channel_num)
{
	int ret = 0;

	if (channel_num == 1)
	{
#if 0
		ret = chains_hal_single_cam_vpe_resize_save_StopApp(g_camera_save_local_obj.hal_single_cam_vpe_resize_save_AppObj);
		if (ret < 0)
		{
			Vps_printf("\r\n### ERROR ### hal_camera_deinit ### chains_hal_single_cam_vpe_resize_save_StopApp !!!\r\n");
			return HAL_CAMERA_EFAIL;
		}
#else
		if (run_mode == HAL_MODE_OFFLINE_FEED)
		{
			hal_offline_feed_cam_deinit();
		}
		ret = chains_hal_single_cam_vpe_sad_StopApp(g_camera_save_local_obj.hal_single_cam_vpe_sad_AppObj);
		if (ret < 0)
		{
			Vps_printf("\r\n### ERROR ### hal_camera_deinit ### chains_hal_single_cam_vpe_sad_StopApp !!!\r\n");
			return HAL_CAMERA_EFAIL;
		}
#endif
	}
	else if (channel_num == 4)
	{
		ret = chains_hal_multi_camera_save_local_StopApp(g_camera_save_local_obj.hal_multi_camera_save_localAppObj);
		if (ret < 0)
		{
			Vps_printf("\r\n### ERROR ### hal_camera_deinit ### chains_hal_multi_camera_save_local_StopApp \r\n");
			return HAL_CAMERA_EFAIL;
		}
	}

	return HAL_CAMERA_OK;
}

/*******************************************************************************
 *  函数名：hal_offline_cam_deinit
 *  描述：hal层关闭 hal offline camera设备时释放初始化申请的资源,确保camera正常退出
 *  输入：无
 *  输出：无
 *  返回：成功返回0, 失败返回<0
 *  备注：无，只提供1路camera测试
 *******************************************************************************/
int hal_offline_cam_deinit()
{
	int ret = 0;

	//ret = chains_hal_single_cam_vpe_sad_StopApp(g_camera_save_local_obj.hal_single_cam_vpe_sad_AppObj);
	ret = chains_hal_offline_cam_vpe_sad_StopApp(g_camera_save_local_obj.hal_offline_cam_vpe_sad_AppObj);
	if (ret < 0)
	{
		Vps_printf("\r\n### ERROR ### hal_camera_deinit ### chains_hal_offline_cam_vpe_sad_StopApp !!!\r\n");
		return HAL_CAMERA_EFAIL;
	}

	return HAL_CAMERA_OK;
}

/*******************************************************************************
 *  函数名：hal_camera_init
 *  描述：hal层打开camera设备时申请资源以及配置初始化参数
 *  输入：
 *    file_name：初始化camera的配置文件
 *    group_index：对应相机的组号，每组4个相机，如果硬件平台没有相机则忽略该参数
 *    channel_num：初始化camera通道数量
 *       1：表示为1v1r 初始化 1个摄像头并且带有resize功能
 *       4：表示为APA  初始化 4个摄像头没有resize功能
 * 		 X: 其他的暂时不支持
 *    framerate：初始化camera设备输出帧率,目前仅支持15fps，20fps，25fps
 *    colorformat：帧数据的颜色格式配置
 *    func：如果使用回调方式，func为调用者自己实现的callback函数指针;
 *			如果使用主动获取方式，func为NULL
 *  输出：无
 *  返回：成功返回0, 失败返回<0
 *  备注：无
 *******************************************************************************/
int hal_camera_init(const char *file_name, int group_index, int channel_num, int framerate, hal_frame_format_e colorformat, HAL_FRMDATA_CB func)
{
	int i = 0, ret = 0;

	ret = _config_file_parser(file_name);
	if (ret < 0)
	{
		Vps_printf("\r\n### hal_camera_init ### file_name %s ERROR not exist !!!\r\n", file_name);
		return HAL_CAMERA_EINVALID_PARAMS;
	}

	if (group_index != 0 && group_index != 1 && group_index != 2 && group_index != 3)
	{
		Vps_printf("\r\n### hal_camera_init ### group_index %d ERROR !!!\r\n", group_index);
		return HAL_CAMERA_EINVALID_PARAMS;
	}

	if (channel_num < 0 || channel_num > 4)
	{
		Vps_printf("\r\n### hal_camera_init ### channel_num  %d ERROR !!!\r\n", channel_num);
		return HAL_CAMERA_EINVALID_PARAMS;
	}

	if (framerate != 15  && framerate != 20 && framerate != 25)
	{
		Vps_printf("\r\n### hal_camera_init ### framerate  %d ERROR not support !!! %d\r\n", framerate);
		return HAL_CAMERA_EFAIL;
	}
	_cam_set_framerate(framerate);

	gChains_usecaseCfg.numLvdsCh = channel_num;

	/*Init CAM FrameRate*/
	g_camera_save_local_obj.outPutFrameRate = framerate;  //暂时强制写死30，底层同步信号是出发后为15fps,20fps或25fps，后期修改

	/*Init CAM color format*/
	g_camera_save_local_obj.outPutColorFormat = colorformat;

	/*Init multi camera app OBJ params*/
	g_camera_save_local_obj.hal_multi_camera_save_localAppObj = &g_hal_multi_camera_save_local_AppObj;
	g_camera_save_local_obj.hal_multi_camera_save_localAppObj->chainsCfg = &gChains_usecaseCfg;
	g_camera_save_local_obj.hal_multi_camera_save_localAppObj->chainsCfg->numLvdsCh = channel_num;
	/*Init single camera app OBJ params*/
	g_camera_save_local_obj.hal_single_camera_save_local_AppObj = &g_hal_single_camera_save_local_AppObj;
	g_camera_save_local_obj.hal_single_camera_save_local_AppObj->chainsCfg = &gChains_usecaseCfg;
	g_camera_save_local_obj.hal_single_camera_save_local_AppObj->chainsCfg->numLvdsCh = channel_num;
	/*Init single cam vpe resize save OBJ params*/
	g_camera_save_local_obj.hal_single_cam_vpe_resize_save_AppObj = &g_hal_single_cam_vpe_resize_save_AppObj;
	g_camera_save_local_obj.hal_single_cam_vpe_resize_save_AppObj->chainsCfg = &gChains_usecaseCfg;
	g_camera_save_local_obj.hal_single_cam_vpe_resize_save_AppObj->chainsCfg->numLvdsCh = channel_num;
	/*Init single cam vpe resize save and display OBJ params*/
	g_camera_save_local_obj.hal_single_cam_vpe_sad_AppObj = &g_hal_single_cam_vpe_sad_AppObj;
	g_camera_save_local_obj.hal_single_cam_vpe_sad_AppObj->chainsCfg = &gChains_usecaseCfg;
	g_camera_save_local_obj.hal_single_cam_vpe_sad_AppObj->chainsCfg->numLvdsCh = channel_num;

	if (run_mode == HAL_MODE_OFFLINE_FEED)
	{
		/*Init offline feed cam vpe resize save and display OBJ params*/
		g_camera_save_local_obj.hal_offline_feed_cam_vpe_sad_AppObj = &g_hal_offline_feed_cam_vpe_sad_AppObj;
		g_camera_save_local_obj.hal_offline_feed_cam_vpe_sad_AppObj->chainsCfg = &gChains_usecaseCfg;
		g_camera_save_local_obj.hal_offline_feed_cam_vpe_sad_AppObj->chainsCfg->numLvdsCh = channel_num;
	}
	memset(g_frm_data, 0, sizeof(_hal_frame_t)*channel_num);

	/*Init Hal CAM COLORFORMAT*/
	switch (g_camera_save_local_obj.outPutColorFormat)
	{
	case HAL_FRAME_FORMAT_BGR24_888:
		g_camera_save_local_obj.outPutColorFormatCopyLen = 3;
		break;
	case HAL_FRAME_FORMAT_RGB24_888:
		g_camera_save_local_obj.outPutColorFormatCopyLen = 3;
		break;
	case HAL_FRAME_FORMAT_YUV420SP_UV:
		g_camera_save_local_obj.outPutColorFormatCopyLen = 1.5;
		break;
	case HAL_FRAME_FORMAT_YUV422P:
		g_camera_save_local_obj.outPutColorFormatCopyLen = 1.5;
		break;
	default:
		g_camera_save_local_obj.outPutColorFormatCopyLen = 1.5;
		break;
	}

	for (i = 0; i < g_camera_save_local_obj.resize_chnl_num; i++)
	{
		switch (g_camera_save_local_obj.resize_config[i].colorformat)
		{
		case HAL_FRAME_FORMAT_BGR24_888:
			g_camera_save_local_obj.outVpeResizeColorCopyLen[i] = 3;
			break;
		case HAL_FRAME_FORMAT_RGB24_888:
			g_camera_save_local_obj.outVpeResizeColorCopyLen[i] = 3;
			break;
		case HAL_FRAME_FORMAT_YUV420SP_UV:
			g_camera_save_local_obj.outVpeResizeColorCopyLen[i] = 1.5;
			break;
		case HAL_FRAME_FORMAT_YUV422P:
			g_camera_save_local_obj.outVpeResizeColorCopyLen[i] = 1.5;
			break;
		default:
			g_camera_save_local_obj.outVpeResizeColorCopyLen[i] = 1.5;
			break;
		}
	}

	/*Init Hal CAM mutex*/
	for (i = 0; i < channel_num; i++)
	{
		if (pthread_mutex_init(&g_camera_save_local_obj.mutex[i], NULL))
		{
			Vps_printf("\r\n### hal_camera_init ### pthread_mutex_init[%d] Error !\r\n", i);
			return HAL_CAMERA_EFAIL;
		}

		if (pthread_cond_init(&g_camera_save_local_obj.cond[i], NULL))
		{
			Vps_printf("\r\n### hal_camera_init ### pthread_cond_init[%d] Error !\r\n", i);
			return HAL_CAMERA_EFAIL;
		}
	}

	/*Init Hal CAM CallBack Register*/
	if (NULL == func)
	{
		//_cam_save_local_set_frame_cb(_cam_hubing_get);
		_cam_save_local_set_frame_cb(_cam_save_local_get_frame_from_null);
	}
	else
	{
		_cam_save_local_set_frame_cb(func);
	}

	/*For 1V1R */
	if (1 == channel_num)
	{
		if (run_mode == HAL_MODE_OFFLINE_FEED)
		{
			if (pthread_mutex_init(&feed_mutex, NULL))
			{
				Vps_printf("\r\n### hal_camera_init ### feed_mutex Error !\r\n");
				return HAL_CAMERA_EFAIL;
			}
			/*Init create Link and start camera*/
			ret = chains_hal_offline_feed_cam_vpe_sad_Create(
			          &g_camera_save_local_obj.hal_offline_feed_cam_vpe_sad_AppObj->ucObj,
			          g_camera_save_local_obj.hal_offline_feed_cam_vpe_sad_AppObj
			      );
			if (ret < 0)
			{
				Vps_printf("\r\n### chains_hal_offline_cam_vpe_sad_Create ### Error !!!\r\n");
				return HAL_CAMERA_EFAIL;
			}
			switch(framerate)
			{
				case 15:
					feed_framerate = HAL_FEED_FRAMERATE_15FPS;
					break;
				case 20:
					feed_framerate = HAL_FEED_FRAMERATE_20FPS;
					break;
				case 25:
					feed_framerate = HAL_FEED_FRAMERATE_25FPS;
					break;
				default:
					feed_framerate = HAL_FEED_FRAMERATE_25FPS;
					break;
			}

			chains_hal_offline_feed_cam_vpe_sad_StartApp(
			    g_camera_save_local_obj.hal_offline_feed_cam_vpe_sad_AppObj
			);
		}
		else
		{
			/*Init create Link and start camera*/
			ret = chains_hal_single_cam_vpe_sad_Create(
			          &g_camera_save_local_obj.hal_single_cam_vpe_sad_AppObj->ucObj,
			          g_camera_save_local_obj.hal_single_cam_vpe_sad_AppObj
			      );
			if (ret < 0)
			{
				Vps_printf("\r\n### chains_hal_multi_cam_vpe_sad_Create ### Error !!!\r\n");
				return HAL_CAMERA_EFAIL;
			}

			chains_hal_single_cam_vpe_sad_StartApp(
			    g_camera_save_local_obj.hal_single_cam_vpe_sad_AppObj
			);
		}

	}

	/*For APA */
	if (4 == channel_num)
	{
		/*Init create Link and start camera*/
		ret = chains_hal_multi_camera_save_local_Create(
		          &g_camera_save_local_obj.hal_multi_camera_save_localAppObj->ucObj,
		          g_camera_save_local_obj.hal_multi_camera_save_localAppObj
		      );
		if (ret < 0)
		{
			Vps_printf("\r\n### chains_hal_multi_camera_save_local_Create ### Error !!!\r\n");
			return HAL_CAMERA_EFAIL;
		}

		chains_hal_multi_camera_save_local_StartApp(
		    g_camera_save_local_obj.hal_multi_camera_save_localAppObj
		);
	}

	ret = ChainsCommon_prfLoadCalcEnable(TRUE, FALSE, FALSE);
	if (ret < 0)
	{
		Vps_printf("\r\n### ChainsCommon_prfLoadCalcEnable ### Error !!!\r\n");
		return HAL_CAMERA_EFAIL;
	}
	if (run_mode == HAL_MODE_OFFLINE_FEED)
	{
		hal_cam_offline_feed_pause_signal();
		hal_create_feed_thread();
	}

	return HAL_CAMERA_OK;
}


/*******************************************************************************
 *  函数名：hal_offline_cam_init
 *  描述：hal层打开camera设备时申请资源以及配置初始化参数
 *  输入：
 *    group_index：对应相机的组号，每组4个相机，如果硬件平台没有相机则忽略该参数
 *    channel_num：初始化camera通道数量
 *       1：表示为1v1r 初始化 1个摄像头并且带有resize功能
 * 		 X: 其他的暂时不支持
 *    func：如果使用回调方式，func为调用者自己实现的callback函数指针;
 *			如果使用主动获取方式，func为NULL
 *    framerate：初始化camera设备输出帧率,目前仅支持15fps，20fps，25fps
 *    colorformat：帧数据的颜色格式配置
 *  输出：无
 *  返回：成功返回0, 失败返回<0
 *  备注：无
 *******************************************************************************/
int hal_offline_cam_init(int group_index, int channel_num, int framerate, hal_frame_format_e colorformat, HAL_FRMDATA_CB func)
{
	int i = 0, ret = 0;

	if (group_index != 0 && group_index != 1 && group_index != 2 && group_index != 3)
	{
		Vps_printf("\r\n### hal_camera_init ### group_index %d ERROR !!!\r\n", group_index);
		return HAL_CAMERA_EINVALID_PARAMS;
	}

	if (channel_num < 0 || channel_num > 4)
	{
		Vps_printf("\r\n### hal_camera_init ### channel_num  %d ERROR !!!\r\n", channel_num);
		return HAL_CAMERA_EINVALID_PARAMS;
	}

	if (framerate < 0)
	{
		Vps_printf("\r\n### hal_camera_init ### framerate  %d ERROR not support !!! %d\r\n", framerate);
		return HAL_CAMERA_EFAIL;
	}

	gChains_usecaseCfg.numLvdsCh = channel_num;

	/*Init CAM FrameRate*/
	g_camera_save_local_obj.outPutFrameRate = (unsigned int)1000 / framerate; //暂时强制写死30，底层同步信号是出发后为15fps,20fps或25fps，后期修改

	/*Init CAM color format*/
	//g_camera_save_local_obj.inPutColorFormat = HAL_FRAME_FORMAT_RGB24_888;   //fix input pic colorFormat is RGB24_888
	g_camera_save_local_obj.outPutColorFormat = colorformat;

	// /*Init single cam vpe resize save and display OBJ params*/
	// g_camera_save_local_obj.hal_single_cam_vpe_sad_AppObj = &g_hal_single_cam_vpe_sad_AppObj;
	// g_camera_save_local_obj.hal_single_cam_vpe_sad_AppObj->chainsCfg = &gChains_usecaseCfg;
	// g_camera_save_local_obj.hal_single_cam_vpe_sad_AppObj->chainsCfg->numLvdsCh = channel_num;

	/*Init offline cam vpe resize save and display OBJ params*/
	g_camera_save_local_obj.hal_offline_cam_vpe_sad_AppObj = &g_hal_offline_cam_vpe_sad_AppObj;
	g_camera_save_local_obj.hal_offline_cam_vpe_sad_AppObj->chainsCfg = &gChains_usecaseCfg;
	g_camera_save_local_obj.hal_offline_cam_vpe_sad_AppObj->chainsCfg->numLvdsCh = channel_num;

	memset(g_frm_data, 0, sizeof(_hal_frame_t)*channel_num);

	/*Init Hal CAM COLORFORMAT*/
	switch (g_camera_save_local_obj.outPutColorFormat)
	{
	case HAL_FRAME_FORMAT_BGR24_888:
		g_camera_save_local_obj.outPutColorFormatCopyLen = 3;
		break;
	case HAL_FRAME_FORMAT_RGB24_888:
		g_camera_save_local_obj.outPutColorFormatCopyLen = 3;
		break;
	case HAL_FRAME_FORMAT_YUV420SP_UV:
		g_camera_save_local_obj.outPutColorFormatCopyLen = 1.5;
		break;
	case HAL_FRAME_FORMAT_YUV422P:
		g_camera_save_local_obj.outPutColorFormatCopyLen = 1.5;
		break;
	default:
		g_camera_save_local_obj.outPutColorFormatCopyLen = 1.5;
		break;
	}

	for (i = 0; i < g_camera_save_local_obj.resize_chnl_num; i++)
	{
		switch (g_camera_save_local_obj.resize_config[i].colorformat)
		{
		case HAL_FRAME_FORMAT_BGR24_888:
			g_camera_save_local_obj.outVpeResizeColorCopyLen[i] = 3;
			break;
		case HAL_FRAME_FORMAT_RGB24_888:
			g_camera_save_local_obj.outVpeResizeColorCopyLen[i] = 3;
			break;
		case HAL_FRAME_FORMAT_YUV420SP_UV:
			g_camera_save_local_obj.outVpeResizeColorCopyLen[i] = 1.5;
			break;
		case HAL_FRAME_FORMAT_YUV422P:
			g_camera_save_local_obj.outVpeResizeColorCopyLen[i] = 1.5;
			break;
		default:
			g_camera_save_local_obj.outVpeResizeColorCopyLen[i] = 1.5;
			break;
		}
	}

	/*Init Hal CAM mutex*/
	for (i = 0; i < channel_num; i++)
	{
		if (pthread_mutex_init(&g_camera_save_local_obj.mutex[i], NULL))
		{
			Vps_printf("\r\n### hal_camera_init ### pthread_mutex_init[%d] Error !\r\n", i);
			return HAL_CAMERA_EFAIL;
		}

		if (pthread_cond_init(&g_camera_save_local_obj.cond[i], NULL))
		{
			Vps_printf("\r\n### hal_camera_init ### pthread_cond_init[%d] Error !\r\n", i);
			return HAL_CAMERA_EFAIL;
		}
	}

	/*Init Hal CAM CallBack Register*/
	if (NULL == func)
	{
		//_cam_save_local_set_frame_cb(_cam_hubing_get);
		_cam_save_local_set_frame_cb(_cam_save_local_get_frame_from_null);
	}
	else
	{
		_cam_save_local_set_frame_cb(func);
	}

	/*For 1V1R */
	if (1 == channel_num)
	{
		/*Init create Link and start camera*/
		ret = chains_hal_offline_cam_vpe_sad_Create(
		          &g_camera_save_local_obj.hal_offline_cam_vpe_sad_AppObj->ucObj,
		          g_camera_save_local_obj.hal_offline_cam_vpe_sad_AppObj
		      );
		if (ret < 0)
		{
			Vps_printf("\r\n### chains_hal_offline_cam_vpe_sad_Create ### Error !!!\r\n");
			return HAL_CAMERA_EFAIL;
		}

		chains_hal_offline_cam_vpe_sad_StartApp(
		    g_camera_save_local_obj.hal_offline_cam_vpe_sad_AppObj
		);
	}

	/*For APA */
	if (4 == channel_num)
	{
		/*Init create Link and start camera*/
		ret = chains_hal_multi_camera_save_local_Create(
		          &g_camera_save_local_obj.hal_multi_camera_save_localAppObj->ucObj,
		          g_camera_save_local_obj.hal_multi_camera_save_localAppObj
		      );
		if (ret < 0)
		{
			Vps_printf("\r\n### chains_hal_multi_camera_save_local_Create ### Error !!!\r\n");
			return HAL_CAMERA_EFAIL;
		}

		chains_hal_multi_camera_save_local_StartApp(
		    g_camera_save_local_obj.hal_multi_camera_save_localAppObj
		);
	}

	ret = ChainsCommon_prfLoadCalcEnable(TRUE, FALSE, FALSE);
	if (ret < 0)
	{
		Vps_printf("\r\n### ChainsCommon_prfLoadCalcEnable ### Error !!!\r\n");
		return HAL_CAMERA_EFAIL;
	}

	return HAL_CAMERA_OK;
}


int hal_cam_offline_malloc_continuous_aligned(void **data, unsigned int size, unsigned int align)
{
	int status = HAL_CAMERA_OK;
	Void *addr = NULL;

	if ((0 == size) || (!data))
	{
		return HAL_CAMERA_EINVALID_PARAMS;
	}

	addr = OSA_memAllocSR(OSA_HEAPID_DDR_CACHED_SR1, size, align);

	status = (NULL != addr) ? HAL_CAMERA_OK : HAL_CAMERA_EALLOC;

	*data = addr;
	g_camera_save_local_obj.offlineDataFileName = (char *)addr;

	return status;
}


int hal_cam_offline_free_continuous_aligned(void *data, unsigned int size)
{
	int status = HAL_CAMERA_OK;

	if ((0 == size) || (!data))
	{
		return HAL_CAMERA_EINVALID_PARAMS;
	}

	status = OSA_memFreeSR(OSA_HEAPID_DDR_CACHED_SR1, data, size);

	status = (status == SYSTEM_LINK_STATUS_SOK) ? HAL_CAMERA_OK : HAL_CAMERA_EALLOC;

	return status;
}


/*******************************************************************************
*  函数名：hal_cam_offline_restart_signal
*  描述：向NullSrcLink发送restart signal 用于重新启动chains
*  输入：无
*  输出：无
*  返回：成功返回0, 失败返回<0
*  备注：
*******************************************************************************/
int hal_cam_offline_restart_signal(void)
{
	chains_hal_offline_cam_vpe_sad_nullsrc_restart(g_camera_save_local_obj.hal_offline_cam_vpe_sad_AppObj);
	return 0;
}


/*******************************************************************************
*  函数名：hal_cam_offline_pause_signal
*  描述：向NullSrcLink发送pause signal 用于阻塞Chains
*  输入：无
*  输出：无
*  返回：成功返回0, 失败返回<0
*  备注：
*******************************************************************************/
int hal_cam_offline_pause_signal(void)
{
	chains_hal_offline_cam_vpe_sad_nullsrc_pause(g_camera_save_local_obj.hal_offline_cam_vpe_sad_AppObj);
	return 0;
}

