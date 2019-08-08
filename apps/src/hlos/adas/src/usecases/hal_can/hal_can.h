/******************************************************************************

Copyright (c) [2018 - 2019] MOMENTA Incorporated. All rights reserved.

******************************************************************************/


/*******************************************************************************
 *
 * 文件: hal_can.h
 *
 * 描述: 该文件提供HAL CAN模块的接口函数以及相应的结构体变量类型
 *
 ******************************************************************************/


#ifndef __HAL_CAN_H__
#define __HAL_CAN_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************/
#include <stdint.h>
#include <sys/time.h>


/*******************************************************************************
 *  TYPEDEFS
 *******************************************************************************/
/*******************************************************************************
 *  枚举: hal_can_block_state_e
 *  描述: 该枚举表示CAN block state
 *  备注：无
 *******************************************************************************/
typedef enum _hal_can_block_state_e {
    CAN_BLOCK = 0,
    CAN_UNBLOCK
} hal_can_block_state_e;


/*******************************************************************************
 *  枚举: hal_can_id_type_e
 *  描述: 该枚举表示CAN ID TYPE
 *  备注：无
 *******************************************************************************/
typedef enum _hal_can_id_type_e {
    CAN_ID_TYPE_STD = 0,
    CAN_ID_TYPE_EXT
} hal_can_id_type_e;


/*******************************************************************************
 *  枚举: hal_can_protocol_type_e
 *  描述: 该枚举表示CAN protocol TYPE
 *  备注：无
 *******************************************************************************/
typedef enum _hal_can_protocol_type_e {
    CAN_PROTOCOL_TYPE_NORMAL,
} hal_can_protocol_type_e;


/*******************************************************************************
 *  结构体:hal_can_filter_t
 *  描述：定义HAL CAN 协议数据结构体变量类型
 *  成员：
		id: CAN id
		mask: CAN mask
 *  备注：
 *******************************************************************************/
typedef struct _hal_can_filter_t {
    uint32_t id;
    uint32_t mask;
} hal_can_filter_t;


/*******************************************************************************
 *  结构体:hal_can_msg_t
 *  描述：定义HAL CAN MSG数据结构体变量类型
 *  成员：
		can_id: CAN ID
		can_dlc: CAN DLC
		timestamp: CAN 帧时间戳
		data: CAN 帧数据
 *  备注：
 *******************************************************************************/
typedef struct _hal_can_msg_t {
    uint32_t can_id;
    uint8_t can_dlc;
    struct timeval timestamp;
    uint8_t data[8];
} hal_can_msg_t;

typedef struct _hal_can_t hal_can_t;

/*******************************************************************************
 *  结构体:hal_can_protocol_t
 *  描述：定义HAL CAN 协议数据结构体变量类型
 *  成员：
		init: CAN初始化函数指针
		deinit: CAN关闭函数指针
		set_filter: CAN设置过滤器函数指针
		read: CAN Read 函数指针
		write: CAN Write 函数指针
		set_block_state: 设置can总线阻塞状态
		set_block_timeout: 设置can总线阻塞状态超时
 *  备注：
 *******************************************************************************/
typedef struct _hal_can_protocol_t
{
    int (*init)(hal_can_t *);
    int (*deinit)(hal_can_t *);
    int (*set_filter)(hal_can_t *, hal_can_filter_t *, int);
    int (*read)(hal_can_t *, hal_can_msg_t *, int);
    int (*write)(hal_can_t *, const hal_can_msg_t *, int);
    int (*set_block_state)(hal_can_t *, hal_can_block_state_e);
    int (*set_block_timeout)(hal_can_t *, struct timeval *);
} hal_can_protocol_t;


/*******************************************************************************
 *  结构体:hal_can_t
 *  描述：定义HAL CAN 结构体变量类型
 *  成员：
		protocol: can protocol结构体
		id_type: can id 类型
		protocol_type: can protocol 类型
		interface_name: 接口名
		socket_fd: socket can 文件描述符
 *  备注：
 *******************************************************************************/
typedef struct _hal_can_t
{
    hal_can_protocol_t protocol;
    hal_can_id_type_e id_type;
    hal_can_protocol_type_e protocol_type;
    const char *interface_name;
    int socket_fd;
} hal_can_t;


/*******************************************************************************
 *  结构体:socket_can_t
 *  描述：定义SOCKET CAN数据结构体变量类型
 *  成员：
		protocol: can protocol结构体
		id_type: can id 类型
		protocol_type: can protocol 类型
		interface_name: 接口名
		socket_fd: socket can 文件描述符
 *  备注：
 *******************************************************************************/
typedef struct _socket_can_t {
	hal_can_protocol_t protocol;
    hal_can_id_type_e id_type;
    hal_can_protocol_type_e protocol_type;
    const char *interface_name;
    int socket_fd;
} socket_can_t;


/*******************************************************************************
 *  函数名：hal_can_init
 *  描述：hal层打开CAN总线设备时申请资源以及配置初始化参数
 *  输入：
 *      can：canbus接口上下文结构体
 *  输出：
 *      can：canbus接口上下文结构体
 *  返回：成功返回0, 失败返回-1
 *  备注：无
 *******************************************************************************/
int hal_can_init(hal_can_t *can);


/*******************************************************************************
 *  函数名：hal_can_deinit
 *  描述：hal层关闭CAN总线设备时申请资源以及配置初始化参数
 *  输入：
 *      can：canbus接口上下文
 *  输出：无
 *  返回：成功返回0, 失败返回-1
 *  备注：无
 *******************************************************************************/
int hal_can_deinit(hal_can_t *can);


/*******************************************************************************
 *  函数名：hal_can_read_msg
 *  描述：从can总线读取数据
 *  输入：
 *      can:canbus接口上下文
 *      n:  读取消息个数
 *  输出：
 *      msg:读取的canbus数据
 *  返回：成功返回0, 失败返回-1
 *  备注：无
 *******************************************************************************/
int hal_can_read_msg(hal_can_t *can, hal_can_msg_t *msg, int n);


/*******************************************************************************
 *  函数名：hal_can_write_msg
 *  描述：向can总线读取数据
 *  输入：
 *      can:canbus接口上下文
 *      msg:写入的canbus数据
 *      n:  写入消息个数
 *  输出：无
 *  返回：成功返回0, 失败返回-1
 *  备注：无
 *******************************************************************************/
int hal_can_write_msg(hal_can_t *can, const hal_can_msg_t *msg, int n);


/*******************************************************************************
 *  函数名：hal_can_set_filter
 *  描述：设置can总线过滤器
 *  输入：
 *      can:canbus接口上下文
 *      filter:canbus数据过滤器
 *      n: 消息个数
 *  输出：无
 *  返回：成功返回0, 失败返回-1
 *  备注：无
 *******************************************************************************/
int hal_can_set_filter(hal_can_t *can, hal_can_filter_t *filter, int n);


/*******************************************************************************
 *  函数名：hal_can_set_block_state
 *  描述：设置can总线阻塞状态
 *  输入：
 *      can:canbus接口上下文
 *      state:canbus阻塞状态枚举
 *  输出：无
 *  返回：成功返回0, 失败返回-1
 *  备注：无
 *******************************************************************************/
int hal_can_set_block_state(hal_can_t *can, hal_can_block_state_e state);


/*******************************************************************************
 *  函数名：hal_can_extract_bits
 *  描述：从msg中提取比特位
 *  输入：
 *      msg:canbus消息
 *      start_bit:起始位
 *      bit_len:比特位长度
 *      bigend:大小端格式
 *  输出：无
 *  返回：提取的比特数据
 *  备注：无
 *******************************************************************************/
unsigned long int hal_can_extract_bits(const hal_can_msg_t *msg,
				unsigned int start_bit,
				unsigned int bit_len,
				char bigend);


/*******************************************************************************
 *	函数名：hal_can_pack_bits
 *	描述：从msg中提取比特位
 *	输入：
 *		msg:canbus消息
 *		start_bit:起始位
 *		data：要打包的比特位数据
 *		bit_len:比特位长度
 *		bigend:大小端格式
 *	输出：无
 *	返回：无
 *	备注：无
 *******************************************************************************/
void hal_can_pack_bits(hal_can_msg_t *msg,
				unsigned int start_bit,
				unsigned int bit_len,
				unsigned long int data,
				char bigend);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif //__HAL_CAN_H__

