/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include "chains_hal_multi_camera_save_local.h"
#include "src/hlos/adas/src/usecases/hal_camera/camera_base.h"
#include "src/hlos/adas/src/usecases/hal_imu/hal_imu.h"
#include "src/hlos/adas/src/usecases/hal_imu/hal_imu_priv.h"

#define CAPTURE_SENSOR_WIDTH      (1280)
#define CAPTURE_SENSOR_HEIGHT     (720)
//#define LCD_DISPLAY_WIDTH         (800)
//#define LCD_DISPLAY_HEIGHT        (480)

/**
 *******************************************************************************
 * \brief Channels with timestamp difference <= SYNC_DELTA_IN_MSEC
 *        are synced together by sync link
 *******************************************************************************
 */
#define SYNC_DELTA_IN_MSEC              (16)

/**
 *******************************************************************************
 * \brief Channels with timestamp older than SYNC_DROP_THRESHOLD_IN_MSEC
 *        are dropped by sync link
 *******************************************************************************
 */
#define SYNC_DROP_THRESHOLD_IN_MSEC     (33)

Chains_hal_multi_camera_save_localAppObj g_hal_multi_camera_save_local_AppObj;

Void chains_hal_multi_camera_save_local_set_output_frame_prms(
	Chains_hal_multi_camera_save_localAppObj *appObj,
	CaptureLink_CreateParams *pPrm)
{
	UInt32 i, streamId;
	UInt32 frameSkipMask;
	CaptureLink_VipInstParams *pInstPrm;
	CaptureLink_OutParams *pOutprms;

	switch(g_camera_save_local_obj.outPutFrameRate)
	{
		case 10:
			frameSkipMask = 0x1B6DB6DB;
			break;
		case 15:
			frameSkipMask = 0x2AAAAAAA;
			break;
		case 20:
			frameSkipMask = 0x24924924;
			break;
		default://30fps
			frameSkipMask = 0x00;
			break;
	}

	for (i=0; i<SYSTEM_CAPTURE_VIP_INST_MAX; i++)
	{
		pInstPrm = &pPrm->vipInst[i];

		for (streamId = 0; streamId < CAPTURE_LINK_MAX_OUTPUT_PER_INST; streamId++)
			{
				pOutprms = &pInstPrm->outParams[streamId];
				pOutprms->dataFormat	= g_camera_save_local_obj.outPutColorFormat;
				pOutprms->frameSkipMask =	frameSkipMask;	// 30fps
			}
	}
}


/**
 *******************************************************************************
 *
 * \brief   Set link Parameters
 *
 *          It is called in Create function of the auto generated use-case file.
 *
 * \param pUcObj    [IN] Auto-generated usecase object
 * \param appObj    [IN] Application specific object
 *
 *******************************************************************************
*/
Void chains_hal_multi_camera_save_local_SetAppPrms(chains_hal_multi_camera_save_localObj *pUcObj, Void *appObj)
{
    Chains_hal_multi_camera_save_localAppObj *pObj
            = (Chains_hal_multi_camera_save_localAppObj*)appObj;

    pObj->captureOutWidth  = CAPTURE_SENSOR_WIDTH;
    pObj->captureOutHeight = CAPTURE_SENSOR_HEIGHT;

    pObj->vidSensorPrm.captureSrcId = pObj->chainsCfg->captureSrc;
    pObj->vidSensorPrm.isLVDSCaptMode = TRUE;
    pObj->vidSensorPrm.numLvdsCh = pObj->chainsCfg->numLvdsCh;
	Vps_printf("hubing: CHAINS:chains_hal_multi_camera_save_local_SetAppPrms pObj->vidSensorPrm.numLvdsCh=%d\n",pObj->vidSensorPrm.numLvdsCh);

    System_linkControl(SYSTEM_LINK_ID_APP_CTRL,
                                APP_CTRL_LINK_CMD_VIDEO_SENSOR_CREATE_AND_START,
                                &pObj->vidSensorPrm,
                                sizeof(VideoSensorCreateAndStartAppPrm),
                                TRUE);
	if(pObj->vidSensorPrm.captureSrcId == CHAINS_CAPTURE_SRC_XC7027)
		System_linkControl(SYSTEM_LINK_ID_APP_CTRL,
				APP_CTRL_LINK_CMD_SYNC_TIMER_OUTPUT_START,
				&pObj->vidSensorPrm,
				sizeof(VideoSensorCreateAndStartAppPrm),
				TRUE);
    ChainsCommon_MultiCam_SetCapturePrms(
                    &pUcObj->CapturePrm,
                    pObj->chainsCfg->numLvdsCh);

	chains_hal_multi_camera_save_local_set_output_frame_prms(pObj, &pUcObj->CapturePrm);

//	pUcObj->NullPrm.dumpDataType = NULL_LINK_COPY_TYPE_FILE;

//	strncpy(pUcObj->NullPrm.nameDataFile[0], "/home/stream0.yuv", 260U);
//	strncpy(pUcObj->NullPrm.nameDataFile[1], "/home/stream1.yuv", 260U);
//	strncpy(pUcObj->NullPrm.nameDataFile[2], "/home/stream2.yuv", 260U);
//	strncpy(pUcObj->NullPrm.nameDataFile[3], "/home/stream3.yuv", 260U);
}

/**
 *******************************************************************************
 *
 * \brief   Start the capture display Links
 *
 *          Function sends a control command to capture and display link to
 *          to Start all the required links . Links are started in reverce
 *          order as information of next link is required to connect.
 *          System_linkStart is called with LinkId to start the links.
 *
 * \param   pObj  [IN] Chains_hal_multi_camera_save_localAppObj
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
*/
void chains_hal_multi_camera_save_local_StartApp(Chains_hal_multi_camera_save_localAppObj *pObj)
{
    ChainsCommon_statCollectorReset();
    ChainsCommon_memPrintHeapStatus();
    chains_hal_multi_camera_save_local_Start(&pObj->ucObj);
}

/**
 *******************************************************************************
 *
 * \brief   Delete the capture display Links
 *
 *          Function sends a control command to capture and display link to
 *          to delete all the prior created links
 *          System_linkDelete is called with LinkId to delete the links.
 *
 * \param   pObj   [IN]   Chains_hal_multi_camera_save_localAppObj
 *
 *******************************************************************************
*/
int chains_hal_multi_camera_save_local_StopApp(Chains_hal_multi_camera_save_localAppObj *pObj)
{
	Int32 status = 0;

    status = chains_hal_multi_camera_save_local_Stop(&pObj->ucObj);
	if(status < 0)
	{
		Vps_printf("\n chains_hal_multi_camera_save_local_Stop ERROR !!!\n");
		return status;
	}

    status = chains_hal_multi_camera_save_local_Delete(&pObj->ucObj);
	if(status < 0)
	{
		Vps_printf("\n chains_hal_multi_camera_save_local_Delete ERROR !!!\n");
		return status;
	}

    status = ChainsCommon_StopDisplayCtrl();
	if(status < 0)
	{
		Vps_printf("\n ChainsCommon_StopDisplayCtrl ERROR !!!\n");
		return status;
	}

    status = System_linkControl(SYSTEM_LINK_ID_APP_CTRL,
                        APP_CTRL_LINK_CMD_VIDEO_SENSOR_STOP_AND_DELETE,
                        &pObj->vidSensorPrm,
                        sizeof(VideoSensorCreateAndStartAppPrm),
                        TRUE);
	if(status < 0)
	{
		Vps_printf("\n System_linkControl APP_CTRL_LINK_CMD_VIDEO_SENSOR_STOP_AND_DELETE ERROR !!!\n");
		return status;
	}


	if(pObj->vidSensorPrm.captureSrcId == CHAINS_CAPTURE_SRC_XC7027)
		System_linkControl(SYSTEM_LINK_ID_APP_CTRL,
			APP_CTRL_LINK_CMD_SYNC_TIMER_OUTPUT_STOP,
			&pObj->vidSensorPrm,
			sizeof(VideoSensorCreateAndStartAppPrm),
			TRUE);

     status = ChainsCommon_prfLoadCalcEnable(FALSE, FALSE, FALSE);
	if(status < 0)
	{
		Vps_printf("\n ChainsCommon_prfLoadCalcEnable ERROR !!!\n");
		return status;
	}

	return status;
}

/**
 *******************************************************************************
 *
 * \brief   Single Channel Capture Display usecase function
 *
 *          This functions executes the create, start functions
 *
 *          Further in a while loop displays run time menu and waits
 *          for user inputs to print the statistics or to end the demo.
 *
 *          Once the user inputs end of demo stop and delete
 *          functions are executed.
 *
 * \param   chainsCfg       [IN]   Chains_Ctrl
 *
 *******************************************************************************
*/
Void chains_hal_multi_camera_save_local(Chains_Ctrl *chainsCfg)
{
    char ch;
    UInt32 done = FALSE;
    Chains_hal_multi_camera_save_localAppObj chainsObj;

    chainsObj.chainsCfg = chainsCfg;

    chains_hal_multi_camera_save_local_Create(&chainsObj.ucObj, &chainsObj);

    chains_hal_multi_camera_save_local_StartApp(&chainsObj);

    ChainsCommon_prfLoadCalcEnable(TRUE, FALSE, FALSE);

    while(!done)
    {
        ch = Chains_menuRunTime();

        switch(ch)
        {
            case '0':
                done = TRUE;
                break;
            case 'p':
            case 'P':
                ChainsCommon_PrintStatistics();
                chains_hal_multi_camera_save_local_printStatistics(&chainsObj.ucObj);
                chains_hal_multi_camera_save_local_printBufferStatistics(&chainsObj.ucObj);
                break;
            default:
                Vps_printf("\nUnsupported option '%c'. Please try again\n", ch);
                break;
        }
    }

    chains_hal_multi_camera_save_local_StopApp(&chainsObj);

}

static int hal_multi_camera_get_timestamp(hal_frame_data_t *frmdata, UInt64 m4_timestamp)
{
	struct timeval time;
	UInt64	a15Timestamp = 0;
	UInt64	deltaTimestamp;

	//Vps_printf("\n--Enter-----hal_multi_camera_get_timestamp--------\n");

	a15Timestamp = OSA_getCurGlobalTimeInUsec();
	if(a15Timestamp < m4_timestamp){
		Vps_printf("\n--Error!!!-----a15Timestamp=%lld < m4_timestamp=%lld--------\n",a15Timestamp,m4_timestamp);
		return -1;
	}
	deltaTimestamp = a15Timestamp - m4_timestamp;
	gettimeofday(&time, NULL);

	if(deltaTimestamp < 1000000){
		frmdata->timestamp.tv_sec =  time.tv_sec;
		frmdata->timestamp.tv_usec = time.tv_usec - deltaTimestamp;
	}else{
		frmdata->timestamp.tv_sec =  time.tv_sec - (deltaTimestamp/1000000);
		frmdata->timestamp.tv_usec = time.tv_usec - deltaTimestamp%1000000;
	}
	//Vps_printf("\n--End-----hal_multi_camera_get_timestamp--------\n");
	return 0;
}

Void hal_multi_camera_null_link_callback(System_LinkChInfo *pChInfo, Void *pBuf, Void *arg)
{
	int ret =0;
	UInt64	m4Timestamp;
	hal_frame_data_t frmdata;
	System_Buffer *sys_buf = (System_Buffer *)pBuf;
	System_BitstreamBuffer *bitstreamBuf = ((System_BitstreamBuffer *)sys_buf->payload);

	if(g_frame_cb == NULL)
		return;

	//Vps_printf("--Enter-----hal_multi_camera_null_link_callback--------\n");
	frmdata.len = pChInfo->width * pChInfo->height * g_camera_save_local_obj.outPutColorFormatCopyLen;
	m4Timestamp = sys_buf->srcTimestamp;
	ret = hal_multi_camera_get_timestamp(&frmdata, m4Timestamp);
	if(ret < 0)
	{
		Vps_printf("--Error!!!-----hal_multi_camera_get_timestamp--------\n");
		return ;
	}
	frmdata.channel = sys_buf->chNum;
	frmdata.buffer = (unsigned char *)bitstreamBuf->bufAddr;

	g_frame_cb(&frmdata);
	//Vps_printf("--End-----hal_multi_camera_null_link_callback--------\n");
	return ;
}

static int hal_multi_imu_get_timestamp(hal_imu_data_t *imudata, UInt64 m4_timestamp)
{
	struct timeval time;
	UInt64	a15Timestamp = 0;
	UInt64	deltaTimestamp;

	//Vps_printf("\n--Enter-----hal_multi_camera_get_timestamp--------\n");

	a15Timestamp = OSA_getCurGlobalTimeInUsec();
	if(a15Timestamp < m4_timestamp){
		Vps_printf("\n--Error!!!-----a15Timestamp=%lld < m4_timestamp=%lld--------\n",a15Timestamp,m4_timestamp);
		return -1;
	}
	deltaTimestamp = a15Timestamp - m4_timestamp;
	gettimeofday(&time, NULL);

	if(deltaTimestamp < 1000000){
		imudata->time.tv_sec =  time.tv_sec;
		imudata->time.tv_usec = time.tv_usec - deltaTimestamp;
	}else{
		imudata->time.tv_sec =  time.tv_sec - (deltaTimestamp/1000000);
		imudata->time.tv_usec = time.tv_usec - deltaTimestamp%1000000;
	}
	//Vps_printf("\n--End-----hal_multi_camera_get_timestamp--------\n");
	return 0;
}

Void hal_multi_camera_null_link_imu_callback(System_LinkChInfo *pChInfo, Void *pBuf, Void *arg)
{
	int ret, i, j;
	hal_imu_data_t imudata;
	hal_imu_raw_data_t *raw_data = (hal_imu_raw_data_t *)pBuf;

	//Vps_printf("--Enter-----hal_multi_camera_null_link_imu_callback--------\n");
	
	if(g_imu_cb == NULL)
		return;
	for(i = 0; i < g_imu_num; i++)
	{
		imudata.orgtime = raw_data[i].time;
		ret = hal_multi_imu_get_timestamp(&imudata, raw_data[i].time);
		if(ret < 0)
		{
			Vps_printf("--Error!!!-----hal_multi_camera_get_timestamp--------\n");
			return ;
		}
		imudata.channel = raw_data[i].channel;
		imudata.index = raw_data[i].index;
		imudata.len = IMU_DATA_MAX;
		for(j = 0; j < IMU_DATA_MAX; j++)
			imudata.buffer[j] = raw_data[i].data[j];
		//Vps_printf("callback gyro_acc: 0x%04x, 0x%04x, 0x%04x\n",  
		//		(int16_t)imudata.buffer[1]<<8|imudata.buffer[0], (int16_t)imudata.buffer[3]<<8|imudata.buffer[2], (int16_t)imudata.buffer[5]<<8|imudata.buffer[4]);

		//Vps_printf("--End-----hal_multi_camera_null_link_imu_callback--------\n");
		g_imu_cb(&imudata);
	}
	return ;
}


