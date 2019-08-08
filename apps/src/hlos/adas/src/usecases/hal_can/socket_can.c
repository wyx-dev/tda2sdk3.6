/******************************************************************************

Copyright (c) [2018 - 2019] MOMENTA Incorporated. All rights reserved.

******************************************************************************/


/*******************************************************************************
 *
 * 文件: socket_can.c
 *
 * 描述: 该文件提供SOCKET CAN模块的接口函数
 *
 ******************************************************************************/

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <string.h>
#include <assert.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <src/hlos/adas/include/chains.h>
#include "socket_can.h"


/*******************************************************************************
 *  函数名: socket_can_init
 *  描述：socket CAN 初始化
 *  输入：
 *    can: 为hal_can_t结构体指针
 *  输出：无
 *  返回：SOCKET_CAN_OK 表示正常初始化,SOCKET_CAN_EFAIL表示初始化失败
 *  备注：无
 *******************************************************************************/
int socket_can_init(hal_can_t *can)
{
	int flags = 0;
	struct ifreq ifr;
	struct sockaddr_can addr;
	socket_can_t *socket_can = NULL;

	assert(can != NULL);

	can->protocol.set_block_timeout = NULL;
	socket_can = (socket_can_t *)can;
	socket_can->socket_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);

	flags = fcntl(socket_can->socket_fd, F_GETFL, 0);
	fcntl(socket_can->socket_fd, F_SETFL, flags | O_NONBLOCK); //设置成非阻塞模式

	if(socket_can->interface_name == NULL)
	{
	    strcpy(ifr.ifr_name, "can0");
	}
	else
	{
	    strcpy(ifr.ifr_name, socket_can->interface_name);
	}

	if(SOCKET_CAN_EFAIL == ioctl(socket_can->socket_fd, SIOCGIFINDEX, &ifr))
	{
	    fprintf(stderr, "Error when open can interface!\n");
	    close(socket_can->socket_fd);
	    return SOCKET_CAN_EFAIL;
	}

	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	bind(socket_can->socket_fd, (struct sockaddr *)&addr, sizeof(addr));

	return SOCKET_CAN_OK;
}


/*******************************************************************************
 *  函数名: socket_can_deinit
 *  描述：socket CAN close
 *  输入：
 *    can: 为hal_can_t结构体指针
 *  输出：无
 *  返回：SOCKET_CAN_OK 表示正常初始化,SOCKET_CAN_EFAIL表示初始化失败
 *  备注：无
 *******************************************************************************/
int socket_can_deinit(hal_can_t *can)
{
	socket_can_t *socket_can = (socket_can_t *) can;

	close(socket_can->socket_fd);

	return SOCKET_CAN_OK;
}


/*******************************************************************************
 *  函数名: socket_can_set_filter
 *  描述：socket CAN 过滤器
 *  输入：
 *    can: 为hal_can_t结构体指针
 *    can_filter: CAN过滤器结构体指针
 *    n：num
 *  输出：无
 *  返回：SOCKET_CAN_OK 表示正常初始化,SOCKET_CAN_EFAIL表示初始化失败
 *  备注：无
 *******************************************************************************/
int socket_can_set_filter(hal_can_t *can, hal_can_filter_t *can_filter, int n)
{
	int return_value = 0;
	socket_can_t *socket_can = NULL;
	struct can_filter *can_filter_s = NULL;

	if(can == NULL)
	{
		Vps_printf("\r\n### socket_can_set_filter ### CAN not init !!!\r\n");
		return SOCKET_CAN_EFAIL;
	}

	socket_can = (socket_can_t *)can;

	if(n == 0)
	{
		return setsockopt(socket_can->socket_fd, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);
	}

	can_filter_s = (struct can_filter *)malloc(n * sizeof(struct can_filter));

	switch (can->id_type)
	{
		case CAN_ID_TYPE_STD:
			for (int i = 0; i < n; ++i) {
				can_filter_s[i].can_id = (can_filter[i].id & CAN_SFF_MASK);
				can_filter_s[i].can_mask = ((can_filter[i].mask & CAN_SFF_MASK) | CAN_EFF_FLAG | CAN_RTR_FLAG);
			}
			break;
		case CAN_ID_TYPE_EXT: //not tested
			for (int j = 0; j < n; ++j) {
				can_filter_s[j].can_id = ((can_filter[j].id & CAN_EFF_MASK) | CAN_EFF_FLAG);
				can_filter_s[j].can_mask = ((can_filter[j].mask & CAN_EFF_MASK) | CAN_EFF_FLAG | CAN_RTR_FLAG);
			}
			break;
	}

	return_value = setsockopt(socket_can->socket_fd,
				SOL_CAN_RAW,
				CAN_RAW_FILTER,
				&can_filter_s,
				sizeof(struct can_filter) * n);

	free(can_filter_s);

	return return_value;
}


/*******************************************************************************
 *  函数名: socket_can_read_msg
 *  描述：socket CAN read msg
 *  输入：
 *    can: 为hal_can_t结构体指针
 *    can_filter: CAN过滤器结构体指针
 *    n：num
 *  输出：读到的can msg 信息
 *  返回：SOCKET_CAN_OK 表示正常初始化,SOCKET_CAN_EFAIL表示初始化失败
 *  备注：无
 *******************************************************************************/
int socket_can_read_msg(hal_can_t *can, hal_can_msg_t *msg, int n)
{
    int read_n = n;
    ssize_t nbytes = 0;
    struct can_frame frame = {0};
    socket_can_t *socket_can = NULL;

	if(can == NULL)
	{
		Vps_printf("\r\n### socket_can_set_filter ### CAN not init !!!\r\n");
		return SOCKET_CAN_EFAIL;
	}

	socket_can = (socket_can_t *)can;

    for(int i = 0; i < n; ++i)
	{
        nbytes = read(socket_can->socket_fd, &frame, sizeof(struct can_frame));
        if(nbytes > 0)
		{
            msg[i].can_id = frame.can_id;
            msg[i].can_dlc = frame.can_dlc;
            memcpy(msg[i].data, frame.data, frame.can_dlc);
            gettimeofday(&msg[i].timestamp, NULL);
            read_n = nbytes / sizeof(struct can_frame);
            //ioctl(socket_can->socket_fd, SIOCGSTAMPNS, &msg[i].timestamp);
        }
		else
		{
            read_n = i;
            break;
        }
    }

    return read_n;
}


/*******************************************************************************
 *  函数名: socket_can_write_msg
 *  描述：socket CAN write msg
 *  输入：
 *    can: 为hal_can_t结构体指针
 *    can_filter: CAN过滤器结构体指针
 *    n：num
 *  输出：can msg
 *  返回：SOCKET_CAN_OK 表示正常初始化,SOCKET_CAN_EFAIL表示初始化失败
 *  备注：无
 *******************************************************************************/
int socket_can_write_msg(hal_can_t *can, const hal_can_msg_t *msg, int n)
{
    int write_n = 0;
    ssize_t nbytes = 0;
    struct can_frame frame = {0};
	socket_can_t *socket_can = NULL;

	if(can == NULL)
	{
		Vps_printf("\r\n### socket_can_set_filter ### CAN not init !!!\r\n");
		return SOCKET_CAN_EFAIL;
	}

    socket_can = (socket_can_t *)can;

    for (int i = 0; i < n; ++i)
	{
        frame.can_id = msg[i].can_id;
        frame.can_dlc = msg[i].can_dlc;
        memcpy(frame.data, msg[i].data, frame.can_dlc);
        nbytes = write(socket_can->socket_fd, &frame, sizeof(struct can_frame));
		if(nbytes <= 0)
		{
            write_n = i;
            break;
        }
    }

    return write_n;
}


/*******************************************************************************
 *  函数名: socket_can_set_block_state
 *  描述：socket CAN 设置CAN总线阻塞状态
 *  输入：
 *    can: 为hal_can_t结构体指针
 *    state: CAN 阻塞状态
 *  输出：无
 *  返回：SOCKET_CAN_OK 表示正常初始化,SOCKET_CAN_EFAIL表示初始化失败
 *  备注：无
 *******************************************************************************/
int socket_can_set_block_state(hal_can_t *can, hal_can_block_state_e state)
{
	int flags = 0;
    socket_can_t *socket_can = NULL;

	if(can == NULL)
	{
		Vps_printf("\r\n### socket_can_set_filter ### CAN not init !!!\r\n");
		return SOCKET_CAN_EFAIL;
	}

	flags = fcntl(socket_can->socket_fd, F_GETFL, 0);

	socket_can = (socket_can_t *)can;

    switch (state)
	{
        case CAN_BLOCK:
            flags &= ~(O_NONBLOCK);
            break;
        case CAN_UNBLOCK:
            flags |= O_NONBLOCK;
            break;
    }

    fcntl(socket_can->socket_fd, F_SETFL, flags);

    return 0;
}

