/******************************************************************************

Copyright (c) [2018 - 2019] MOMENTA Incorporated. All rights reserved.

******************************************************************************/


/*******************************************************************************
 *
 * 文件: hal_can.c
 *
 * 描述: 该文件提供HAL CAN模块的接口函数以及相应的结构体变量类型
 *
 ******************************************************************************/


/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <math.h>
#include <netinet/in.h>
#include <src/hlos/adas/include/chains.h>
#include <src/hlos/common/chains_common.h>
#include "hal_can.h"
#include "socket_can.h"
#include "../hal_camera/hal_camera.h"
#include "../hal_camera/hal_offline_feed.h"

extern hal_run_mode_e run_mode;

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
int hal_can_read_msg(hal_can_t *can, hal_can_msg_t *msg, int n)
{
	if(run_mode == HAL_MODE_OFFLINE_FEED)
	{
		return hal_offline_feed_can_read_msg(can, msg, n);
	}
	else
	{
	    assert(can != NULL);
	    assert(can->protocol.read != NULL);

	    return can->protocol.read(can, msg, n);
	}
}


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
int hal_can_write_msg(hal_can_t *can, const hal_can_msg_t *msg, int n)
{
    assert(can != NULL);
    assert(can->protocol.write != NULL);

    return can->protocol.write(can, msg, n);
}


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
int hal_can_set_filter(hal_can_t *can, hal_can_filter_t *filter, int n)
{
    assert(can != NULL);
    assert(can->protocol.set_filter != NULL);

    return can->protocol.set_filter(can, filter, n);
}


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
int hal_can_set_block_state(hal_can_t *can, hal_can_block_state_e state)
{
    assert(can != NULL);
    assert(can->protocol.set_block_state != NULL);

    return can->protocol.set_block_state(can, state);
}


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
				char bigend)
{
    uint64_t buf = 0;

    if (bigend)
	{
        for (int i = 0; i < 8; i++)
		{
            buf <<= 8;
            buf += msg->data[i];
        }
    }
	else
	{
        for (int i = 7; i >= 0; i--)
		{
            buf <<= 8;
            buf += msg->data[i];
        }
    }

    uint64_t mask = 0;

    assert(bit_len <= 64);
    if (bit_len == 64)
	{
        mask = ~0ULL;
    }
	else
	{
        mask = (1ULL << bit_len) - 1;
    }

    uint32_t lsb = start_bit;
    if (bigend)
	{
        int msb = start_bit % 8 + (7 - (start_bit / 8)) * 8;
        lsb = msb - (bit_len - 1);
    }

    int64_t raw_result = (buf >> lsb) & mask;

    /*
    if (is_signed)
    {
        if (raw_result & (1ULL << (bit_len - 1)))
        {
            raw_result -= (1ULL << bit_len);
        }
    }*/

    return raw_result;
}


/*******************************************************************************
 *  函数名：hal_can_pack_bits
 *  描述：从msg中提取比特位
 *  输入：
 *      msg:canbus消息
 *      start_bit:起始位
 *      data：要打包的比特位数据
 *      bit_len:比特位长度
 *      bigend:大小端格式
 *  输出：无
 *  返回：无
 *  备注：无
 *******************************************************************************/
void hal_can_pack_bits (hal_can_msg_t *msg,
				unsigned int start_bit,
				unsigned int bit_len,
				unsigned long int data,
				char bigend)
{
    uint64_t buf = 0;

    if (bigend)
	{
        for (int i = 0; i < 8; i++)
		{
            buf <<= 8;
            buf += msg->data[i];
        }
    }
	else
	{
        for (int i = 7; i >= 0; i--)
		{
            buf <<= 8;
            buf += msg->data[i];
        }
    }

    uint64_t mask = 0;
    assert(bit_len <= 64);

	if (bit_len == 64)
	{
        mask = ~0ULL;
    }
	else
	{
        mask = (1ULL << bit_len) - 1;
    }

    uint64_t data_raw = data;

    if (data_raw < 0)
	{
        data_raw = (1ULL << bit_len) + data_raw;
    }

    data_raw &= mask;

    uint32_t lsb = start_bit;

    if (bigend)
	{
        int msb = start_bit % 8 + (7 - (start_bit / 8)) * 8;
        lsb = msb - (bit_len - 1);
    }

    buf += data_raw << lsb;

    if (bigend)
	{
        for (int i = 7; i >= 0; i--)
		{
            msg->data[i] = (uint8_t)(buf & 0xff);
            buf >>= 8;
        }
    }
	else
	{
        for (int i = 0; i < 8; i++)
		{
            msg->data[i] = (uint8_t)(buf & 0xff);
            buf >>= 8;
        }
    }
}


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
int hal_can_init(hal_can_t *can)
{
    assert(can != NULL);
	if(run_mode == HAL_MODE_OFFLINE_FEED)
	{
		return 0;
	}
	else
	{
	    can->protocol.init = socket_can_init;
	    can->protocol.deinit = socket_can_deinit;
	    can->protocol.read = socket_can_read_msg;
	    can->protocol.write = socket_can_write_msg;
	    can->protocol.set_filter = socket_can_set_filter;
	    can->protocol.set_block_state = socket_can_set_block_state;

	    return can->protocol.init(can);
	}
}


/*******************************************************************************
 *  函数名：hal_can_deinit
 *  描述：hal层关闭CAN总线设备时申请资源以及配置初始化参数
 *  输入：
 *      can：canbus接口上下文
 *  输出：无
 *  返回：成功返回0, 失败返回-1
 *  备注：无
 *******************************************************************************/
int hal_can_deinit(hal_can_t *can)
{
    assert(can != NULL);
    assert(can->protocol.deinit != NULL);

    return can->protocol.deinit(can);
}

#if 0
int hal_offline_feed_socket_can(char *file_path)
{
	static char buf[BUFSZ], device[BUFSZ], ascframe[BUFSZ];
	static struct canfd_frame frame;
	static struct timeval today_tv, log_tv, last_log_tv, diff_tv;
	struct timespec sleep_ts;
	FILE *infile = stdin;
	unsigned long gap = DEFAULT_GAP; 
	int use_timestamps = 1;
	static int verbose, delay_loops;
	static unsigned long skipgap;
	static int infinite_loops = 0;
	static int loops = DEFAULT_LOOPS;
	static int index_w = 0;
	int eof, txmtu;
	char *fret;

	//infile = fopen(offline_candump_file_path, "r");
	infile = fopen("candump-2019-01-14_025801.log", "r");
	if (!infile) {
		perror("infile");
		return 1;
	}
	
	if (verbose > 1) { /* use -v -v to see this */
		if (infinite_loops)
			printf("infinite_loops\n");
		else
			printf("%d loops\n", loops);
	}

	sleep_ts.tv_sec  =  gap / 1000;
	sleep_ts.tv_nsec = (gap % 1000) * 1000000;

	while (infinite_loops || loops--) {

		if (verbose > 1) /* use -v -v to see this */
			printf (">>>>>>>>> start reading file. remaining loops = %d\n", loops);

		/* read first non-comment frame from logfile */
		while ((fret = fgets(buf, BUFSZ-1, infile)) != NULL && buf[0] != '(') {
			if (strlen(buf) >= BUFSZ-2) {
				fprintf(stderr, "comment line too long for input buffer\n");
				return 1;
			}
		}

		if (!fret)
			goto out; /* nothing to read */

		eof = 0;

		if (sscanf(buf, "(%ld.%ld) %s %s", &log_tv.tv_sec, &log_tv.tv_usec,
			   device, ascframe) != 4) {
			fprintf(stderr, "incorrect line format in logfile\n");
			return 1;
		}
			   HAL_DEBUG("[wagtest]%s:%d \n", __func__, __LINE__);

		if (use_timestamps) { /* throttle sending due to logfile timestamps */
			HAL_DEBUG("[wagtest]%s:%d \n", __func__, __LINE__);

			gettimeofday(&today_tv, NULL);
			create_diff_tv(&today_tv, &diff_tv, &log_tv);
			last_log_tv = log_tv;
		}

		while (!eof) {
			while ((!use_timestamps) ||
			       (frames_to_send(&today_tv, &diff_tv, &log_tv) < 0)) {

				/* log_tv/device/ascframe are valid here */

				if (strlen(device) >= IFNAMSIZ) {
					fprintf(stderr, "log interface name '%s' too long!", device);
					return 1;
				}
				txmtu = parse_canframe(ascframe, &frame);
				if (!txmtu) {
					fprintf(stderr, "wrong CAN frame format: '%s'!", ascframe);
					return 1;
				}
#if 0
				addr.can_family  = AF_CAN;
				addr.can_ifindex = txidx; /* send via this interface */

				if (sendto(s, &frame, txmtu, 0,	(struct sockaddr*)&addr, sizeof(addr)) != txmtu) {
					perror("sendto");
					return 1;
				}
#else
				//
				pthread_mutex_lock(&can_mutex);
				while(array_is_full == 1)
				{
					HAL_DEBUG("[wagtest]%s:%d \n", __func__, __LINE__);
					pthread_cond_wait(&can_cond_w, &can_mutex);
				}
				msg_array[index_w].can_dlc = frame.len;
				msg_array[index_w].can_id = frame.can_id;
				memcpy(msg_array[index_w].data, frame.data, frame.len);
				msg_array[index_w].timestamp = log_tv;
				HAL_DEBUG("[wagtest]%s:%d id=%3x, can_dlc=%d, sec=%ld, usec=%6ld\n", __func__, __LINE__, 
					msg_array[index_w].can_id,
					msg_array[index_w].can_dlc, 
					msg_array[index_w].timestamp.tv_sec,
					msg_array[index_w].timestamp.tv_usec);
				
				index_w++;
				if(index_w >= CAN_MSG_ARRAY_SIZE )
				{
					index_w = 0;
					array_is_full = 1;
					HAL_DEBUG("[wagtest]%s:%d \n", __func__, __LINE__);
					pthread_cond_signal(&can_cond_r);
					HAL_DEBUG("[wagtest]%s:%d \n", __func__, __LINE__);
				}
				pthread_mutex_unlock(&can_mutex);
#endif
				/* read next non-comment frame from logfile */
				while ((fret = fgets(buf, BUFSZ-1, infile)) != NULL && buf[0] != '(') {
					if (strlen(buf) >= BUFSZ-2) {
						fprintf(stderr, "comment line too long for input buffer\n");
						return 1;
					}
				}

				if (!fret) {
					eof = 1; /* this file is completely processed */
					break;
				}
				HAL_DEBUG("[wagtest]%s:%d %s\n", __func__, __LINE__, buf);

				if (sscanf(buf, "(%ld.%ld) %s %s", &log_tv.tv_sec, &log_tv.tv_usec,
					   device, ascframe) != 4) {
					fprintf(stderr, "incorrect line format in logfile\n");
					return 1;
				}

				/*
				 * ensure the fractions of seconds are 6 decimal places long to catch
				 * 3rd party or handcrafted logfiles that treat the timestamp as float
				 */
				if (strchr(buf, ')') - strchr(buf, '.') != 7) {
					fprintf(stderr, "timestamp format in logfile requires 6 decimal places\n");
					return 1;
				}

				if (use_timestamps) {
					gettimeofday(&today_tv, NULL);

					/* test for logfile timestamps jumping backwards OR      */
					/* if the user likes to skip long gaps in the timestamps */
					if ((last_log_tv.tv_sec > log_tv.tv_sec) ||
					    (skipgap && labs(last_log_tv.tv_sec - log_tv.tv_sec) > skipgap))
						create_diff_tv(&today_tv, &diff_tv, &log_tv);

					last_log_tv = log_tv;
				}

			} /* while frames_to_send ... */

			if (nanosleep(&sleep_ts, NULL))
				return 1;

			delay_loops++; /* private statistics */
			gettimeofday(&today_tv, NULL);

		} /* while (!eof) */

	} /* while (infinite_loops || loops--) */

out:

	fclose(infile);

	if (verbose > 1) /* use -v -v to see this */
		printf("%d delay_loops\n", delay_loops);

	return 0;
}
#endif

