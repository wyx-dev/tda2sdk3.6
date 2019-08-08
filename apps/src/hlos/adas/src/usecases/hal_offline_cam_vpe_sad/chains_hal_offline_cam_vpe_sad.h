#ifndef __CHAINS_HAL_OFFLINE_CAM_VPE_SAD_H__
#define __CHAINS_HAL_OFFLINE_CAM_VPE_SAD_H__

#include <string.h>
#include <sys/time.h>
#include <src/hlos/adas/include/chains.h>
#include <src/hlos/common/chains_common.h>
#include <src/hlos/system/system_gbm_allocator.h>
#include "chains_hal_offline_cam_vpe_sad_priv.h"
#include "src/hlos/adas/src/usecases/hal_camera/hal_camera.h"

/**
 *******************************************************************************
 *
 *  \brief  SingleCameraViewObject
 *
 *        This structure contains all the LinksId's and create Params.
 *        The same is passed to all create, start, stop functions.
 *
 *******************************************************************************
*/
typedef struct {

    chains_hal_offline_cam_vpe_sadObj ucObj;

    VideoSensorCreateAndStartAppPrm  vidSensorPrm;

    /**< Link Id's and device IDs to use for this use-case */
    UInt32  appCtrlLinkId;

    UInt32  captureOutWidth;
    UInt32  captureOutHeight;
    UInt32  displayOutWidth;
    UInt32  displayOutHeight;

    Chains_Ctrl *chainsCfg;
}Chains_hal_offline_cam_vpe_sadAppObj;

extern HAL_FRMDATA_CB g_frame_cb;
extern Chains_hal_offline_cam_vpe_sadAppObj g_hal_offline_cam_vpe_sad_AppObj;
void chains_hal_offline_cam_vpe_sad_StartApp(Chains_hal_offline_cam_vpe_sadAppObj *pObj);
int chains_hal_offline_cam_vpe_sad_StopApp(Chains_hal_offline_cam_vpe_sadAppObj *pObj);
void chains_hal_offline_cam_vpe_sad_nullsrc_pause(Chains_hal_offline_cam_vpe_sadAppObj *pObj);
void chains_hal_offline_cam_vpe_sad_nullsrc_restart(Chains_hal_offline_cam_vpe_sadAppObj *pObj);
#endif /*__CHAINS_HAL_OFFLINE_CAM_VPE_SAD_H__*/

