/******************************************************************************
Copyright (c) [2012 - 2017] Texas Instruments Incorporated

All rights reserved not granted herein.

Limited License.

 Texas Instruments Incorporated grants a world-wide, royalty-free, non-exclusive
 license under copyrights and patents it now or hereafter owns or controls to
 make,  have made, use, import, offer to sell and sell ("Utilize") this software
 subject to the terms herein.  With respect to the foregoing patent license,
 such license is granted  solely to the extent that any such patent is necessary
 to Utilize the software alone.  The patent license shall not apply to any
 combinations which include this software, other than combinations with devices
 manufactured by or for TI ("TI Devices").  No hardware patent is licensed
 hereunder.

 Redistributions must preserve existing copyright notices and reproduce this
 license (including the above copyright notice and the disclaimer and
 (if applicable) source code license limitations below) in the documentation
 and/or other materials provided with the distribution

 Redistribution and use in binary form, without modification, are permitted
 provided that the following conditions are met:

 * No reverse engineering, decompilation, or disassembly of this software
   is permitted with respect to any software provided in binary form.

 * Any redistribution and use are licensed by TI for use only with TI Devices.

 * Nothing shall obligate TI to provide you with source code for the software
   licensed and provided to you in object code.

 If software source code is provided to you, modification and redistribution of
 the source code are permitted provided that the following conditions are met:

 * Any redistribution and use of the source code, including any resulting
   derivative works, are licensed by TI for use only with TI Devices.

 * Any redistribution and use of any object code compiled from the source code
   and any resulting derivative works, are licensed by TI for use only with TI
   Devices.

 Neither the name of Texas Instruments Incorporated nor the names of its
 suppliers may be used to endorse or promote products derived from this software
 without specific prior written permission.

 DISCLAIMER.

 THIS SOFTWARE IS PROVIDED BY TI AND TI�S LICENSORS "AS IS" AND ANY EXPRESS OR
 IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL TI AND TI�S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include "chains_radarobjectdetectnull_priv.h"
#include <include/common/chains_radar.h>

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */


/**
 *******************************************************************************
 *
 *  \brief  Chains_radarobjectdetectnullObj
 *
 *        This structure contains all the LinksId's and create Params.
 *        The same is passed to all create, start, stop functions.
 *
 *******************************************************************************
*/
typedef struct {

    chains_radarobjectdetectnullObj ucObj;

    ChainsCommon_Ar12xxConfigOut ar12xxCfg;

    Chains_Ctrl *chainsCfg;

    /* Algorithm Function Create Parameters */
    AlgorithmFxn_RadarFftCreateParams radarFftParams;
    AlgorithmFxn_RadarPkDetectCreateParams radarPkDetectParams;
    AlgorithmFxn_RadarBeamFormCreateParams radarBeamFormParams;
    AlgorithmFxn_RadarDrawCreateParams radarDrawParams;
} Chains_radarobjectdetectnullAppObj;

static char Chains_radarobjectdetectnull_runTimeMenu();


/**
 *******************************************************************************
 *
 * \brief   Start the Links
 *
 *          Function sends a control command to all the links to
 *          to Start them. Links are started in reverce
 *          order as information of next link is required to connect.
 *          System_linkStart is called with LinkId to start the links.
 *
 * \param   pObj  [IN] Chains_radarobjectdetectnullAppObj
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
*/
Void chains_radarobjectdetectnull_StartApp(Chains_radarobjectdetectnullAppObj *pObj)
{
    Chains_memPrintHeapStatus();

    chains_radarobjectdetectnull_Start(&pObj->ucObj);

    ChainsCommon_ar12xxStart();

    Chains_prfLoadCalcEnable(TRUE, FALSE, FALSE);
}

/**
 *******************************************************************************
 *
 * \brief   Delete the usecase Links
 *
 *          Function sends a control command to all usecase links to
 *          to delete them
 *          System_linkDelete is called with LinkId to delete the links.
 *
 * \param   pObj   [IN]   Chains_radarobjectdetectnullAppObj
 *
 *******************************************************************************
*/
Void chains_radarobjectdetectnull_StopAndDeleteApp(Chains_radarobjectdetectnullAppObj *pObj)
{
    chains_radarobjectdetectnull_Stop(&pObj->ucObj);
    chains_radarobjectdetectnull_Delete(&pObj->ucObj);

    ChainsCommon_ar12xxStop(&pObj->ar12xxCfg);
    ChainsCommon_ar12xxBeamFormDeInit(&pObj->ar12xxCfg, &pObj->radarBeamFormParams);
    /* Print the HWI, SWI and all tasks load */
    /* Reset the accumulated timer ticks */
    Chains_prfLoadCalcEnable(FALSE, TRUE, FALSE);
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
Void chains_radarobjectdetectnull_SetAppPrms(chains_radarobjectdetectnullObj *pUcObj, Void *appObj)
{
    Chains_radarobjectdetectnullAppObj *pObj
        = (Chains_radarobjectdetectnullAppObj*)appObj;

    Chains_ar12xxGetSampleConfig(&pObj->ar12xxCfg, 0U);
    ChainsCommon_ar12xxConfig(&pObj->ar12xxCfg);
    ChainsCommon_ar12xxSetIssCaptureParams(&pUcObj->IssCapturePrm, &pObj->ar12xxCfg);

    ChainsCommon_ar12xxSetFFTParams(
        &pObj->radarFftParams,
        &pObj->ar12xxCfg
        );
    ChainsCommon_ar12xxSetPkDetectParams(
        &pObj->radarPkDetectParams,
        &pObj->ar12xxCfg
        );
    ChainsCommon_ar12xxSetBeamFormParams(
        &pObj->radarBeamFormParams,
        &pObj->ar12xxCfg
        );
    ChainsCommon_ar12xxSetRadarDrawParams(
        &pObj->radarDrawParams,
        &pObj->ar12xxCfg,
        1920,
        1080);
    /* Do not ask for selection in DIB/VAB setup */
    if (Bsp_boardGetId() != BSP_BOARD_TDA3XX_AR12XX_RADAR)
    {
        /* Select network mode for null link */
        ChainsCommon_networkTxSetMode(&pUcObj->NetworkTxPrm);
    }

    return;
}

/**
 *******************************************************************************
 *
 * \brief   Single Channel radar sensor (AR1243) capture processing and display
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
Void Chains_radarobjectdetectnull(Chains_Ctrl *chainsCfg)
{
    char ch;
    UInt32 done = FALSE;
    Chains_radarobjectdetectnullAppObj chainsObj;

    chainsObj.chainsCfg = chainsCfg;

    chainsObj.ucObj.Alg_RadarProcess_fftPrm =
        (AlgorithmLink_RadarProcessCreateParams *)&chainsObj.radarFftParams;
    chainsObj.ucObj.Alg_RadarProcess_pkDetectPrm =
        (AlgorithmLink_RadarProcessCreateParams *)&chainsObj.radarPkDetectParams;
    chainsObj.ucObj.Alg_RadarProcess_beamFormPrm =
        (AlgorithmLink_RadarProcessCreateParams *)&chainsObj.radarBeamFormParams;
    chainsObj.ucObj.Alg_RadarProcess_drawObjsPrm =
        (AlgorithmLink_RadarProcessCreateParams *)&chainsObj.radarDrawParams;

    chains_radarobjectdetectnull_Create(&chainsObj.ucObj, &chainsObj);

    chains_radarobjectdetectnull_StartApp(&chainsObj);

    while(!done)
    {

        ch = Chains_radarobjectdetectnull_runTimeMenu();
        switch(ch)
        {
            case '0':
                done = TRUE;
                break;
            case 'c':
            case 'C':
                ChainsCommon_ar12xxCheckParameters(&chainsObj.ar12xxCfg);
                break;
            case 'd':
            case 'D':
                /* For the 0th Profile */
                ChainsCommon_ar12xxChangeParameters(
                    chainsObj.ucObj.Alg_RadarProcess_drawObjsLinkID,
                    &chainsObj.ar12xxCfg);
                ChainsCommon_ar12xxSetDrawProfile(
                    chainsObj.ucObj.Alg_RadarProcess_drawObjsLinkID,
                    &chainsObj.ar12xxCfg,
                    chainsObj.ar12xxCfg.radarParams[0].currProfileId);
                break;
            case 'p':
            case 'P':
                ChainsCommon_PrintStatistics();
                chains_radarobjectdetectnull_printStatistics(&chainsObj.ucObj);
                break;
            default:
                Vps_printf("\nUnsupported option '%c'. Please try again\n", ch);
                break;
        }
    }

    chains_radarobjectdetectnull_StopAndDeleteApp(&chainsObj);
}

/**
 *******************************************************************************
 * \brief Run Time Menu string.
 *******************************************************************************
 */
static char gChains_radarCaptProcWrite_customRunTimeMenu[] = {
    "\r\n "
    "\r\n ===================="
    "\r\n Chains Run-time Menu"
    "\r\n ===================="
    "\r\n "
    "\r\n 0: Stop Chain"
    "\r\n "
    "\r\n c: Read-back and Check AR params"
    "\r\n "
    "\r\n d: Dynamically change params "
    "\r\n "
    "\r\n p: Print Performance Statistics "
    "\r\n "
    "\r\n Enter Choice: "
    "\r\n "
};

static char Chains_radarobjectdetectnull_runTimeMenu()
{
    Vps_printf(gChains_radarCaptProcWrite_customRunTimeMenu);

    return Chains_readChar();
}

