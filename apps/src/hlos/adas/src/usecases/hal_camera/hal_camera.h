/******************************************************************************

Copyright (c) [2018 - 2019] MOMENTA Incorporated. All rights reserved.

******************************************************************************/


/*******************************************************************************
 *
 * 文件: hal_camera.h
 *
 * 描述: 该文件提供HAL CAMERA模块的接口函数以及相应的结构体变量类型
 *
 ******************************************************************************/

#ifndef __HAL_CAMERA_H__
#define __HAL_CAMERA_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************/
#include <sys/time.h>
#include <stdbool.h>

/*******************************************************************************
*  DEFINES
*  描述：The return status of the HAL CAMERA API.
*******************************************************************************/
#define HAL_CAMERA_OK              (0)
#define HAL_CAMERA_EFAIL           (-1)
#define HAL_CAMERA_ETIMEOUT        (-2)
#define HAL_CAMERA_EALLOC          (-3)
#define HAL_CAMERA_EBUSY           (-4)
#define HAL_CAMERA_EINVALID_PARAMS (-5)
#define HAL_CAMERA_UNKNOWN         (-6)

#define HAL_CAMERA_VPE_RESIZE_MAX_NUM  (8)


/*******************************************************************************
 *  结构体：hal_frame_data_t
 *  描述：定义HAL CAM 帧数据结构体变量类型
 *  成员：
	len:帧数据的长度
	channel：帧数据对应的camera 通道序号
	group：帧数据对应的camera 组序号
	exposure_time：每一帧对应的曝光时间
	silence：数据是否需要静默
	reserve：保留
	hw_timestamp：帧数据和平台相关的时间戳
	timestamp：帧数据对应的timeval
	buffer:帧数据指针
 *  备注：该结构是从camera拿到每一帧图像的抽象
 *******************************************************************************/
typedef struct _hal_frame_data_t
{
	unsigned int len;                       //frame buf size
	unsigned int channel;                   //frame channel index
	unsigned int group;                     //frame group index
	unsigned int exposure_time;             //frame with exporse time
	unsigned int silence;                   //silence 1 is drop this channel frame, and vice versa
	unsigned int reserve;                   //reserve
	unsigned long long hw_timestamp;        //frame time stamp come from timeval with platform
	struct timeval timestamp;               //frame time stamp
	unsigned char *buffer;                  //frame buf point
	unsigned char *resize_buf[HAL_CAMERA_VPE_RESIZE_MAX_NUM];    //vpe frame resize output address buffer array
	unsigned int resize_buf_len[HAL_CAMERA_VPE_RESIZE_MAX_NUM];  //vpe resize buf size
}hal_frame_data_t;


/*******************************************************************************
 *  枚举：hal_frame_format_e
 *  描述：该枚举成员表示配置camera 输出数据颜色格式可选择类型
 *  备注：无
 *******************************************************************************/
typedef enum _hal_frame_format_e
{
	HAL_FRAME_FORMAT_YUV422I_YUYV = 1,
	HAL_FRAME_FORMAT_YUV422SP_UV = 4,
	HAL_FRAME_FORMAT_YUV422SP_VU = 5,
	HAL_FRAME_FORMAT_YUV422P     = 6,
	HAL_FRAME_FORMAT_YUV420SP_UV = 7,	//default NV12
	HAL_FRAME_FORMAT_YUV420SP_VU = 8,	//default NV21
	HAL_FRAME_FORMAT_YUV420P     = 9,
	HAL_FRAME_FORMAT_RGB24_888   = 20,
	HAL_FRAME_FORMAT_BGR24_888   = 36,
	HAL_FRAME_FORMAT_INVALID,
}hal_frame_format_e;

/*******************************************************************************
 *  枚举：hal_run_mode_e
 *  描述：该枚举成员表示配置camera 输出数据颜色格式可选择类型
 *  备注：无
 *******************************************************************************/
typedef enum _hal_run_mode_e
{
	HAL_MODE_ONLINE = 0,
	HAL_MODE_OFFLINE_FEED,
	HAL_MODE_INVALID,
}hal_run_mode_e;

/*******************************************************************************
 *  结构体：hal_frame_resize_t
 *  描述：定义HAL CAM 帧数据缩放结构体变量类型
 *  成员：
 *	start_x:选择图像crop 起点坐标 x value
 *	start_y:选择图像crop 起点坐标 y value
 *	end_x:选择图像crop 终点坐标 x value
 *	end_y:选择图像crop 终点坐标 y value
 *	output_width:图像缩放后输出宽度
 *	output_height:图像缩放后输出高度
 *	bool is_use_vpe:是否使用vpe模块去缩放
 *	colorformat:配置缩放后输出图像的颜色格式
 *  备注：无
 *******************************************************************************/
typedef struct _hal_frame_resize_t
{
	unsigned int start_x;                   //crop start point x value
	unsigned int start_y;                   //crop start point y value
	unsigned int end_x;                     //crop end point x value
	unsigned int end_y;                     //crop end point y value
	unsigned int output_width;              //resize output width value
	unsigned int output_height;             //resize output height value
	hal_frame_format_e colorformat;         //resize output colorformat
	bool is_use_vpe;                        //use vpe module to resize or not
}hal_frame_resize_t;


/*******************************************************************************
 *  函数指针：HAL_FRMDATA_CB
 *  描述：定义函数指针变量类型
 *  输入：
 *    frmdata: 为hal_frame_data_t结构体指针
 *  输出：无
 *  返回：无
 *  备注：如果调用者使用自定义callback来接收帧数据，
 *     可以自定义一个该函数指针类型的函数，并在hal_camera_init中注册
 *******************************************************************************/
typedef void (*HAL_FRMDATA_CB)(hal_frame_data_t *frmdata);


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
int hal_camera_init(const char *file_name, int group_index, int channel_num, int framerate, hal_frame_format_e colorformat, HAL_FRMDATA_CB func);


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
int hal_offline_cam_init(int group_index, int channel_num, int framerate, hal_frame_format_e colorformat, HAL_FRMDATA_CB func);


/*******************************************************************************
 *  函数名：hal_camera_set_resize
 *  描述：对外的hal层接口,实现hal层对图像缩放配置;
 *  输入：
 *    resize_config: hal_frame_resize_t结构体指针，其中成员是配置的缩放信息
 *    resize_num: resize 使用vpe的路数；固定最大支持8路；
 *  输出：无
 *  返回：成功返回0, 失败返回<0
 *  备注：如果需要使用vpe_resize功能，则该接口需要在hal_camera_init接口之前调用
 *******************************************************************************/
int hal_camera_set_resize(hal_frame_resize_t *resize_config, unsigned int resize_num);


/*******************************************************************************
 *  函数名：hal_camera_get_frame
 *  描述：对外的hal层接口,实现hal层获取camera的帧数据;
 *  输入：
 *    frmdata: 为hal_frame_data_t结构体指针，需要输入channel成员
 *    timeout: 为获取帧数据时的等待超时时间，0表示永远阻塞
 *  输出：
      frmdata：图像帧数据
 *  返回：成功返回0, 失败返回<0
 *  备注：一般会被调用者的线程函数服务函数调用
 *******************************************************************************/
int hal_camera_get_frame(hal_frame_data_t *frmdata, unsigned int timeout);


/*******************************************************************************
 *  函数名：hal_camera_deinit
 *  描述：hal层关闭camera设备时释放初始化申请的资源,确保camera正常退出
 *  输入：无
 *  输出：无
 *  返回：成功返回0, 失败返回<0
 *  备注：无
 *******************************************************************************/
int hal_camera_deinit(int channel_num);


/*******************************************************************************
 *  函数名：hal_offline_cam_deinit
 *  描述：hal层关闭 hal offline camera设备时释放初始化申请的资源,确保camera正常退出
 *  输入：无
 *  输出：无
 *  返回：成功返回0, 失败返回<0
 *  备注：无，只提供1路camera测试
 *******************************************************************************/
int hal_offline_cam_deinit();


/*******************************************************************************
*  函数名：hal_cam_offline_restart_signal
*  描述：向NullSrcLink发送restart signal 用于重新启动chains
*  输入：无
*  输出：无
*  返回：成功返回0, 失败返回<0
*  备注：
*******************************************************************************/
int hal_cam_offline_restart_signal(void);


/*******************************************************************************
*  函数名：hal_cam_offline_pause_signal
*  描述：向NullSrcLink发送pause signal 用于阻塞Chains
*  输入：无
*  输出：无
*  返回：成功返回0, 失败返回<0
*  备注：
*******************************************************************************/
int hal_cam_offline_pause_signal(void);


int hal_cam_offline_malloc_continuous_aligned(void **data, unsigned int size, unsigned int align);
int hal_cam_offline_free_continuous_aligned(void *data, unsigned int size);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif //__HAL_CAMERA_H__
