/****************************************************************************
Copyright (c) [2012 - 2017] Texas Instruments Incorporated

All rights reserved not granted herein.

Limited License.

 Texas Instruments Incorporated grants a world-wide, royalty-free, non-exclusive
 license under copyrights and patents it now or hereafter owns or controls to
 make,  have made, use, import, offer to sell and sell ('Utilize') this software
 subject to the terms herein.  With respect to the foregoing patent license,
 such license is granted  solely to the extent that any such patent is necessary
 to Utilize the software alone.  The patent license shall not apply to any
 combinations which include this software, other than combinations with devices
 manufactured by or for TI ('TI Devices').  No hardware patent is licensed
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

 THIS SOFTWARE IS PROVIDED BY TI AND TI�S LICENSORS 'AS IS' AND ANY EXPRESS OR
 IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO DSPNT SHALL TI AND TI�S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 PROFITS; OR BUSINESS INTERRUPTION) HOWDSPR CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, DSPN IF
 ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************************************************************
*/
/*
*******************************************************************************
*
* IMPORTANT NOTE:
*  This file is AUTO-GENERATED by Vision SDK use case generation tool
*
*******************************************************************************
*/
#include "chains_hal_warp_affine_priv.h"
Void chains_hal_warp_affine_SetLinkId(unsigned int procID, chains_hal_warp_affineObj *pObj){
    pObj->NullSrcIPCOut_A15_DSP_LinkID[procID]   = A15_0_LINK (SYSTEM_LINK_ID_NULL_SRC_IPC_OUT_16 + procID);
    pObj->IPCIn_DSP_A15_LinkID[procID]           = SYSTEM_MAKE_LINK_ID ((SYSTEM_PROC_DSP1 + procID / MAX_WA_INSTANCE_COUNT),
                                                                        (SYSTEM_LINK_ID_IPC_IN_3 + procID % MAX_WA_INSTANCE_COUNT));
    pObj->Alg_WarpAffine_LinkID[procID]          = SYSTEM_MAKE_LINK_ID ((SYSTEM_PROC_DSP1 + procID / MAX_WA_INSTANCE_COUNT),
                                                                        (SYSTEM_LINK_ID_ALG_3 + procID % MAX_WA_INSTANCE_COUNT));
    pObj->IPCOut_DSP_A15_LinkID[procID]          = SYSTEM_MAKE_LINK_ID ((SYSTEM_PROC_DSP1 + procID / MAX_WA_INSTANCE_COUNT),
                                                                        (SYSTEM_LINK_ID_IPC_OUT_3 + procID % MAX_WA_INSTANCE_COUNT));
    pObj->IPCInNull_A15_DSP_LinkID[procID]       = A15_0_LINK (SYSTEM_LINK_ID_IPC_IN_NULL_16 + procID);
}

Void chains_hal_warp_affine_ResetLinkPrms(unsigned int procID, chains_hal_warp_affineObj *pObj){
    IpcLink_CreateParams_Init(&pObj->NullSrcIPCOut_A15_DSP_Prm[procID]);
    IpcLink_CreateParams_Init(&pObj->IPCIn_DSP_A15_Prm[procID]);
    AlgorithmLink_WarpAffine_Init(&pObj->Alg_WarpAffine_Prm[procID]);
    IpcLink_CreateParams_Init(&pObj->IPCOut_DSP_A15_Prm[procID]);
    IpcLink_CreateParams_Init(&pObj->IPCInNull_A15_DSP_Prm[procID]);
}

Void chains_hal_warp_affine_SetPrms(unsigned int procID, chains_hal_warp_affineObj *pObj){
    (pObj->Alg_WarpAffine_Prm[procID]).baseClassCreate.size  = sizeof(AlgorithmLink_WarpAffineCreateParams);
    (pObj->Alg_WarpAffine_Prm[procID]).baseClassCreate.algId  = ALGORITHM_LINK_DSP_ALG_WARP_AFFINE;
}

Void chains_hal_warp_affine_ConnectLinks(unsigned int procID, chains_hal_warp_affineObj *pObj){

    //NullSrcIPCOut_A15_DSP
    pObj->NullSrcIPCOut_A15_DSP_Prm[procID].outQueParams.nextLink = pObj->IPCIn_DSP_A15_LinkID[procID];

    //IPCIn_DSP_A15
    pObj->IPCIn_DSP_A15_Prm[procID].inQueParams.prevLinkId = pObj->NullSrcIPCOut_A15_DSP_LinkID[procID];
    pObj->IPCIn_DSP_A15_Prm[procID].inQueParams.prevLinkQueId = 0;
    pObj->IPCIn_DSP_A15_Prm[procID].outQueParams.nextLink = pObj->Alg_WarpAffine_LinkID[procID];

    //Alg_WarpAffine
    pObj->Alg_WarpAffine_Prm[procID].inQueParams.prevLinkId = pObj->IPCIn_DSP_A15_LinkID[procID];
    pObj->Alg_WarpAffine_Prm[procID].inQueParams.prevLinkQueId = 0;
    pObj->Alg_WarpAffine_Prm[procID].outQueParams.nextLink = pObj->IPCOut_DSP_A15_LinkID[procID];

    //IPCOut_DSP_A15
    pObj->IPCOut_DSP_A15_Prm[procID].inQueParams.prevLinkId = pObj->Alg_WarpAffine_LinkID[procID];
    pObj->IPCOut_DSP_A15_Prm[procID].inQueParams.prevLinkQueId = 0;
    pObj->IPCOut_DSP_A15_Prm[procID].outQueParams.nextLink = pObj->IPCInNull_A15_DSP_LinkID[procID];

    //IPCInNull_A15_DSP
    pObj->IPCInNull_A15_DSP_Prm[procID].inQueParams.prevLinkId = pObj->IPCOut_DSP_A15_LinkID[procID];
    pObj->IPCInNull_A15_DSP_Prm[procID].inQueParams.prevLinkQueId = 0;

}

Int32 chains_hal_warp_affine_Create(unsigned int procID, chains_hal_warp_affineObj *pObj, Void *appObj){

    Int32 status;

    chains_hal_warp_affine_SetLinkId(procID, pObj);
    chains_hal_warp_affine_ResetLinkPrms(procID, pObj);

    chains_hal_warp_affine_SetPrms(procID, pObj);
    chains_hal_warp_affine_SetAppPrms(procID, pObj, appObj);

    chains_hal_warp_affine_ConnectLinks(procID, pObj);

    status = System_linkCreate(pObj->NullSrcIPCOut_A15_DSP_LinkID[procID], &pObj->NullSrcIPCOut_A15_DSP_Prm[procID], sizeof(pObj->NullSrcIPCOut_A15_DSP_Prm[procID]));
    UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

    status = System_linkCreate(pObj->IPCIn_DSP_A15_LinkID[procID], &pObj->IPCIn_DSP_A15_Prm[procID], sizeof(pObj->IPCIn_DSP_A15_Prm[procID]));
    UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

    status = System_linkCreate(pObj->Alg_WarpAffine_LinkID[procID], &pObj->Alg_WarpAffine_Prm[procID], sizeof(pObj->Alg_WarpAffine_Prm[procID]));
    UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

    status = System_linkCreate(pObj->IPCOut_DSP_A15_LinkID[procID], &pObj->IPCOut_DSP_A15_Prm[procID], sizeof(pObj->IPCOut_DSP_A15_Prm[procID]));
    UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

    status = System_linkCreate(pObj->IPCInNull_A15_DSP_LinkID[procID], &pObj->IPCInNull_A15_DSP_Prm[procID], sizeof(pObj->IPCInNull_A15_DSP_Prm[procID]));
    UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

    return status;
}

Int32 chains_hal_warp_affine_Start(unsigned int procID, chains_hal_warp_affineObj *pObj){

    Int32 status;

    status = System_linkStart(pObj->IPCInNull_A15_DSP_LinkID[procID]);
    UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

    status = System_linkStart(pObj->IPCOut_DSP_A15_LinkID[procID]);
    UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

    status = System_linkStart(pObj->Alg_WarpAffine_LinkID[procID]);
    UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

    status = System_linkStart(pObj->IPCIn_DSP_A15_LinkID[procID]);
    UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

    status = System_linkStart(pObj->NullSrcIPCOut_A15_DSP_LinkID[procID]);
    UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

    return status;
}

Int32 chains_hal_warp_affine_Stop(unsigned int procID, chains_hal_warp_affineObj *pObj){

    Int32 status;

    status = System_linkStop(pObj->IPCInNull_A15_DSP_LinkID[procID]);
    UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

    status = System_linkStop(pObj->IPCOut_DSP_A15_LinkID[procID]);
    UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

    status = System_linkStop(pObj->Alg_WarpAffine_LinkID[procID]);
    UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

    status = System_linkStop(pObj->IPCIn_DSP_A15_LinkID[procID]);
    UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

    status = System_linkStop(pObj->NullSrcIPCOut_A15_DSP_LinkID[procID]);
    UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

    return status;
}

Int32 chains_hal_warp_affine_Delete(unsigned int procID, chains_hal_warp_affineObj *pObj){

    Int32 status;

    status = System_linkDelete(pObj->IPCInNull_A15_DSP_LinkID[procID]);
    UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

    status = System_linkDelete(pObj->IPCOut_DSP_A15_LinkID[procID]);
    UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

    status = System_linkDelete(pObj->Alg_WarpAffine_LinkID[procID]);
    UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

    status = System_linkDelete(pObj->IPCIn_DSP_A15_LinkID[procID]);
    UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

    status = System_linkDelete(pObj->NullSrcIPCOut_A15_DSP_LinkID[procID]);
    UTILS_assert(status == SYSTEM_LINK_STATUS_SOK);

    return status;
}

Void chains_hal_warp_affine_printBufferStatistics(unsigned int procID, chains_hal_warp_affineObj *pObj){
    System_linkPrintBufferStatistics(pObj->NullSrcIPCOut_A15_DSP_LinkID[procID]);
    Task_sleep(500);
    System_linkPrintBufferStatistics(pObj->IPCIn_DSP_A15_LinkID[procID]);
    System_linkPrintBufferStatistics(pObj->Alg_WarpAffine_LinkID[procID]);
    System_linkPrintBufferStatistics(pObj->IPCOut_DSP_A15_LinkID[procID]);
    Task_sleep(500);
    System_linkPrintBufferStatistics(pObj->IPCInNull_A15_DSP_LinkID[procID]);
    Task_sleep(500);
}

Void chains_hal_warp_affine_printStatistics(unsigned int procID, chains_hal_warp_affineObj *pObj){
    System_linkPrintStatistics(pObj->NullSrcIPCOut_A15_DSP_LinkID[procID]);
    Task_sleep(500);
    System_linkPrintStatistics(pObj->IPCIn_DSP_A15_LinkID[procID]);
    System_linkPrintStatistics(pObj->Alg_WarpAffine_LinkID[procID]);
    System_linkPrintStatistics(pObj->IPCOut_DSP_A15_LinkID[procID]);
    Task_sleep(500);
    System_linkPrintStatistics(pObj->IPCInNull_A15_DSP_LinkID[procID]);
    Task_sleep(500);
}

