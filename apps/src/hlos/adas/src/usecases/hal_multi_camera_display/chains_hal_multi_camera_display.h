#ifndef __CHAINS_HAL_MULTI_CAMERA_DISPLAY_H__
#define __CHAINS_HAL_MULTI_CAMERA_DISPLAY_H__

#include <sys/time.h>
#include <string.h>
#include "chains_hal_multi_camera_display_priv.h"
#include <src/hlos/adas/include/chains.h>
#include <src/hlos/common/chains_common.h>
#include <src/hlos/system/system_gbm_allocator.h>

typedef struct {
    /**< Link Id's and device IDs to use for this use-case */
    chains_hal_multi_camera_displayObj ucObj;
    VideoSensorCreateAndStartAppPrm vidSensorPrm;

    UInt32  appCtrlLinkId;

    UInt32  captureOutWidth;
    UInt32  captureOutHeight;

    Chains_Ctrl *chainsCfg;

} Chains_hal_multi_camera_displayAppObj;

Void chains_hal_multi_camera_display(Chains_Ctrl *chainsCfg);
void chains_hal_multi_camera_display_StopApp(Chains_hal_multi_camera_displayAppObj *pObj);
void chains_hal_multi_camera_display_StartApp(Chains_hal_multi_camera_displayAppObj *pObj);
Void chains_hal_multi_camera_display_SetAppPrms(chains_hal_multi_camera_displayObj *pUcObj, Void *appObj);

#endif /*__CHAINS_HAL_MULTI_CAMERA_DISPLAY_H__*/

