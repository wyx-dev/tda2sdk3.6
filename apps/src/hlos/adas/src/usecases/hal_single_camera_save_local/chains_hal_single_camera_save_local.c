
/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */

#include "chains_hal_single_camera_save_local.h"
#include "src/hlos/adas/src/usecases/hal_camera/camera_base.h"

#define CAPTURE_SENSOR_WIDTH      (1280)
#define CAPTURE_SENSOR_HEIGHT     (720)
#define VPE_RESIZE_WIDTH         (640)
#define VPE_RESIZE_HEIGHT        (280)

Chains_hal_single_camera_save_localAppObj g_hal_single_camera_save_local_AppObj;

Void chains_hal_single_camera_save_local_set_output_frame_prms(
	Chains_hal_single_camera_save_localAppObj *appObj,
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
 * \brief   Set VPE Create Parameters
 *
 * \param   pPrm    [OUT]    VpeLink_CreateParams
 *
 *******************************************************************************
*/
static Void chains_hal_single_camera_save_local_SetVpePrm(
                    VpeLink_CreateParams *pPrm,
                    UInt32 numLvdsCh,
                    UInt32 displayWidth,
                    UInt32 displayHeight,
                    UInt32 inputWidth,
                    UInt32 inputHeight
                    )
{
    UInt16 chId;

    pPrm->enableOut[0] = TRUE;

    for(chId = 0; chId < numLvdsCh; chId++)
    {
        pPrm->chParams[chId].outParams[0].numBufsPerCh =
                                 VPE_LINK_NUM_BUFS_PER_CH_DEFAULT;

        pPrm->chParams[chId].outParams[0].width = displayWidth;
        pPrm->chParams[chId].outParams[0].height = displayHeight;
        //pPrm->chParams[chId].outParams[0].dataFormat = SYSTEM_DF_YUV420SP_UV;
		pPrm->chParams[chId].outParams[0].dataFormat = SYSTEM_DF_RGB24_888;

        pPrm->chParams[chId].scCfg.bypass       = FALSE;
        pPrm->chParams[chId].scCfg.nonLinear    = FALSE;
        pPrm->chParams[chId].scCfg.stripSize    = 0;

        pPrm->chParams[chId].scCropCfg.cropStartX = 0;
        pPrm->chParams[chId].scCropCfg.cropStartY = 0;
        pPrm->chParams[chId].scCropCfg.cropWidth = inputWidth;
        pPrm->chParams[chId].scCropCfg.cropHeight = inputHeight;
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
Void chains_hal_single_camera_save_local_SetAppPrms(chains_hal_single_camera_save_localObj *pUcObj,Void *appObj)
{
    Chains_hal_single_camera_save_localAppObj *pObj
            = (Chains_hal_single_camera_save_localAppObj*)appObj;

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
#else
    pObj->vidSensorPrm.captureSrcId = pObj->chainsCfg->captureSrc;
    //pObj->vidSensorPrm.captureSrcId = CHAINS_CAPTURE_SRC_XC7027;
    pObj->vidSensorPrm.isLVDSCaptMode = TRUE;
    pObj->vidSensorPrm.numLvdsCh = 1;

    System_linkControl(SYSTEM_LINK_ID_APP_CTRL,
                                APP_CTRL_LINK_CMD_VIDEO_SENSOR_CREATE_AND_START,
                                &pObj->vidSensorPrm,
                                sizeof(VideoSensorCreateAndStartAppPrm),
                                TRUE);

	ChainsCommon_MultiCam_SetCapturePrms(
				&pUcObj->CapturePrm,
				pObj->vidSensorPrm.numLvdsCh);

	chains_hal_single_camera_save_local_set_output_frame_prms(pObj, &pUcObj->CapturePrm);
	chains_hal_single_camera_save_local_SetVpePrm(&pUcObj->VPEPrm,
											1,
											VPE_RESIZE_WIDTH,
											VPE_RESIZE_HEIGHT,
											CAPTURE_SENSOR_WIDTH,
											CAPTURE_SENSOR_HEIGHT);
#endif
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
 * \param   pObj  [IN] chains_hal_single_camera_save_localAppObj
 *
 *
 *******************************************************************************
*/
void chains_hal_single_camera_save_local_StartApp(Chains_hal_single_camera_save_localAppObj *pObj)
{
    ChainsCommon_statCollectorReset();
    ChainsCommon_memPrintHeapStatus();

    chains_hal_single_camera_save_local_Start(&pObj->ucObj);
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
 * \param   pObj  [IN] chains_hal_single_camera_save_localAppObj
 *
 *
 *******************************************************************************
*/
void chains_hal_single_camera_save_local_StopApp(Chains_hal_single_camera_save_localAppObj *pObj)
{

    chains_hal_single_camera_save_local_Stop(&pObj->ucObj);

    chains_hal_single_camera_save_local_Delete(&pObj->ucObj);

    ChainsCommon_StopDisplayCtrl();

    System_linkControl(SYSTEM_LINK_ID_APP_CTRL,
                        APP_CTRL_LINK_CMD_VIDEO_SENSOR_STOP_AND_DELETE,
                        &pObj->vidSensorPrm,
                        sizeof(VideoSensorCreateAndStartAppPrm),
                        TRUE);
    ChainsCommon_prfLoadCalcEnable(FALSE, FALSE, FALSE);

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
Void chains_hal_single_camera_save_local(Chains_Ctrl *chainsCfg)
{
    char ch;
    UInt32 done = FALSE;
    Chains_hal_single_camera_save_localAppObj chainsObj;

    chainsObj.chainsCfg = chainsCfg;

    chains_hal_single_camera_save_local_Create(&chainsObj.ucObj, &chainsObj);

    chains_hal_single_camera_save_local_StartApp(&chainsObj);

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
                chains_hal_single_camera_save_local_printStatistics(&chainsObj.ucObj);
                chains_hal_single_camera_save_local_printBufferStatistics(&chainsObj.ucObj);
                break;
            default:
                Vps_printf("\nUnsupported option '%c'. Please try again\n", ch);
                break;
        }
    }

    chains_hal_single_camera_save_local_StopApp(&chainsObj);
}

static int hal_single_camera_get_timestamp(hal_frame_data_t *frmdata, UInt64 m4_timestamp)
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

Void hal_single_camera_null_link_callback(System_LinkChInfo *pChInfo, Void *pBuf, Void *arg)
{
	int ret =0;
	UInt64	m4Timestamp;
	hal_frame_data_t frmdata;
	System_Buffer *sys_buf = (System_Buffer *)pBuf;
	System_BitstreamBuffer *bitstreamBuf = ((System_BitstreamBuffer *)sys_buf->payload);

	if(g_frame_cb == NULL)
		return;

	//Vps_printf("--Enter-----hal_single_camera_null_link_callback---width=%d---height=%d--\n",pChInfo->width,pChInfo->height);
	//frmdata.len = pChInfo->width * pChInfo->height * g_camera_save_local_obj.outPutColorFormatCopyLen;
    frmdata.len = pChInfo->width * pChInfo->height * 3;
	m4Timestamp = sys_buf->srcTimestamp;
	ret = hal_single_camera_get_timestamp(&frmdata, m4Timestamp);
	if(ret < 0)
	{
		Vps_printf("--Error!!!-----hal_single_camera_get_timestamp--------\n");
		return;
	}
	frmdata.channel = sys_buf->chNum;
	frmdata.buffer = (unsigned char *)bitstreamBuf->bufAddr;

	g_frame_cb(&frmdata);
	//Vps_printf("--End-----hal_single_camera_null_link_callback--------\n");
	return ;
}

