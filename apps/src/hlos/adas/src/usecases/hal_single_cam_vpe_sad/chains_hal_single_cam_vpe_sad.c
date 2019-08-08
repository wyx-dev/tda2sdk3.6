
/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */

#include "chains_hal_single_cam_vpe_sad.h"
#include "src/hlos/adas/src/usecases/hal_camera/camera_base.h"
#include <src/hlos/adas/include/chains.h>
#include <src/hlos/common/chains_common.h>

// #define CAPTURE_SENSOR_WIDTH     (1280)
// #define CAPTURE_SENSOR_HEIGHT    (720)

// #define LCD_DISPLAY_WIDTH         (1280)
// #define LCD_DISPLAY_HEIGHT        (720)

#define CAPTURE_SENSOR_WIDTH     (1920)
#define CAPTURE_SENSOR_HEIGHT    (1080)
#define LCD_DISPLAY_WIDTH        (1920)
#define LCD_DISPLAY_HEIGHT       (1080)

Chains_hal_single_cam_vpe_sadAppObj g_hal_single_cam_vpe_sad_AppObj;

static Void chains_hal_single_cam_vpe_sad_SetSyncPrm(chains_hal_single_cam_vpe_sadObj *pUcObj)
{
    pUcObj->Sync_vpePrm.syncDelta = (UInt32)(g_camera_save_local_obj.outPutFrameRate/2); //SYNC_VPE_DELTA_IN_MSEC
    pUcObj->Sync_vpePrm.syncThreshold = (UInt32)g_camera_save_local_obj.outPutFrameRate;//SYNC_VPE_DROP_THRESHOLD_IN_MSEC
}


Void chains_hal_single_cam_vpe_sad_set_output_frame_prms(CaptureLink_CreateParams *pPrm)
{
	UInt32 i = 0, streamId = 0;
	CaptureLink_VipInstParams *pInstPrm;
	CaptureLink_OutParams *pOutprms;

	for (i=0; i<SYSTEM_CAPTURE_VIP_INST_MAX; i++)
	{
		pInstPrm = &pPrm->vipInst[i];

		for (streamId = 0; streamId < CAPTURE_LINK_MAX_OUTPUT_PER_INST; streamId++)
        {
            pOutprms = &pInstPrm->outParams[streamId];
            pOutprms->dataFormat	= g_camera_save_local_obj.outPutColorFormat;
        }
	}
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
static Void chains_hal_single_cam_vpe_sad_SetVpePrm(
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
Void chains_hal_single_cam_vpe_sad_SetAppPrms(chains_hal_single_cam_vpe_sadObj *pUcObj,Void *appObj)
{
    Chains_hal_single_cam_vpe_sadAppObj *pObj
            = (Chains_hal_single_cam_vpe_sadAppObj*)appObj;

    pObj->captureOutWidth  = CAPTURE_SENSOR_WIDTH;
    pObj->captureOutHeight = CAPTURE_SENSOR_HEIGHT;
#if 0
    ChainsCommon_SingleCam_SetCapturePrms(&(pUcObj->CapturePrm),
            CAPTURE_SENSOR_WIDTH,
            CAPTURE_SENSOR_HEIGHT,
            pObj->captureOutWidth,
            pObj->captureOutHeight,
            pObj->chainsCfg->captureSrc
            );
#endif
    pObj->vidSensorPrm.captureSrcId = CHAINS_CAPTURE_SRC_HDMI_1080P;//CHAINS_CAPTURE_SRC_HDMI_1080P;//pObj->chainsCfg->captureSrc;
    pObj->vidSensorPrm.isLVDSCaptMode = FALSE;
    pObj->vidSensorPrm.numLvdsCh = 1;

    ChainsCommon_GetDisplayWidthHeight(
                                pObj->chainsCfg->displayType,
                                &pObj->displayOutWidth,
                                &pObj->displayOutHeight
                                );
#if 0
    System_linkControl(SYSTEM_LINK_ID_APP_CTRL,
                                APP_CTRL_LINK_CMD_VIDEO_SENSOR_CREATE_AND_START,
                                &pObj->vidSensorPrm,
                                sizeof(VideoSensorCreateAndStartAppPrm),
                                TRUE);

    if(pObj->vidSensorPrm.captureSrcId == CHAINS_CAPTURE_SRC_XC7027)
    {
		System_linkControl(SYSTEM_LINK_ID_APP_CTRL,
                                APP_CTRL_LINK_CMD_SYNC_TIMER_OUTPUT_START,
                                &pObj->vidSensorPrm,
                                sizeof(VideoSensorCreateAndStartAppPrm),
                                TRUE);
    }
#endif
#if 1
    System_linkControl(SYSTEM_LINK_ID_IPU1_0,
                                0xabcd0002,
                                NULL,
                                0,
                                TRUE);
#endif

	ChainsCommon_MultiCam_SetCapturePrms(&pUcObj->CapturePrm,
                                pObj->vidSensorPrm.numLvdsCh);

	chains_hal_single_cam_vpe_sad_set_output_frame_prms(&pUcObj->CapturePrm);
    /*VPE0 resize nothing, and color format to BGR*/
    chains_hal_single_cam_vpe_sad_SetVpePrm(&pUcObj->VPE_resize_0Prm,
                                1,
                                g_camera_save_local_obj.resize_config[1].output_width,
                                g_camera_save_local_obj.resize_config[1].output_height,
                                g_camera_save_local_obj.resize_config[1].start_x,
                                g_camera_save_local_obj.resize_config[1].start_y,
                                g_camera_save_local_obj.resize_config[1].end_x,
                                g_camera_save_local_obj.resize_config[1].end_y,
                                g_camera_save_local_obj.resize_config[1].colorformat);

    /*VPE1 resize to 224*512 and color format to YUV*/
    chains_hal_single_cam_vpe_sad_SetVpePrm(&pUcObj->VPE_resize_1Prm,
                                1,
                                g_camera_save_local_obj.resize_config[2].output_width,
                                g_camera_save_local_obj.resize_config[2].output_height,
                                g_camera_save_local_obj.resize_config[2].start_x,
                                g_camera_save_local_obj.resize_config[2].start_y,
                                g_camera_save_local_obj.resize_config[2].end_x,
                                g_camera_save_local_obj.resize_config[2].end_y,
                                g_camera_save_local_obj.resize_config[2].colorformat);

    /*VPE2 resize to 224*384 and color format to BGR*/
    chains_hal_single_cam_vpe_sad_SetVpePrm(&pUcObj->VPE_resize_2Prm,
                                1,
                                g_camera_save_local_obj.resize_config[3].output_width,
                                g_camera_save_local_obj.resize_config[3].output_height,
                                g_camera_save_local_obj.resize_config[3].start_x,
                                g_camera_save_local_obj.resize_config[3].start_y,
                                g_camera_save_local_obj.resize_config[3].end_x,
                                g_camera_save_local_obj.resize_config[3].end_y,
                                g_camera_save_local_obj.resize_config[3].colorformat);

    /*VPE3 resize to 224*384 and color format to BGR*/
    chains_hal_single_cam_vpe_sad_SetVpePrm(&pUcObj->VPE_resize_3Prm,
                                1,
                                g_camera_save_local_obj.resize_config[4].output_width,
                                g_camera_save_local_obj.resize_config[4].output_height,
                                g_camera_save_local_obj.resize_config[4].start_x,
                                g_camera_save_local_obj.resize_config[4].start_y,
                                g_camera_save_local_obj.resize_config[4].end_x,
                                g_camera_save_local_obj.resize_config[4].end_y,
                                g_camera_save_local_obj.resize_config[4].colorformat);

    chains_hal_single_cam_vpe_sad_SetSyncPrm(pUcObj);

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
 * \param   pObj  [IN] chains_hal_single_cam_vpe_sadAppObj
 *
 *
 *******************************************************************************
*/
void chains_hal_single_cam_vpe_sad_StartApp(Chains_hal_single_cam_vpe_sadAppObj *pObj)
{
    ChainsCommon_statCollectorReset();
    ChainsCommon_memPrintHeapStatus();

    chains_hal_single_cam_vpe_sad_Start(&pObj->ucObj);
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
 * \param   pObj  [IN] chains_hal_single_cam_vpe_sadAppObj
 *
 *
 *******************************************************************************
*/
int chains_hal_single_cam_vpe_sad_StopApp(Chains_hal_single_cam_vpe_sadAppObj *pObj)
{

    chains_hal_single_cam_vpe_sad_Stop(&pObj->ucObj);

    chains_hal_single_cam_vpe_sad_Delete(&pObj->ucObj);

    ChainsCommon_StopDisplayCtrl();
#if 0
    System_linkControl(SYSTEM_LINK_ID_APP_CTRL,
                        APP_CTRL_LINK_CMD_VIDEO_SENSOR_STOP_AND_DELETE,
                        &pObj->vidSensorPrm,
                        sizeof(VideoSensorCreateAndStartAppPrm),
                        TRUE);
#endif
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
Void chains_hal_single_cam_vpe_sad(Chains_Ctrl *chainsCfg)
{
    char ch;
    UInt32 done = FALSE;
    Chains_hal_single_cam_vpe_sadAppObj chainsObj;
    chainsCfg->displayType = CHAINS_DISPLAY_TYPE_HDMI_1080P;
    chainsCfg->captureSrc = CHAINS_CAPTURE_SRC_HDMI_1080P;
    chainsObj.chainsCfg = chainsCfg;

    chains_hal_single_cam_vpe_sad_Create(&chainsObj.ucObj, &chainsObj);

    chains_hal_single_cam_vpe_sad_StartApp(&chainsObj);

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
                chains_hal_single_cam_vpe_sad_printStatistics(&chainsObj.ucObj);
                chains_hal_single_cam_vpe_sad_printBufferStatistics(&chainsObj.ucObj);
                break;
            default:
                Vps_printf("\nUnsupported option '%c'. Please try again\n", ch);
                break;
        }
    }
    chains_hal_single_cam_vpe_sad_StopApp(&chainsObj);
}

static int hal_single_cam_vpe_sad_get_timestamp(hal_frame_data_t *frmdata, UInt64 m4_timestamp)
{
	struct timeval time;
	UInt64	a15Timestamp = 0;
	UInt64	deltaTimestamp;

	//Vps_printf("\n--Enter-----hal_single_camera_get_timestamp--------\n");
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
	//Vps_printf("\n--End-----hal_single_camera_get_timestamp--------\n");
	return 0;
}

Void hal_single_cam_vpe_sad_null_link_callback(System_LinkChInfo *pChInfo, Void *pBuf, Void *arg)
{
	int ret =0;
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
	ret = hal_single_cam_vpe_sad_get_timestamp(&frmdata, m4Timestamp);
	if(ret < 0)
	{
		Vps_printf("--Error!!!-----hal_single_cam_vpe_sad_null_link_callback--------\n");
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

	//Vps_printf("--End-----hal_single_camera_null_link_callback--------\n");
	return ;
}

