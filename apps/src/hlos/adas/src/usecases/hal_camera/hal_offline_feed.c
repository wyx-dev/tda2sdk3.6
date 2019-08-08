#include "camera_base.h"
#include "can.h"
#include "lib.h"
#include <net/if.h>
#include "hal_offline_feed.h"
#include "ringbuffer.h"
//#define __DEBUG
#ifdef __DEBUG
#define HAL_DEBUG(args...) \
 	do {						\
			Vps_printf(args);		\
	} while(0)
#else
#define HAL_DEBUG(args...) do { } while(0)
#endif

#define DEFAULT_GAP	1	/* ms */
#define DEFAULT_LOOPS	1	/* only one replay */
#define CHANNELS	20	/* anyone using more than 20 CAN interfaces at a time? */
#define COMMENTSZ 200
#define BUFSZ (sizeof("(1345212884.318850)") + IFNAMSIZ + 4 + CL_CFSZ + COMMENTSZ) /* for one line in the logfile */
#define STDOUTIDX	65536	/* interface index for printing on stdout - bigger than max uint16 */
#define CANID_DELIM '#'
#define DATA_SEPERATOR '.'

#define CAN_MSG_QUEUE_SIZE 4096
#define CAN_MSG_ARRAY_SIZE 10

pthread_mutex_t can_mutex;
pthread_mutex_t feed_mutex;
pthread_cond_t can_cond_r, can_cond_w;

char offline_image_path[128];
char offline_candump_file_path[128];
FILE *offline_filelist_fp = NULL;
struct timeval diff_tv;
hal_offline_feed_framerate_e feed_framerate;

int fps_usec[HAL_FEED_FRAMERATE_INVALID] = {60000, 50000, 38000, 33000};

int msg_index = 0;
int array_is_full = 0;
hal_can_msg_t msg_array[CAN_MSG_ARRAY_SIZE];
struct ringbuffer *can_ring_buf;
FILE *canfile_fp = NULL;
int feed_line = 0;
int recv_line = 0;


#define NULLSRC_OUT_WIDTH           (1280)
#define NULLSRC_OUT_HEIGHT          (720)
//#define OFFLINE_FILE_SIZE (NULLSRC_OUT_WIDTH*NULLSRC_OUT_HEIGHT*2)
#define OFFLINE_FRAMERATE 5
#define TIME_STEP 1
//#define OFFLINE_FILE_BUF_SIZE (OFFLINE_FRAMERATE * TIME_STEP)
#define OFFLINE_FILE_BUF_SIZE 50

#define OFFLINE_CONTINUE 1
#define OFFLINE_DONE 0
#define DEFAULT_GAP	1	/* ms */
#define OFFLINE_FILE_SIZE (1024*1024*2)

typedef struct hal_offline_filebuf
{
	struct timeval timestamp;
	char buf[OFFLINE_FILE_SIZE];
} hal_offline_filebuf_t;

hal_offline_filebuf_t offline_file_buf[OFFLINE_FILE_BUF_SIZE];
hal_offline_filebuf_t offline_blank_file_buf;

void hal_feed_fillSrcBuf(chains_hal_offline_feed_cam_vpe_sadObj *pObj);

/*******************************************************************************
*  函数名：hal_cam_offline_feed_restart_signal
*  描述：向NullSrcLink发送restart signal 用于重新启动chains
*  输入：无
*  输出：无
*  返回：成功返回0, 失败返回<0
*  备注：
*******************************************************************************/
int hal_cam_offline_feed_restart_signal(void)
{
	chains_hal_offline_feed_cam_vpe_sad_nullsrc_restart(g_camera_save_local_obj.hal_offline_feed_cam_vpe_sad_AppObj);
	return 0;
}


/*******************************************************************************
*  函数名：hal_cam_offline_feed_pause_signal
*  描述：向NullSrcLink发送pause signal 用于阻塞Chains
*  输入：无
*  输出：无
*  返回：成功返回0, 失败返回<0
*  备注：
*******************************************************************************/
int hal_cam_offline_feed_pause_signal(void)
{
	chains_hal_offline_feed_cam_vpe_sad_nullsrc_pause(g_camera_save_local_obj.hal_offline_feed_cam_vpe_sad_AppObj);
	return 0;
}

/*******************************************************************************
 *  函数名：hal_offline_feed_cam_deinit
 *  描述：hal层关闭 hal offline camera设备时释放初始化申请的资源,确保camera正常退出
 *  输入：无
 *  输出：无
 *  返回：成功返回0, 失败返回<0
 *  备注：无，只提供1路camera测试
 *******************************************************************************/
int hal_offline_feed_cam_deinit()
{
	int ret = 0;

	ret = chains_hal_offline_feed_cam_vpe_sad_StopApp(g_camera_save_local_obj.hal_offline_feed_cam_vpe_sad_AppObj);
	if (ret < 0)
	{
		Vps_printf("\r\n### ERROR ### hal_offline_feed_cam_deinit ### chains_hal_offline_feed_cam_vpe_sad_StopApp !!!\r\n");
		return HAL_CAMERA_EFAIL;
	}

	return HAL_CAMERA_OK;
}
static int hal_get_offlineimage_first_timestamp(struct timeval *timestamp)
{
	char file_name[128];

	fseek(offline_filelist_fp, 0, SEEK_SET);
	if (!feof(offline_filelist_fp))
	{
		fscanf(offline_filelist_fp, "%s", file_name);
		sscanf(file_name, "%ld.%ld.raw", &timestamp->tv_sec, &timestamp->tv_usec);
	}

	return 0;
}

/* copied from /usr/src/linux/include/linux/time.h ...
 * lhs < rhs:  return <0
 * lhs == rhs: return 0
 * lhs > rhs:  return >0
 */
static inline int timeval_compare(struct timeval *lhs, struct timeval *rhs)
{
	if (lhs->tv_sec < rhs->tv_sec)
		return -1;
	if (lhs->tv_sec > rhs->tv_sec)
		return 1;
	return lhs->tv_usec - rhs->tv_usec;
}
static inline void create_diff_tv(struct timeval *today, struct timeval *diff,
                                  struct timeval *log)
{

	/* create diff_tv so that log_tv + diff_tv = today_tv */
	diff->tv_sec  = today->tv_sec  - log->tv_sec;
	diff->tv_usec = today->tv_usec - log->tv_usec;
}
static inline void compensation_diff_tv(struct timeval *start, struct timeval *diff,
                                  struct timeval *end)
{
	diff->tv_sec += end->tv_sec  - start->tv_sec;
	diff->tv_usec += end->tv_usec - start->tv_usec;
}

/*time1 - time2 = ?*/
static inline long long timeval_diff(struct timeval *time1, struct timeval *time2)
{
	unsigned long long time1_tt, time2_tt;

	time1_tt = 1000000 * (long long)time1->tv_sec + (long long)time1->tv_usec;
	time2_tt = 1000000 * (long long)time2->tv_sec + (long long)time2->tv_usec;
	return time1_tt - time2_tt;
}

static inline int frames_to_send(struct timeval *today, struct timeval *diff,
                                 struct timeval *log)
{
	/* return value <0 when log + diff < today */

	struct timeval cmp;

	cmp.tv_sec  = log->tv_sec  + diff->tv_sec;
	cmp.tv_usec = log->tv_usec + diff->tv_usec;

	if (cmp.tv_usec > 1000000)
	{
		cmp.tv_usec -= 1000000;
		cmp.tv_sec++;
	}

	if (cmp.tv_usec < 0)
	{
		cmp.tv_usec += 1000000;
		cmp.tv_sec--;
	}

	return timeval_compare(&cmp, today);
}
unsigned char asc2nibble(char c)
{

	if ((c >= '0') && (c <= '9'))
		return c - '0';

	if ((c >= 'A') && (c <= 'F'))
		return c - 'A' + 10;

	if ((c >= 'a') && (c <= 'f'))
		return c - 'a' + 10;

	return 16; /* error */
}
int parse_canframe(char *cs, struct canfd_frame *cf)
{
	/* documentation see lib.h */

	int i, idx, dlen, len;
	int maxdlen = CAN_MAX_DLEN;
	int ret = CAN_MTU;
	unsigned char tmp;

	len = strlen(cs);

	memset(cf, 0, sizeof(*cf)); /* init CAN FD frame, e.g. LEN = 0 */

	if (len < 4)
		return 0;

	if (cs[3] == CANID_DELIM)   /* 3 digits */
	{

		idx = 4;
		for (i = 0; i < 3; i++)
		{
			if ((tmp = asc2nibble(cs[i])) > 0x0F)
				return 0;
			cf->can_id |= (tmp << (2 - i) * 4);
		}

	}
	else if (cs[8] == CANID_DELIM)     /* 8 digits */
	{

		idx = 9;
		for (i = 0; i < 8; i++)
		{
			if ((tmp = asc2nibble(cs[i])) > 0x0F)
				return 0;
			cf->can_id |= (tmp << (7 - i) * 4);
		}
		if (!(cf->can_id & CAN_ERR_FLAG)) /* 8 digits but no errorframe?	*/
			cf->can_id |= CAN_EFF_FLAG;	/* then it is an extended frame */

	}
	else
		return 0;

	if ((cs[idx] == 'R') || (cs[idx] == 'r')) /* RTR frame */
	{
		cf->can_id |= CAN_RTR_FLAG;

		/* check for optional DLC value for CAN 2.0B frames */
		if (cs[++idx] && (tmp = asc2nibble(cs[idx])) <= CAN_MAX_DLC)
			cf->len = tmp;

		return ret;
	}

	if (cs[idx] == CANID_DELIM)   /* CAN FD frame escape char '##' */
	{

		maxdlen = CANFD_MAX_DLEN;
		ret = CANFD_MTU;

		/* CAN FD frame <canid>##<flags><data>* */
		if ((tmp = asc2nibble(cs[idx + 1])) > 0x0F)
			return 0;

		cf->flags = tmp;
		idx += 2;
	}

	for (i = 0, dlen = 0; i < maxdlen; i++)
	{

		if (cs[idx] == DATA_SEPERATOR) /* skip (optional) separator */
			idx++;

		if (idx >= len) /* end of string => end of data */
			break;

		if ((tmp = asc2nibble(cs[idx++])) > 0x0F)
			return 0;
		cf->data[i] = (tmp << 4);
		if ((tmp = asc2nibble(cs[idx++])) > 0x0F)
			return 0;
		cf->data[i] |= tmp;
		dlen++;
	}
	cf->len = dlen;

	return ret;
}

int hal_get_candump_first_timestamp(struct timeval *timestamp)
{
	FILE *fp = NULL;
	char *fret;
	char buf[BUFSZ], device[BUFSZ], ascframe[BUFSZ];
	fp = fopen(offline_candump_file_path, "r");
	if (!fp)
	{
		printf("open %s failed!\n", offline_candump_file_path);
		return 1;
	}
	/* read first non-comment frame from logfile */
	while ((fret = fgets(buf, BUFSZ - 1, fp)) != NULL && buf[0] != '(')
	{
		if (strlen(buf) >= BUFSZ - 2)
		{
			fprintf(stderr, "comment line too long for input buffer\n");
			fclose(fp);
			return 1;
		}
	}

	if (!fret)
	{
		fclose(fp);
		return 1; /* nothing to read */
	}

	if (sscanf(buf, "(%ld.%ld) %s %s", &timestamp->tv_sec, &timestamp->tv_usec,
	           device, ascframe) != 4)
	{
		fprintf(stderr, "incorrect line format in logfile\n");
		fclose(fp);
		return 1;
	}
	fclose(fp);
	return 0;

}
int hal_offline_feed_can_init()
{
	if (pthread_mutex_init(&can_mutex, NULL))
	{
		Vps_printf("\r\n### hal_offline_feed_can_init ### pthread_mutex_init Error !\r\n");
		return 1;
	}
	if (pthread_cond_init(&can_cond_r, NULL))
	{
		Vps_printf("\r\n### hal_offline_feed_can_init ### pthread_cond_init Error !\r\n");
		return 1;
	}
	if (pthread_cond_init(&can_cond_w, NULL))
	{
		Vps_printf("\r\n### hal_offline_feed_can_init ### pthread_cond_init Error !\r\n");
		return 1;
	}
	HAL_DEBUG("[haltest]%s:%d candump file is %s\n", __func__, __LINE__, offline_candump_file_path);
	canfile_fp = fopen(offline_candump_file_path, "r");
	if (!canfile_fp)
	{
		perror("Open the candump file failed\n");
		return 1;
	}
	//sizeof(hal_can_msg_t) == 24,but ringbuffer size must be power of 2, so use 32.
	can_ring_buf = ringbuffer_create(CAN_MSG_QUEUE_SIZE*32);
	
	return 0;
}

static int find_min_timestamp(struct timeval *timestamp)
{
	int ret = 0;
	struct timeval image_timestamp, can_timestamp;

	if (hal_get_candump_first_timestamp(&can_timestamp))
	{
		Vps_printf("Can not get first candump  timestamp\n");
		return 1;
	}

	if (hal_get_offlineimage_first_timestamp(&image_timestamp))
	{
		Vps_printf("Can not get first offline image timestamp\n");
		return 1;
	}
	ret = timeval_compare(&can_timestamp, &image_timestamp);

	if (ret < 0)
	{
		timestamp->tv_sec = can_timestamp.tv_sec;
		timestamp->tv_usec = can_timestamp.tv_usec;
	}
	else
	{
		timestamp->tv_sec = image_timestamp.tv_sec;
		timestamp->tv_usec = image_timestamp.tv_usec;
	}

	return 0;

}
static void hal_read_file(char *path, char *buf, int file_size)
{
	FILE *fp = fopen(path, "r");

	if (fp)
	{
		fread(buf, 1, file_size, fp);
	}
	fflush(fp);
	fclose(fp);
}

static int hal_load_offline_file()
{
	static unsigned long long line = 0;
	int buf_index = 0;
	struct timeval timestamp, time_diff, time_tmp;
	char path[256];
	char file_name[128];
	static hal_offline_filebuf_t last_buf;

	memset(path, 0, 256);
	memset(file_name, 0, 128);
	memset(offline_file_buf, 0, 50 * sizeof(hal_offline_filebuf_t));

	if (line > 0)
	{
		memcpy(&offline_file_buf[buf_index], &last_buf,  sizeof(hal_offline_filebuf_t));
		buf_index++;
	}
	else
	{
		if(!feof(offline_filelist_fp))
		{
			fscanf(offline_filelist_fp, "%s", file_name);
			sscanf(file_name, "%ld.%ld.raw", &timestamp.tv_sec, &timestamp.tv_usec);
			sprintf(path, "%s/%s", offline_image_path, file_name);
			HAL_DEBUG("[haltest]%s:%d line=%lld, path=%s\n", __func__, __LINE__, line, path);
			hal_read_file(path, offline_file_buf[buf_index].buf, OFFLINE_FILE_SIZE);
			offline_file_buf[buf_index].timestamp = timestamp;
			memcpy(offline_file_buf[buf_index].buf, &timestamp, sizeof(struct timeval));
			memcpy(&last_buf, &offline_file_buf[buf_index], sizeof(hal_offline_filebuf_t));
			last_buf.timestamp = timestamp;
			buf_index++;
			line++;
		}
	}

	while (!feof(offline_filelist_fp))
	{
		fscanf(offline_filelist_fp, "%s", file_name);
		if (!feof(offline_filelist_fp))
		{
			sscanf(file_name, "%ld.%ld.raw", &timestamp.tv_sec, &timestamp.tv_usec);
			sprintf(path, "%s/%s", offline_image_path, file_name);
			HAL_DEBUG("[haltest]%s:%d line=%lld, path=%s\n", __func__, __LINE__, line, path);
			
			timersub(&timestamp, &last_buf.timestamp, &time_diff);
			
			/*Whether some blank frames need to be inserted*/
			while((time_diff.tv_sec*1000000 + time_diff.tv_usec) >= fps_usec[feed_framerate])
			{
				memcpy(offline_file_buf[buf_index].buf,offline_blank_file_buf.buf, OFFLINE_FILE_SIZE);
				//memset(offline_file_buf[buf_index].buf, 0, OFFLINE_FILE_SIZE);
				time_tmp.tv_sec = 0;
				time_tmp.tv_usec = fps_usec[feed_framerate];
				
				timeradd(&last_buf.timestamp, &time_tmp, &offline_file_buf[buf_index].timestamp);
				//Vps_printf("[haltest]%s:%d line=%lld,insert blank frame, preframe=%ld.%.6ld\n", __func__, __LINE__, line, last_buf.timestamp.tv_sec, last_buf.timestamp.tv_usec);
				//Vps_printf("[haltest]%s:%d line=%lld,insert blank frame, insertframe=%ld.%.6ld\n", __func__, __LINE__, line, offline_file_buf[buf_index].timestamp.tv_sec, offline_file_buf[buf_index].timestamp.tv_usec);
				//Vps_printf("[haltest]%s:%d line=%lld,insert blank frame, afterframe=%ld.%.6ld\n", __func__, __LINE__, line, timestamp.tv_sec, timestamp.tv_usec);
				memcpy(offline_file_buf[buf_index].buf, &offline_file_buf[buf_index].timestamp, sizeof(struct timeval));
				last_buf.timestamp = offline_file_buf[buf_index].timestamp;
				timersub(&timestamp, &last_buf.timestamp, &time_diff);
				buf_index++;
			}
			
			hal_read_file(path, offline_file_buf[buf_index].buf, OFFLINE_FILE_SIZE);
			offline_file_buf[buf_index].timestamp = timestamp;
			memcpy(offline_file_buf[buf_index].buf, &timestamp, sizeof(struct timeval));
			last_buf.timestamp = timestamp;
			if (timeval_diff(&offline_file_buf[buf_index].timestamp, &offline_file_buf[0].timestamp) >= TIME_STEP * 1000000)
			{
				HAL_DEBUG("time span is %lld\n", timeval_diff(&offline_file_buf[buf_index].timestamp, &offline_file_buf[0].timestamp));
				memcpy(last_buf.buf, offline_file_buf[buf_index].buf, OFFLINE_FILE_SIZE);
				last_buf.timestamp = timestamp;

				offline_file_buf[buf_index].timestamp.tv_sec = 0;
				offline_file_buf[buf_index].timestamp.tv_usec = 0;
				line++;
				return OFFLINE_CONTINUE;
			}
			buf_index++;
			line++;
		}
	}

	return OFFLINE_DONE;

}

int hal_offline_feed_image()
{
	static struct timeval today_tv;
	struct timeval start_tv, end_tv;
	static int feed_index = 0;
	//struct timespec sleep_ts;

	//sleep_ts.tv_sec  =	0;
	//sleep_ts.tv_nsec =  1000000;

	if ((offline_file_buf[feed_index].timestamp.tv_usec != 0) ||
	        (offline_file_buf[feed_index].timestamp.tv_sec != 0))
	{		
		gettimeofday(&today_tv, NULL);
		//HAL_DEBUG("[haltest]%s:%d today.sec=%ld today.usec=%ld\n", __func__, __LINE__, today_tv.tv_sec, today_tv.tv_usec);
		if (frames_to_send(&today_tv, &diff_tv, &offline_file_buf[feed_index].timestamp) < 0)
		{
			HAL_DEBUG("[haltest]%s:%d feed image line=%d sec=%ld usec=%ld\n", __func__, __LINE__,
			          feed_line,
			          offline_file_buf[feed_index].timestamp.tv_sec,
			          offline_file_buf[feed_index].timestamp.tv_usec);
			gettimeofday(&start_tv, NULL);
			hal_cam_offline_feed_pause_signal();
			g_camera_save_local_obj.offlineDataFileName = offline_file_buf[feed_index].buf;

			hal_feed_fillSrcBuf(&g_camera_save_local_obj.hal_offline_feed_cam_vpe_sad_AppObj->ucObj);

			hal_cam_offline_feed_restart_signal();
			gettimeofday(&end_tv, NULL);
			//HAL_DEBUG("[haltest]%s:%d feed image time usec=%ld\n", __func__, __LINE__,
			//          (end_tv.tv_sec - start_tv.tv_sec)*1000000 + (end_tv.tv_usec - start_tv.tv_usec));
			feed_index++;
			//pthread_mutex_lock(&feed_mutex);
			feed_line++;
			//pthread_mutex_unlock(&feed_mutex);
			#if 0
			while(1)
			{
				pthread_mutex_lock(&feed_mutex);
				if(feed_line == recv_line)
				{
					pthread_mutex_unlock(&feed_mutex);
					break;
				}
				pthread_mutex_unlock(&feed_mutex);
				nanosleep(&sleep_ts, NULL);
			}
			gettimeofday(&end_tv, NULL);
			compensation_diff_tv(&today_tv, &diff_tv, &end_tv);
			#endif
		}

	}
	else
	{
		feed_index = 0;
		offline_file_buf[feed_index].timestamp.tv_sec = 0;
		offline_file_buf[feed_index].timestamp.tv_usec = 0;
		return HAL_FEED_NEED_LOAD_IMAGES;//need load new images from SSD
	}
	return HAL_FEED_OK;
}
int hal_offline_feed_can_read_msg(hal_can_t *can, hal_can_msg_t *msg, int n)
{
#if 0
	static int index_r = 0;

	pthread_mutex_lock(&can_mutex);
	while (array_is_full == 0)
	{
		pthread_cond_wait(&can_cond_r, &can_mutex);
	}
	msg->can_dlc = msg_array[index_r].can_dlc;
	msg->can_id = msg_array[index_r].can_id;
	memcpy(msg->data, msg_array[index_r].data, 8);
	msg->timestamp = msg_array[index_r].timestamp;
	index_r++;
	if (index_r >= CAN_MSG_ARRAY_SIZE)
	{
		index_r = 0;
		array_is_full = 0;
		memset(msg_array, 0, CAN_MSG_ARRAY_SIZE * sizeof(hal_can_msg_t));
		pthread_cond_signal(&can_cond_w);
	}
	pthread_mutex_unlock(&can_mutex);
#else
	if (ringbuffer_is_empty(can_ring_buf)) {
		//HAL_DEBUG("[haltest]%s:%d can ring buffer is empty !\n", __func__, __LINE__);
		return 0;
	}

	ringbuffer_get(can_ring_buf, (char*)msg, sizeof(hal_can_msg_t));
	return 1;
#endif
	//return n;
}

int hal_offline_feed_can(char *ascframe, struct timeval *log_tv)
{
	static int index_w = 0;
	int txmtu;
	struct canfd_frame frame;
	hal_can_msg_t msg;

	txmtu = parse_canframe(ascframe, &frame);
	if (!txmtu)
	{
		fprintf(stderr, "wrong CAN frame format: '%s'!", ascframe);
		return HAL_FEED_ERROR;
	}
#if 0
	pthread_mutex_lock(&can_mutex);
	while (array_is_full == 1)
	{
		pthread_cond_wait(&can_cond_w, &can_mutex);
	}
	msg_array[index_w].can_dlc = frame.len;
	msg_array[index_w].can_id = frame.can_id;
	memcpy(msg_array[index_w].data, frame.data, frame.len);
	msg_array[index_w].timestamp = *log_tv;
	/*HAL_DEBUG("[haltest]%s:%d id=%3x, can_dlc=%d, sec=%ld, usec=%6ld\n", __func__, __LINE__,
		msg_array[index_w].can_id,
		msg_array[index_w].can_dlc,
		msg_array[index_w].timestamp.tv_sec,
		msg_array[index_w].timestamp.tv_usec);*/

	index_w++;
	if (index_w >= CAN_MSG_ARRAY_SIZE)
	{
		index_w = 0;
		array_is_full = 1;
		pthread_cond_signal(&can_cond_r);
	}
	pthread_mutex_unlock(&can_mutex);
#else
	msg.can_dlc = frame.len;
	msg.can_id = frame.can_id;
	memcpy(msg.data, frame.data, frame.len);
	msg.timestamp = *log_tv;

	if (ringbuffer_is_full(can_ring_buf)) {
		HAL_DEBUG("[haltest]%s:%d can ring buffer is full !\n", __func__, __LINE__);
		return HAL_FEED_ERROR;
	}
	ringbuffer_put(can_ring_buf, (const char *)&msg, sizeof(hal_can_msg_t));

#endif
	return HAL_FEED_OK;
}
int hal_offline_feed_image_can(int needloadimage)
{
	static char buf[BUFSZ], device[BUFSZ], ascframe[BUFSZ];
	static struct timeval today_tv, log_tv;
	struct timespec sleep_ts;
	char *fret;
	unsigned long gap = DEFAULT_GAP;

	sleep_ts.tv_sec  =	gap / 1000;
	sleep_ts.tv_nsec = (gap % 1000) * 1000000;

	if (!feof(canfile_fp))
	{
		if ((log_tv.tv_usec == 0) && (log_tv.tv_sec == 0)) /* The first line of candump file.*/
		{
			/* read first non-comment frame from logfile */
			while ((fret = fgets(buf, BUFSZ - 1, canfile_fp)) != NULL && buf[0] != '(')
			{
				if (strlen(buf) >= BUFSZ - 2)
				{
					fprintf(stderr, "comment line too long for input buffer\n");
					return HAL_FEED_ERROR;
				}
			}

			if (!fret)
				return HAL_FEED_ERROR; /* nothing to read */

			if (sscanf(buf, "(%ld.%ld) %s %s", &log_tv.tv_sec, &log_tv.tv_usec,
			           device, ascframe) != 4)
			{
				fprintf(stderr, "incorrect line format in logfile\n");
				return HAL_FEED_ERROR;
			}
		}

		gettimeofday(&today_tv, NULL);

		while (!feof(canfile_fp))
		{
			//if (frames_to_send(&today_tv, &diff_tv, &log_tv) < 0)
			while (frames_to_send(&today_tv, &diff_tv, &log_tv) < 0)
			{
				/* log_tv/device/ascframe are valid here */

				if (strlen(device) >= IFNAMSIZ)
				{
					fprintf(stderr, "log interface name '%s' too long!", device);
					return HAL_FEED_ERROR;
				}

				hal_offline_feed_can(ascframe, &log_tv);

				/* read next non-comment frame from logfile */
				while ((fret = fgets(buf, BUFSZ - 1, canfile_fp)) != NULL && buf[0] != '(')
				{
					if (strlen(buf) >= BUFSZ - 2)
					{
						fprintf(stderr, "comment line too long for input buffer\n");
						return HAL_FEED_ERROR;
					}
				}

				if (!fret)
				{
					break;/* this file is completely processed */
				}

				if (sscanf(buf, "(%ld.%ld) %s %s", &log_tv.tv_sec, &log_tv.tv_usec,
				           device, ascframe) != 4)
				{
					fprintf(stderr, "incorrect line format in logfile\n");
					return HAL_FEED_ERROR;
				}

				/*
				 * ensure the fractions of seconds are 6 decimal places long to catch
				 * 3rd party or handcrafted logfiles that treat the timestamp as float
				 */
				if (strchr(buf, ')') - strchr(buf, '.') != 7)
				{
					fprintf(stderr, "timestamp format in logfile requires 6 decimal places\n");
					return HAL_FEED_ERROR;
				}

				gettimeofday(&today_tv, NULL);

			} /* while frames_to_send ... */

			if (hal_offline_feed_image() == HAL_FEED_NEED_LOAD_IMAGES)
			{
				if (needloadimage)
					return HAL_FEED_NEED_LOAD_IMAGES;
			}

			if (nanosleep(&sleep_ts, NULL))
				return HAL_FEED_ERROR;

			gettimeofday(&today_tv, NULL);

		} /* while (!eof) */

		return HAL_FEED_CAN_FILE_END; /* candump file end */
	} /* while (infinite_loops || loops--) */

	return HAL_FEED_CAN_FILE_END;
}

void *hal_offline_feed_image_process(void *ptr)
{
	FILE *blank_fp = NULL;
	static struct timeval min_tv, today_start_load_tv, today_end_load_tv;//load_diff_tv, 
	char filelist_path[128];

	hal_offline_feed_can_init();

	sprintf(filelist_path, "%s/%s", offline_image_path, "filelist.txt");
	HAL_DEBUG("[haltest]%s:%d filelist_path is %s\n", __func__, __LINE__, filelist_path);
	offline_filelist_fp = fopen(filelist_path, "r");
	if (!offline_filelist_fp)
	{
		Vps_printf("\r\n### open the filelist.txt failed ### Error !!!\r\n");
		pthread_exit(0);
	}
	blank_fp = fopen("./black_blank_frame.yuv", "r");
	if (!blank_fp)
	{
		Vps_printf("\r\n### open the black_blank_frame.yuv failed ### Error !!!\r\n");
		pthread_exit(0);
	}
	fread(offline_blank_file_buf.buf, 1, OFFLINE_FILE_SIZE, blank_fp);
	fclose(blank_fp);
	
	min_tv.tv_sec = 0;
	min_tv.tv_usec = 0;
	find_min_timestamp(&min_tv);
	gettimeofday(&today_start_load_tv, NULL);
	create_diff_tv(&today_start_load_tv, &diff_tv, &min_tv);
	while (1)
	{
		gettimeofday(&today_start_load_tv, NULL);
		if (OFFLINE_CONTINUE == hal_load_offline_file())
		{
			sleep(1);
			gettimeofday(&today_end_load_tv, NULL);
			//create_diff_tv(&today_after_load_tv, &load_diff_tv, &today_before_load_tv);
			//timeradd(&diff_tv, &load_diff_tv, &diff_tv);
			compensation_diff_tv(&today_start_load_tv, &diff_tv, &today_end_load_tv);
			if (hal_offline_feed_image_can(1) != HAL_FEED_NEED_LOAD_IMAGES)
				break;//The offline can data has been fed!
		}
		else
		{
			//The offline image has been fed!
			sleep(1);
			gettimeofday(&today_end_load_tv, NULL);
			//create_diff_tv(&today_after_load_tv, &load_diff_tv, &today_before_load_tv);
			//timeradd(&diff_tv, &load_diff_tv, &diff_tv);
			compensation_diff_tv(&today_start_load_tv, &diff_tv, &today_end_load_tv);
			hal_offline_feed_image_can(0);
			break;
		}
	}
	fclose(canfile_fp);
	fclose(offline_filelist_fp);
	ringbuffer_destroy(can_ring_buf);
	HAL_DEBUG("offline feed image end!\n");
	pthread_exit(0);
}

void hal_create_feed_thread()
{
	pthread_attr_t attr;
	pthread_t tid;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&tid, &attr, hal_offline_feed_image_process, NULL);
	pthread_attr_destroy(&attr);

}

