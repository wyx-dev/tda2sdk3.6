/******************************************************************************

Copyright (c) [2018 - 2019] MOMENTA Incorporated. All rights reserved.

******************************************************************************/


/*******************************************************************************
 *
 * 文件: main.c
 *
 * 描述: 该文件提供HAL 模块测试各个模块功能的接口函数
 *
 ******************************************************************************/

#include "main.h"

#define YUV_LEN 2
#define YUV_STUFF "yuv"
#define RGB_LEN 3
#define RGB_STUFF "bgr"
#define VPE_RESIZE_NUM 5

bool Wait_exit;
int channel_num = 0;
int g_image_data_len = 720*1280*3/2;
char *g_image_data = NULL;

float img_sufix_len[5]={1.5,3,1.5,3,3};
char *img_sufix[5]={"yuv","bgr","yuv","bgr","bgr"};
hal_frame_resize_t resize_config[VPE_RESIZE_NUM] = {
	{  0,   0, 1280, 720, 1280, 720, HAL_FRAME_FORMAT_YUV420SP_UV ,false},
	{  0,   0, 1280, 720, 1280, 720, HAL_FRAME_FORMAT_RGB24_888   ,true },
	{  0, 160, 1280, 720,  512, 224, HAL_FRAME_FORMAT_YUV420SP_UV ,true },
	{ 22,   0, 1258, 720,  384, 224, HAL_FRAME_FORMAT_RGB24_888   ,true },
	{332, 180,  948, 560,  384, 224, HAL_FRAME_FORMAT_RGB24_888   ,true },
};


static void SigHandler(int signum)
{
    signal(SIGKILL, SIG_IGN);
    signal(SIGINT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGSTOP, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    Wait_exit = false;

    signal(SIGKILL, SIG_DFL);
    signal(SIGINT, SIG_DFL);
    signal(SIGTERM, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
    signal(SIGSTOP, SIG_DFL);
    signal(SIGHUP, SIG_DFL);
}

static void SigSetup(void)
{
    struct sigaction action;

    memset(&action, 0, sizeof(action));
    action.sa_handler = SigHandler;

    sigaction(SIGKILL, &action, NULL);
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGTERM, &action, NULL);
    sigaction(SIGQUIT, &action, NULL);
    sigaction(SIGSTOP, &action, NULL);
    sigaction(SIGHUP, &action, NULL);
}

/*******************************************************************************
 *  函数名：_cam_save_local_thread_func
 *  描述：保持图片到本地时，所使用的线程服务函数
 *  输入：
 *    ptr：创建线程时传入的参数，此处是channel id
 *  输出：void *
 *  返回：无
 *  备注：无
 *******************************************************************************/
static void* _cam_save_local_thread_func(void* ptr)
{
	int i = 0;
	int ret = 0;
	unsigned int width = 1280U;
	unsigned int height = 720U;
	hal_frame_data_t frmdata;
	static int count_frame = 0;
	long long new_user_time = 0;
	unsigned long long old_user_time[4] = {0};
	int imgLength = width*height*YUV_LEN;
	frmdata.channel = *(unsigned int*)ptr;

	/*malloc mem for 1v1r vpe resize*/
	if(1 == channel_num)
	{
		for(i = 0; i < VPE_RESIZE_NUM; i++)
		{
			frmdata.resize_buf[i] = (unsigned char *)malloc(resize_config[i].output_width*resize_config[i].output_height*img_sufix_len[i]);
		}
	}/*malloc mem for apa 4 camera*/
	else if(4 == channel_num)
	{
        frmdata.buffer = (unsigned char *)malloc(imgLength);
	}

	/*loop to get data*/
	while(Wait_exit)
	{
		ret = hal_camera_get_frame(&frmdata,1000);
		if((0 == ret) && ((count_frame%1) == 0))
		{
			#if 1
			//if(count_frame == 100){
			char file_name[500] = {0};
			long long timeout = 0;
			memset(file_name, 0, 64);
			timeout = 1000000 * (long long)frmdata.timestamp.tv_sec + (long long)frmdata.timestamp.tv_usec;

			if(1 == channel_num)
			{
				for(i = 0; i < VPE_RESIZE_NUM; i++)
				{
					//sprintf(file_name, "off_frame_%lld_channel_%d_%d_count_%d.%s", timeout, frmdata.channel, i, count_frame, img_sufix[i]);
					sprintf(file_name, "on_frame_%lld_channel_%d_%d_count_%d.%s", timeout, frmdata.channel, i, count_frame, img_sufix[i]);
					FILE* fp = fopen(file_name, "wb+");
					if(fp)
					{
						fwrite(frmdata.resize_buf[i], 1, frmdata.resize_buf_len[i], fp);
						fflush(fp);
						fclose(fp);
					}
				}
			}
			else if(4 == channel_num)
			{
				new_user_time = 1000000 * (long long)frmdata.timestamp.tv_sec + (long long)frmdata.timestamp.tv_usec;
				printf("\r\n### cam_save_local_thread_func ### hal_camera_get_frame Delta Time[%d] = %lld \r\n",frmdata.channel, new_user_time-old_user_time[frmdata.channel]);
				old_user_time[frmdata.channel] = new_user_time;
				sprintf(file_name, "frame_%lld_channel_%d_count_%d.%s", timeout, frmdata.channel, count_frame, YUV_STUFF);
				FILE* fp = fopen(file_name, "wb+");
				if(fp)
				{
					fwrite(frmdata.buffer, 1, imgLength, fp);
					fflush(fp);
					fclose(fp);
				}
			}
			//}
			#endif
		}

		if(ret < 0)
		{
			pthread_exit((void*)(-1));
		}

		count_frame++;
		usleep(30);
	}

	/*free mem*/
	if(1 == channel_num)
	{
		for(i = 0; i < VPE_RESIZE_NUM; i++)
		{
			free(frmdata.resize_buf[i]);
		}
	}
	else if(4 == channel_num)
	{
		free(frmdata.buffer);
	}

	pthread_exit(0);
}

#if 1
static void* _cam_offline_thread_func(void* ptr)
{
	FILE* fp = NULL;

	/*loop to get data*/
	while(Wait_exit)
	{
		hal_cam_offline_pause_signal();
		//sprintf(file_name, "frame_%lld_channel_%d_%d_count_%d.%s", timeout, frmdata.channel, i, count_frame, img_sufix[i]);
		//FILE* fp = fopen(file_name, "wb+");
		fp = fopen("/home/hubing/aa.yuv", "r+");
		if(fp)
		{
			fread(g_image_data, 1, g_image_data_len, fp);
			fflush(fp);
			fclose(fp);
		}
		hal_cam_offline_restart_signal();
		usleep(66000);
		// hal_cam_offline_pause_signal();
		// //sprintf(file_name, "frame_%lld_channel_%d_%d_count_%d.%s", timeout, frmdata.channel, i, count_frame, img_sufix[i]);
		// //FILE* fp = fopen(file_name, "wb+");
		// fp = fopen("/home/hubing/frame_1547883789935465_channel_0_0_count_0.yuv", "r");
		// if(fp)
		// {
		// 	fread(g_image_data, 1, g_image_data_len, fp);
		// 	fflush(fp);
		// 	fclose(fp);
		// }
		// hal_cam_offline_restart_signal();
		// usleep(66000);
	}

	pthread_exit(0);
}
#endif


/*******************************************************************************
 *  函数名：can_run
 *  描述：can接口内部测试用例
 *  输入：无
 *  输出：无
 *  返回：无
 *  备注：无
 *******************************************************************************/
void can_run(void)
{
	int i = 0;
	int ret = 0;
	int can0_len = 1;
	hal_can_t *can0 = NULL;
	hal_can_msg_t can0_msg;
    int j = 0;

	printf("---HAL CAN0 START---\n");

	can0=(hal_can_t *)malloc(sizeof(hal_can_t));
	hal_can_init(can0);

	while(1)
	{
        ret = hal_can_read_msg(can0, &can0_msg, can0_len);
        if (ret < 0)
        {
            hal_can_deinit(can0);
        }
        else if (ret < 1)
        {
            //do nothing
        }
        else
        {
            for (j = 0; j < ret; j++)
            {
                hal_can_read_msg(can0, &can0_msg, 1);
            }
        }

		printf("---can_id = 0x%x\n",can0_msg.can_id);
		printf("---can_dlc = 0x%x\n",can0_msg.can_dlc);
		printf("---timestamp.tv_sec = %ld\n",can0_msg.timestamp.tv_sec);
		printf("---timestamp.tv_usec = %ld\n",can0_msg.timestamp.tv_usec);
		printf("---CAN0 data:");

		for(i=0; i<can0_msg.can_dlc; i++)
		{
			printf(" data[%d] = 0x%x",i,can0_msg.data[i]);
		}

		printf("---\n");
	}

	hal_can_deinit(can0);
	printf("---HAL CAN0 END---\n");

	return ;
}


/*******************************************************************************
 *  函数名：sonar_run
 *  描述：声呐内部测试用例
 *  输入：无
 *  输出：无
 *  返回：无
 *  备注：无
 *******************************************************************************/
void sonar_run(void)
{
	int ret = 0;
	hal_can_t *can0 = NULL;
	hal_can_msg_t candata = {0};

	hal_sonar_t *pstsonar = NULL;
	hal_sonar_data_t sonardata;

	printf("---HAL Sonar START---\n");

	can0 = (hal_can_t *)malloc(sizeof(hal_can_t));
	ret = hal_can_init(can0);

	if(ret >= 0)
	{
		pstsonar = (hal_sonar_t *)malloc(sizeof(hal_sonar_t));
		memset((uint8_t *)pstsonar,0,sizeof(hal_sonar_t));
		ret = hal_sonar_init(pstsonar, can0);

		for(int count = 0; count < 5; count++)
		{
			candata.can_id = 0x601;
			candata.can_dlc = 8;
			candata.data[count] = 0x55;
			hal_can_write_msg(can0,&candata,1);
			usleep (10000);
		}

		if(ret >= 0)
		{
			while(1)
			{
				hal_sonar_get_data(pstsonar, NULL, &sonardata);
				printf("---fol_cm = %6d\t,tv_sec = %10ld\t,tv_usec = %10ld---\n", sonardata.fol_cm,sonardata.fol_ts.tv_sec,sonardata.fol_ts.tv_usec);
				printf("---fcl_cm = %6d\t,tv_sec = %10ld\t,tv_usec = %10ld---\n", sonardata.fcl_cm,sonardata.fcl_ts.tv_sec,sonardata.fcl_ts.tv_usec);
				printf("---fcr_cm = %6d\t,tv_sec = %10ld\t,tv_usec = %10ld---\n", sonardata.fcr_cm,sonardata.fcr_ts.tv_sec,sonardata.fcr_ts.tv_usec);
				printf("---for_cm = %6d\t,tv_sec = %10ld\t,tv_usec = %10ld---\n", sonardata.for_cm,sonardata.for_ts.tv_sec,sonardata.for_ts.tv_usec);
				printf("---rol_cm = %6d\t,tv_sec = %10ld\t,tv_usec = %10ld---\n", sonardata.rol_cm,sonardata.rol_ts.tv_sec,sonardata.rol_ts.tv_usec);
				printf("---rcl_cm = %6d\t,tv_sec = %10ld\t,tv_usec = %10ld---\n", sonardata.rcl_cm,sonardata.rcl_ts.tv_sec,sonardata.rcl_ts.tv_usec);
				printf("---rcr_cm = %6d\t,tv_sec = %10ld\t,tv_usec = %10ld---\n", sonardata.rcr_cm,sonardata.rcr_ts.tv_sec,sonardata.rcr_ts.tv_usec);
				printf("---ror_cm = %6d\t,tv_sec = %10ld\t,tv_usec = %10ld---\n", sonardata.ror_cm,sonardata.ror_ts.tv_sec,sonardata.ror_ts.tv_usec);
				printf("---sfl_cm = %6d\t,tv_sec = %10ld\t,tv_usec = %10ld---\n", sonardata.sfl_cm,sonardata.sfl_ts.tv_sec,sonardata.sfl_ts.tv_usec);
				printf("---sfr_cm = %6d\t,tv_sec = %10ld\t,tv_usec = %10ld---\n", sonardata.sfr_cm,sonardata.sfr_ts.tv_sec,sonardata.sfr_ts.tv_usec);
				printf("---srl_cm = %6d\t,tv_sec = %10ld\t,tv_usec = %10ld---\n", sonardata.srl_cm,sonardata.srl_ts.tv_sec,sonardata.srl_ts.tv_usec);
				printf("---srr_cm = %6d\t,tv_sec = %10ld\t,tv_usec = %10ld---\n", sonardata.srr_cm,sonardata.srr_ts.tv_sec,sonardata.srr_ts.tv_usec);
				usleep (10000);
			}
		}
		else
		{
			hal_sonar_deinit(pstsonar);
			free(pstsonar);
			hal_can_deinit(can0);
			free(can0);
			printf("---HAL Sonar Init sonar error---\n");
		}
	}
	else
	{
		hal_can_deinit(can0);
		free(can0);
		printf("---HAL Sonar Init can error---\n");
	}

	printf("---HAL SONAR END---\n");

	return;
}

/*******************************************************************************
 *  函数名：system_run
 *  描述：测试hal system总入口
 *  输入：无
 *  输出：无
 *  返回：无
 *  备注：
 *******************************************************************************/
void system_run(void)
{
	hal_system_perf_t perf;

	printf("\r\n### system_run ### HAL_SYS_PROC_DSP !!!\r\n");
	hal_system_get_perf(HAL_SYS_PROC_DSP_1, &perf);
	hal_system_get_perf(HAL_SYS_PROC_DSP_2, &perf);

	printf("\r\n### system_run ### HAL_SYS_PROC_EVE !!!\r\n");
	hal_system_get_perf(HAL_SYS_PROC_EVE_1, &perf);
	hal_system_get_perf(HAL_SYS_PROC_EVE_2, &perf);
	hal_system_get_perf(HAL_SYS_PROC_EVE_3, &perf);
	hal_system_get_perf(HAL_SYS_PROC_EVE_4, &perf);

	printf("\r\n### system_run ### HAL_SYS_PROC_A15 !!!\r\n");
	hal_system_get_perf(HAL_SYS_PROC_CPU_0, &perf);
}

/*******************************************************************************
 *  函数名：camera_run
 *  描述：测试hal camera总入口
 *  输入：无
 *  输出：无
 *  返回：无
 *  备注：基于TI SDK的测试hal camera接口时才会用到
 *******************************************************************************/
void camera_run(void)
{
	int i = 0,ret = 0, group_index = 0, framerate = 25;
	int cam_channel[4] = {0,1,2,3};
	pthread_t cam_pthread_id[4];

	SigSetup();
	channel_num = 1;
	Wait_exit = true;

	/*set resize to multi output*/
	hal_camera_set_resize(resize_config, VPE_RESIZE_NUM);

	ret = hal_camera_init("./camera.conf", group_index, channel_num, framerate, HAL_FRAME_FORMAT_YUV420SP_UV, NULL);
	if(ret < 0)
	{
		printf("\r\n### hal_camera_run ### hal_camera_init ERROR !!!\r\n");
		return ;
	}

	for(i = 0; i< channel_num; i++)
	{
		pthread_create(&cam_pthread_id[i], NULL, _cam_save_local_thread_func, &cam_channel[i]);
		usleep(10);
	}

	for(i = 0; i < channel_num; i++)
	{
		pthread_join(cam_pthread_id[i], NULL);
	}

	hal_camera_deinit(channel_num);
}

/*******************************************************************************
 *  函数名：cam_offline_run
 *  描述：测试 hal offline camera 总入口
 *  输入：无
 *  输出：无
 *  返回：无
 *  备注：基于TI SDK的测试 hal offline cam 接口时才会用到
 *******************************************************************************/
void cam_offline_run(void)
{
	int i = 0,ret = 0, group_index = 0, framerate = 15;
	int cam_offline_channel = 0;
	int cam_channel[4] = {0,1,2,3};
	pthread_t cam_offline_pthread_id;
	pthread_t cam_pthread_id[4];

	SigSetup();
	channel_num = 1;
	Wait_exit = true;

	/*set resize to multi output*/
	hal_camera_set_resize(resize_config, VPE_RESIZE_NUM);

	//ret = hal_camera_init("./camera.conf", group_index, channel_num, framerate, HAL_FRAME_FORMAT_YUV420SP_UV, NULL);
	ret = hal_offline_cam_init(group_index, channel_num, framerate, HAL_FRAME_FORMAT_YUV420SP_UV, NULL);
	if(ret < 0)
	{
		printf("\r\n### hal_camera_run ### hal_offline_cam_init ERROR !!!\r\n");
		return ;
	}

	hal_cam_offline_malloc_continuous_aligned((void **)&g_image_data, g_image_data_len, 32);

		// hal_cam_offline_pause_signal();
		// //sprintf(file_name, "frame_%lld_channel_%d_%d_count_%d.%s", timeout, frmdata.channel, i, count_frame, img_sufix[i]);
		// //FILE* fp = fopen(file_name, "wb+");
		// FILE *fp = fopen("/home/hubing/yuv_data/1547435278.894631.raw", "r");
		// if(fp)
		// {
		// 	fread(g_image_data, 1, g_image_data_len, fp);
		// 	fflush(fp);
		// 	fclose(fp);
		// }
		// hal_cam_offline_restart_signal();
		// //sleep(1);

	for(i = 0; i< channel_num; i++)
	{
		pthread_create(&cam_pthread_id[i], NULL, _cam_save_local_thread_func, &cam_channel[i]);
		usleep(10);
	}
	pthread_create(&cam_offline_pthread_id, NULL, _cam_offline_thread_func, &cam_offline_channel);

	for(i = 0; i < channel_num; i++)
	{
		pthread_join(cam_pthread_id[i], NULL);
	}
	pthread_join(cam_offline_pthread_id, NULL);

	//hal_camera_deinit(channel_num);
	hal_offline_cam_deinit();
	hal_cam_offline_free_continuous_aligned((void *)g_image_data, g_image_data_len);
}


int main (int argc, char ** argv)
{
	hal_get_version();

	hal_arch_init();
#if 1
	camera_run();
	//cam_offline_run();
#else
	hal_camera_test_display(1);
#endif
	//system_run();
	hal_arch_deinit();

	return 0;
}
