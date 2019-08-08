
/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */

#include "chains_hal_offline_cam_vpe_sad.h"
#include "src/hlos/adas/src/usecases/hal_camera/camera_base.h"
#include <src/hlos/adas/include/chains.h>
#include <src/hlos/common/chains_common.h>

// #define CAPTURE_SENSOR_WIDTH        (1280)
// #define CAPTURE_SENSOR_HEIGHT       (720)
#define CAPTURE_SENSOR_WIDTH        (1920)
#define CAPTURE_SENSOR_HEIGHT       (1080)
// #define LCD_DISPLAY_WIDTH           (1280)
// #define LCD_DISPLAY_HEIGHT          (720)
#define LCD_DISPLAY_WIDTH           (1920)
#define LCD_DISPLAY_HEIGHT          (1080)
#define NULLSRC_OUT_WIDTH           (1280)
#define NULLSRC_OUT_HEIGHT          (720)
#define TILOGO_WIDTH                (1280)
#define TILOGO_HEIGHT               (720)

Chains_hal_offline_cam_vpe_sadAppObj g_hal_offline_cam_vpe_sad_AppObj;

static Void chains_hal_offline_cam_vpe_sad_SetSyncPrm(chains_hal_offline_cam_vpe_sadObj *pUcObj)
{
    pUcObj->Sync_vpePrm.syncDelta = (UInt32)(g_camera_save_local_obj.outPutFrameRate/2); //SYNC_VPE_DELTA_IN_MSEC
    pUcObj->Sync_vpePrm.syncThreshold = g_camera_save_local_obj.outPutFrameRate;//SYNC_VPE_DROP_THRESHOLD_IN_MSEC
}


Void chains_hal_offline_cam_vpe_sad_SetNullSrcPrms(Chains_hal_offline_cam_vpe_sadAppObj *pObj,
                        NullSrcLink_CreateParams *pPrm)
{
    UInt32 chId;
    System_LinkChInfo *pChInfo;

    NullSrcLink_CreateParams_Init(pPrm);

    pPrm->outQueInfo.numCh = 1;
    // pPrm->timerPeriodMilliSecs = 33; //33ms = 30fps
    pPrm->timerPeriodMilliSecs = 100;   //100ms = 10fps
    pPrm->timerPeriodMilliSecs = g_camera_save_local_obj.outPutFrameRate;

    for (chId = 0; chId < pPrm->outQueInfo.numCh; chId++)
    {
        pPrm->channelParams[chId].numBuffers = 4;

        pChInfo = &pPrm->outQueInfo.chInfo[chId];

        pChInfo->flags = System_Link_Ch_Info_Set_Flag_Data_Format(pChInfo->flags,
                                                    SYSTEM_DF_YUV420SP_UV);
        pChInfo->flags = System_Link_Ch_Info_Set_Flag_Scan_Format(pChInfo->flags,
                                                    SYSTEM_SF_PROGRESSIVE);
        pChInfo->flags = System_Link_Ch_Info_Set_Flag_Buf_Type(pChInfo->flags,
                                               SYSTEM_BUFFER_TYPE_VIDEO_FRAME
                                               /*SYSTEM_BUFFER_TYPE_BITSTREAM*/);
        pChInfo->width = pObj->captureOutWidth;
        pChInfo->height = pObj->captureOutHeight;
        pChInfo->startX = 0;
        pChInfo->startY = 0;
        pChInfo->pitch[0] = SystemUtils_align(pChInfo->width, 32);
        pChInfo->pitch[1] = SystemUtils_align(pChInfo->width, 32);

        //pPrm->channelParams[chId].fileReadMode = NULLSRC_LINK_FILEREAD_RUN_TIME;
        // strncpy(pPrm->channelParams[chId].nameDataFile, "/home/1920x1080_00.h264", 260);
        // strncpy(pPrm->channelParams[chId].nameIndexFile, "/home/1920x1080_00.hdr", 260);
    }

    pPrm->dataRxMode = NULLSRC_LINK_DATA_RX_MODE_FILE;
}


/**
 *******************************************************************************
 *
 * \brief   Set VPE Create Parameters
 *
 * \param   pPrm    [OUT]    VpeLink_CreateParams
 *
 *******************************************************************************
*/
static Void chains_hal_offline_cam_vpe_sad_SetVpePrm(
                    VpeLink_CreateParams *pPrm,
                    UInt32 numLvdsCh,
                    UInt32 outputWidth,
                    UInt32 outputHeight,
                    UInt32 CropStartX,
                    UInt32 CropStartY,
                    UInt32 CropEndX,
                    UInt32 CropEndY,
                    System_VideoDataFormat outputColorFormat
                    )
{
    UInt16 chId;

    pPrm->enableOut[0] = TRUE;

    for(chId = 0; chId < numLvdsCh; chId++)
    {
        pPrm->chParams[chId].outParams[0].numBufsPerCh =
                                 VPE_LINK_NUM_BUFS_PER_CH_DEFAULT;
        pPrm->chParams[chId].outParams[0].width = outputWidth;
        pPrm->chParams[chId].outParams[0].height = outputHeight;
		pPrm->chParams[chId].outParams[0].dataFormat = outputColorFormat;

        pPrm->chParams[chId].scCfg.bypass       = FALSE;
        pPrm->chParams[chId].scCfg.nonLinear    = FALSE;
        pPrm->chParams[chId].scCfg.stripSize    = 0;

        pPrm->chParams[chId].scCropCfg.cropStartX = CropStartX;
        pPrm->chParams[chId].scCropCfg.cropStartY = CropStartY;
        pPrm->chParams[chId].scCropCfg.cropWidth = CropEndX - CropStartX;
        pPrm->chParams[chId].scCropCfg.cropHeight = CropEndY - CropStartY;
    }
}


void hal_fillSrcBuf(chains_hal_offline_cam_vpe_sadObj *pObj)
{
    Int32 status;
    UInt32 ht, bufCnt, logoStartX, logoStartY, offset;
    UInt8 *srcAddr, *dstAddr;
    NullSrcLink_GetBufInfoParams getBufInfoPrm;
    System_Buffer *pBuffer;
    System_VideoFrameBuffer *pVideoFrame;
    char *hal_Logo = g_camera_save_local_obj.offlineDataFileName;
    // char *hal_Logo = (char *)malloc(NULLSRC_OUT_WIDTH*NULLSRC_OUT_HEIGHT*2);
    // FILE* fp = fopen("/home/hubing/yuv_data/1547435277.213412.raw", "r");
    // // FILE* fp = fopen(g_camera_save_local_obj.offline_file_name, "r");
    // if(fp)
    // {
    //     fread(hal_Logo, 1, NULLSRC_OUT_WIDTH*NULLSRC_OUT_HEIGHT*2, fp);
    // }
    // fflush(fp);
    // fclose(fp);

    /**
     *  Get the Null source buffer info
     */
    getBufInfoPrm.chId = 0U;
    status = System_linkControl(
                    pObj->NullSourceLinkID,
                    NULL_SRC_LINK_CMD_GET_BUF_INFO,
                    &getBufInfoPrm,
                    sizeof(NullSrcLink_GetBufInfoParams),
                    TRUE);
    UTILS_assert (SYSTEM_LINK_STATUS_SOK == status);

    for (bufCnt = 0;bufCnt < getBufInfoPrm.numBuf;bufCnt ++)
    {
        pBuffer = &getBufInfoPrm.buffer[bufCnt];
        pVideoFrame = pBuffer->payload;

        memset(
            pVideoFrame->bufAddr[0],
            0xFF,
            (NULLSRC_OUT_WIDTH * NULLSRC_OUT_HEIGHT));

        memset(
            pVideoFrame->bufAddr[1],
            0x80,
            (NULLSRC_OUT_WIDTH * NULLSRC_OUT_HEIGHT/2));

        /* copy TI logo */
        logoStartX = (NULLSRC_OUT_WIDTH - TILOGO_WIDTH)/2;
        logoStartY = (NULLSRC_OUT_HEIGHT - TILOGO_HEIGHT)/2;
        offset = (logoStartY * NULLSRC_OUT_WIDTH) + logoStartX;
        dstAddr = (UInt8*)((UInt32)pVideoFrame->bufAddr[0] + offset);
        srcAddr = (UInt8*)hal_Logo;
        for(ht = 0;ht < TILOGO_HEIGHT;ht++)
        {
            memcpy(dstAddr, srcAddr, TILOGO_WIDTH);
            dstAddr += NULLSRC_OUT_WIDTH;
            srcAddr += TILOGO_WIDTH;
        }

        offset = ((logoStartY/2) * NULLSRC_OUT_WIDTH) + logoStartX;
        dstAddr = (UInt8*)((UInt32)pVideoFrame->bufAddr[1] + offset);
        srcAddr = (UInt8*)((UInt32)hal_Logo + \
                                            (TILOGO_WIDTH * TILOGO_HEIGHT));
        for(ht = 0;ht < TILOGO_HEIGHT/2;ht++)
        {
            memcpy(dstAddr, srcAddr, TILOGO_WIDTH);
            dstAddr += NULLSRC_OUT_WIDTH;
            srcAddr += TILOGO_WIDTH;
        }

        OSA_memCacheWb(
                (UInt32)pVideoFrame->bufAddr[0],
                ((UInt32)pVideoFrame->bufAddr[0] + \
                            (NULLSRC_OUT_WIDTH * NULLSRC_OUT_HEIGHT)));
        OSA_memCacheWb(
                (UInt32)pVideoFrame->bufAddr[1],
                ((UInt32)pVideoFrame->bufAddr[1] + \
                            (NULLSRC_OUT_WIDTH * (NULLSRC_OUT_HEIGHT/2))));
    }
}

/**
 *******************************************************************************
 *
 * \brief   chains_hal_offline_cam_vpe_sad_nullsrc_restart
 *
 *          Function sends a control command to NullSrc Link restart.
 *
 * \param appObj    [IN] Application specific object
 *
 *******************************************************************************
*/
void chains_hal_offline_cam_vpe_sad_nullsrc_restart(Chains_hal_offline_cam_vpe_sadAppObj *pObj)
{
    hal_fillSrcBuf(&pObj->ucObj);
    /* Resume Null source */
    System_linkControl(
            pObj->ucObj.NullSourceLinkID,
            NULL_SRC_LINK_CMD_RESUME,
            NULL,
            0U,
            TRUE);
}


/**
 *******************************************************************************
 *
 * \brief   chains_hal_offline_cam_vpe_sad_nullsrc_pause
 *
 *          Function sends a control command to NullSrc Link pause.
 *
 * \param appObj    [IN] Application specific objectd
 *
 *******************************************************************************
*/
void chains_hal_offline_cam_vpe_sad_nullsrc_pause(Chains_hal_offline_cam_vpe_sadAppObj *pObj)
{
    /* Puase Null source */
    System_linkControl(
            pObj->ucObj.NullSourceLinkID,
            NULL_SRC_LINK_CMD_PAUSE,
            NULL,
            0U,
            TRUE);
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
Void chains_hal_offline_cam_vpe_sad_SetAppPrms(chains_hal_offline_cam_vpe_sadObj *pUcObj,Void *appObj)
{
    Chains_hal_offline_cam_vpe_sadAppObj *pObj
            = (Chains_hal_offline_cam_vpe_sadAppObj*)appObj;

    pObj->captureOutWidth  = CAPTURE_SENSOR_WIDTH;
    pObj->captureOutHeight = CAPTURE_SENSOR_HEIGHT;
    pObj->vidSensorPrm.captureSrcId = pObj->chainsCfg->captureSrc;
    pObj->vidSensorPrm.isLVDSCaptMode = TRUE;
    pObj->vidSensorPrm.numLvdsCh = 1;

    ChainsCommon_GetDisplayWidthHeight(
                                pObj->chainsCfg->displayType,
                                &pObj->displayOutWidth,
                                &pObj->displayOutHeight
                                );

    chains_hal_offline_cam_vpe_sad_SetNullSrcPrms(pObj, &pUcObj->NullSourcePrm);

    // System_linkControl(SYSTEM_LINK_ID_APP_CTRL,
    //                             APP_CTRL_LINK_CMD_VIDEO_SENSOR_CREATE_AND_START,
    //                             &pObj->vidSensorPrm,
    //                             sizeof(VideoSensorCreateAndStartAppPrm),
    //                             TRUE);

    // if(pObj->vidSensorPrm.captureSrcId == CHAINS_CAPTURE_SRC_XC7027)
    // {
	// 	System_linkControl(SYSTEM_LINK_ID_APP_CTRL,
    //                             APP_CTRL_LINK_CMD_SYNC_TIMER_OUTPUT_START,
    //                             &pObj->vidSensorPrm,
    //                             sizeof(VideoSensorCreateAndStartAppPrm),
    //                             TRUE);
    // }

	// ChainsCommon_MultiCam_SetCapturePrms(&pUcObj->CapturePrm,
    //                             pObj->vidSensorPrm.numLvdsCh);

    /*VPE0 resize nothing, and color format to BGR*/
    chains_hal_offline_cam_vpe_sad_SetVpePrm(&pUcObj->VPE_resize_0Prm,
                                1,
                                g_camera_save_local_obj.resize_config[1].output_width,
                                g_camera_save_local_obj.resize_config[1].output_height,
                                g_camera_save_local_obj.resize_config[1].start_x,
                                g_camera_save_local_obj.resize_config[1].start_y,
                                g_camera_save_local_obj.resize_config[1].end_x,
                                g_camera_save_local_obj.resize_config[1].end_y,
                                g_camera_save_local_obj.resize_config[1].colorformat);

    /*VPE1 resize to 224*512 and color format to YUV*/
    chains_hal_offline_cam_vpe_sad_SetVpePrm(&pUcObj->VPE_resize_1Prm,
                                1,
                                g_camera_save_local_obj.resize_config[2].output_width,
                                g_camera_save_local_obj.resize_config[2].output_height,
                                g_camera_save_local_obj.resize_config[2].start_x,
                                g_camera_save_local_obj.resize_config[2].start_y,
                                g_camera_save_local_obj.resize_config[2].end_x,
                                g_camera_save_local_obj.resize_config[2].end_y,
                                g_camera_save_local_obj.resize_config[2].colorformat);

    /*VPE2 resize to 224*384 and color format to BGR*/
    chains_hal_offline_cam_vpe_sad_SetVpePrm(&pUcObj->VPE_resize_2Prm,
                                1,
                                g_camera_save_local_obj.resize_config[3].output_width,
                                g_camera_save_local_obj.resize_config[3].output_height,
                                g_camera_save_local_obj.resize_config[3].start_x,
                                g_camera_save_local_obj.resize_config[3].start_y,
                                g_camera_save_local_obj.resize_config[3].end_x,
                                g_camera_save_local_obj.resize_config[3].end_y,
                                g_camera_save_local_obj.resize_config[3].colorformat);

    /*VPE3 resize to 224*384 and color format to BGR*/
    chains_hal_offline_cam_vpe_sad_SetVpePrm(&pUcObj->VPE_resize_3Prm,
                                1,
                                g_camera_save_local_obj.resize_config[4].output_width,
                                g_camera_save_local_obj.resize_config[4].output_height,
                                g_camera_save_local_obj.resize_config[4].start_x,
                                g_camera_save_local_obj.resize_config[4].start_y,
                                g_camera_save_local_obj.resize_config[4].end_x,
                                g_camera_save_local_obj.resize_config[4].end_y,
                                g_camera_save_local_obj.resize_config[4].colorformat);

    chains_hal_offline_cam_vpe_sad_SetSyncPrm(pUcObj);

    ChainsCommon_SetDisplayPrms(&pUcObj->DisplayPrm,
                                NULL,
                                pObj->chainsCfg->displayType,
                                pObj->displayOutWidth,
                                pObj->displayOutHeight
                                );

    ChainsCommon_StartDisplayCtrl(
                                pObj->chainsCfg->displayType,
                                pObj->displayOutWidth,
                                pObj->displayOutHeight
                                );
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
 * \param   pObj  [IN] chains_hal_offline_cam_vpe_sadAppObj
 *
 *
 *******************************************************************************
*/
void chains_hal_offline_cam_vpe_sad_StartApp(Chains_hal_offline_cam_vpe_sadAppObj *pObj)
{
    //hal_fillSrcBuf(&pObj->ucObj);
    ChainsCommon_statCollectorReset();
    ChainsCommon_memPrintHeapStatus();

    chains_hal_offline_cam_vpe_sad_Start(&pObj->ucObj);
}

/**
 *******************************************************************************
 *
 * \brief   Stop the capture display Links
 *
 *          Function sends a control command to capture and display link to
 *          to Start all the required links . Links are started in reverce
 *          order as information of next link is required to connect.
 *          System_linkStart is called with LinkId to start the links.
 *
 * \param   pObj  [IN] chains_hal_offline_cam_vpe_sadAppObj
 *
 *
 *******************************************************************************
*/
int chains_hal_offline_cam_vpe_sad_StopApp(Chains_hal_offline_cam_vpe_sadAppObj *pObj)
{
    chains_hal_offline_cam_vpe_sad_Stop(&pObj->ucObj);

    chains_hal_offline_cam_vpe_sad_Delete(&pObj->ucObj);

    ChainsCommon_StopDisplayCtrl();

    ChainsCommon_prfLoadCalcEnable(FALSE, FALSE, FALSE);

    return 0;
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
Void chains_hal_offline_cam_vpe_sad(Chains_Ctrl *chainsCfg)
{
    char ch;
    UInt32 done = FALSE;
    Chains_hal_offline_cam_vpe_sadAppObj chainsObj;

    chainsObj.chainsCfg = chainsCfg;

    chains_hal_offline_cam_vpe_sad_Create(&chainsObj.ucObj, &chainsObj);

    chains_hal_offline_cam_vpe_sad_StartApp(&chainsObj);

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
                chains_hal_offline_cam_vpe_sad_printStatistics(&chainsObj.ucObj);
                chains_hal_offline_cam_vpe_sad_printBufferStatistics(&chainsObj.ucObj);
                break;
            default:
                Vps_printf("\nUnsupported option '%c'. Please try again\n", ch);
                break;
        }
    }
    chains_hal_offline_cam_vpe_sad_StopApp(&chainsObj);
}

static int hal_offline_cam_vpe_sad_get_timestamp(hal_frame_data_t *frmdata, UInt64 m4_timestamp)
{
	struct timeval time;
	UInt64	a15Timestamp = 0;
	UInt64	deltaTimestamp;

	//Vps_printf("\n--Enter-----hal_offline_camera_get_timestamp--------\n");
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
	//Vps_printf("\n--End-----hal_offline_camera_get_timestamp--------\n");
	return 0;
}

Void hal_offline_cam_vpe_sad_null_link_callback(System_LinkChInfo *pChInfo, Void *pBuf, Void *arg)
{
	int ret = 0;
	UInt64	m4Timestamp;
	hal_frame_data_t frmdata;
	System_Buffer *sys_buf = (System_Buffer *)pBuf;
    System_BitstreamBuffer *bitstreamBuf = ((System_BitstreamBuffer *)sys_buf->payload);
    System_VideoFrameCompositeBuffer *pSysCompBuf = ((System_VideoFrameCompositeBuffer *)sys_buf->payload);

    if(g_frame_cb == NULL)
		return;

	m4Timestamp = sys_buf->srcTimestamp;
    frmdata.channel = sys_buf->chNum;

    /*timestamp change*/
	ret = hal_offline_cam_vpe_sad_get_timestamp(&frmdata, m4Timestamp);
	if(ret < 0)
	{
		Vps_printf("--Error!!!-----hal_offline_cam_vpe_sad_null_link_callback--------\n");
		return;
	}

    /*Init frmdata.buffer*/
    frmdata.buffer = (unsigned char *)bitstreamBuf->bufAddr;
    frmdata.len = pChInfo->width * pChInfo->height * g_camera_save_local_obj.outPutColorFormatCopyLen;

    for(ret = 0; ret < pSysCompBuf->numFrames; ret++)
    {
        frmdata.resize_buf_len[ret] =
            g_camera_save_local_obj.resize_config[ret].output_width *
            g_camera_save_local_obj.resize_config[ret].output_height *
            g_camera_save_local_obj.outVpeResizeColorCopyLen[ret];

        frmdata.resize_buf[ret] = pSysCompBuf->bufAddr[0][ret];
    }

    /*call callback func*/
    g_frame_cb(&frmdata);

	//Vps_printf("--End-----hal_offline_camera_null_link_callback--------\n");
	return ;
}

