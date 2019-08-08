#ifndef __CHAINS_HAL_SINGLE_CAMERA_SAVE_LOCAL_H__
#define __CHAINS_HAL_SINGLE_CAMERA_SAVE_LOCAL_H__


#include <string.h>
#include <sys/time.h>
#include <src/hlos/adas/include/chains.h>
#include <src/hlos/common/chains_common.h>
#include <src/hlos/system/system_gbm_allocator.h>
#include "chains_hal_single_camera_save_local_priv.h"
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

    chains_hal_single_camera_save_localObj ucObj;

    VideoSensorCreateAndStartAppPrm  vidSensorPrm;

    /**< Link Id's and device IDs to use for this use-case */
    UInt32  appCtrlLinkId;

    UInt32  captureOutWidth;
    UInt32  captureOutHeight;

    Chains_Ctrl *chainsCfg;
}Chains_hal_single_camera_save_localAppObj;

extern HAL_FRMDATA_CB g_frame_cb;
extern Chains_hal_single_camera_save_localAppObj g_hal_single_camera_save_local_AppObj;
void chains_hal_single_camera_save_local_StartApp(Chains_hal_single_camera_save_localAppObj *pObj);
void chains_hal_single_camera_save_local_StopApp(Chains_hal_single_camera_save_localAppObj *pObj);

#endif /*__CHAINS_HAL_SINGLE_CAMERA_SAVE_LOCAL_H__*/

