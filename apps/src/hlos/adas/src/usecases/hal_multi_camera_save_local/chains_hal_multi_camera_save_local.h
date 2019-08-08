#ifndef __CHAINS_HAL_MULTI_CAMERA_SAVE_LOCAL_H__
#define __CHAINS_HAL_MULTI_CAMERA_SAVE_LOCAL_H__

#include <string.h>
#include <sys/time.h>
#include "chains_hal_multi_camera_save_local_priv.h"
#include <src/hlos/adas/include/chains.h>
#include <src/hlos/common/chains_common.h>
#include <src/hlos/system/system_gbm_allocator.h>
#include "src/hlos/adas/src/usecases/hal_camera/hal_camera.h"
#include "src/hlos/adas/src/usecases/hal_imu/hal_imu.h"

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
    /**< Link Id's and device IDs to use for this use-case */
    chains_hal_multi_camera_save_localObj ucObj;
    VideoSensorCreateAndStartAppPrm vidSensorPrm;
    UInt32  appCtrlLinkId;
    UInt32  captureOutWidth;
    UInt32  captureOutHeight;
    Chains_Ctrl *chainsCfg;
} Chains_hal_multi_camera_save_localAppObj;

extern HAL_FRMDATA_CB g_frame_cb;
extern HAL_IMUDATA_CB g_imu_cb;

Chains_hal_multi_camera_save_localAppObj g_hal_multi_camera_save_local_AppObj;
void chains_hal_multi_camera_save_local_StartApp(Chains_hal_multi_camera_save_localAppObj *pObj);
int chains_hal_multi_camera_save_local_StopApp(Chains_hal_multi_camera_save_localAppObj *pObj);

#endif /*__CHAINS_HAL_MULTI_CAMERA_SAVE_LOCAL_H__*/
