#ifndef __CHAINS_SINGLE_CAMERA_DISPLAY_H__
#define __CHAINS_SINGLE_CAMERA_DISPLAY_H__

#include "chains_hal_single_camera_display_priv.h"
#include <src/hlos/adas/include/chains.h>
#include <src/hlos/common/chains_common.h>
#include <src/hlos/system/system_gbm_allocator.h>
#include <src/include/chains_common_surround_view.h>

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

    chains_hal_single_camera_displayObj ucObj;

    VideoSensorCreateAndStartAppPrm  vidSensorPrm;

    /**< Link Id's and device IDs to use for this use-case */
    UInt32  appCtrlLinkId;

    UInt32  captureOutWidth;
    UInt32  captureOutHeight;

    Chains_Ctrl *chainsCfg;

}chains_hal_single_camera_displayAppObj;

Void chains_hal_single_camera_display(Chains_Ctrl *chainsCfg);
Void chains_hal_single_camera_display_StopApp(chains_hal_single_camera_displayAppObj *pObj);
Void chains_hal_single_camera_display_StartApp(chains_hal_single_camera_displayAppObj *pObj);

#endif /*__CHAINS_SINGLE_CAMERA_DISPLAY_H__*/

