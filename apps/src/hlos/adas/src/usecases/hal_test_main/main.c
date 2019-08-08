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
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

#define YUV_LEN 2
#define YUV_STUFF "yuv"
#define RGB_LEN 3
#define RGB_STUFF "bgr"
#define VPE_RESIZE_NUM 5

bool Wait_exit;
int channel_num = 0;

int img_sufix_len[5]={2,3,2,3,3};
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
	char dir_name[100] = {0};
	char time_file_name[20] = {0};
	char time_data[100] = {0};
	char cmd[100] = {0};

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

	sprintf(time_file_name, "%s","timestamp");
	FILE* ftimestamp = fopen(time_file_name, "wb+");
//	sprintf(dir_name, "%lld",(long long)frmdata.timestamp.tv_sec);
//	mkdir(dir_name,0755);
//	sprintf(cmd, "%s/%s","/run/media/sda1/hubing/image",dir_name);
//	chdir(cmd);

	/*loop to get data*/
	while(Wait_exit)
	{
		ret = hal_camera_get_frame(&frmdata,1000);
			char file_name[64] = {0};
			long long timeout = 0;
			memset(file_name, 0, 64);
			timeout = 1000000 * (long long)frmdata.timestamp.tv_sec + frmdata.timestamp.tv_usec;

			if(1 == channel_num)
			{
				sprintf(file_name, "%ld.%ld.bgr", frmdata.timestamp.tv_sec,frmdata.timestamp.tv_usec);
				FILE* fp = fopen(file_name, "wb+");
				if(fp)
				{
					fwrite(frmdata.resize_buf[1], 1, frmdata.resize_buf_len[1], fp);
					fflush(fp);
					fclose(fp);
				}
				sprintf(time_data, "%ld.%ld\n", frmdata.timestamp.tv_sec,frmdata.timestamp.tv_usec);
				fwrite(time_data, 1, strlen(time_data), ftimestamp);
				fflush(ftimestamp);
#if 0
				if(count_frame%10 == 0)
				{
					chdir("/run/media/sda1/hubing/image");
					sprintf(dir_name, "%lld",(long long)frmdata.timestamp.tv_sec);
					mkdir(dir_name,0755);
					sprintf(cmd, "%s/%s","/run/media/sda1/hubing/image",dir_name);
					chdir(cmd);
				}
#endif
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
		//	#endif
		//}

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
	mkdir("image",0755);
	usleep(10);
	//chdir("/home/hubing/image");
	chdir("/run/media/sda1/hubing/image");
	usleep(10);
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

int main (int argc, char ** argv)
{
	hal_arch_init();
	printf("\r\n### main ------------------------------### start ERROR !!!\r\n");
	camera_run();
	printf("\r\n### main ------------------------------### end ERROR !!!\r\n");
	hal_arch_deinit();

	return 0;
}
