/******************************************************************************

Copyright (c) [2018 - 2019] MOMENTA Incorporated. All rights reserved.

******************************************************************************/


/*******************************************************************************
 *
 * 文件: socket_can.h
 *
 * 描述: 该文件提供SOCKET CAN模块的接口函数以及相应的结构体变量类型
 *
 ******************************************************************************/

#ifndef __SOCKET_CAN_H__
#define __SOCKET_CAN_H__


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************/
#include <sys/time.h>
#include "hal_can.h"

/*******************************************************************************
*  DEFINES
*  描述：The return status of the SOCKET CAN API.
*******************************************************************************/
#define SOCKET_CAN_OK              (0)
#define SOCKET_CAN_EFAIL           (-1)
#define SOCKET_CAN_ETIMEOUT        (-2)
#define SOCKET_CAN_EALLOC          (-3)
#define SOCKET_CAN_EBUSY           (-4)
#define SOCKET_CAN_EINVALID_PARAMS (-5)

int socket_can_init(hal_can_t *socket_can);
int socket_can_deinit(hal_can_t *socket_can);
int socket_can_read_msg(hal_can_t *socket_can, hal_can_msg_t *msg, int n);
int socket_can_write_msg(hal_can_t *socket_can, const hal_can_msg_t *msg, int n);
int socket_can_set_filter(hal_can_t *socket_can, hal_can_filter_t *can_filter, int n);
int socket_can_set_block_state(hal_can_t *socket_can, hal_can_block_state_e state);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif //__SOCKET_CAN_H__

