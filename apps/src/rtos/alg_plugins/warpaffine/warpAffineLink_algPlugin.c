/******************************************************************************
*
* Copyright (C) 2019-2020 Momenta Incorporated - https://www.momenta.ai/
*
* ALL RIGHTS RESERVED
*
******************************************************************************/

/**
 *******************************************************************************
 * \file warpAffineLink_algPlugin.c
 *
 * \brief  This file contains plug in functions for Warp Affine
 *         Link
 *
 * \version 0.0 (Feb 2019) : [NN] First version
 *
 *******************************************************************************
*/

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */

#include "warpAffineLink_priv.h"
#include <include/link_api/system_common.h>
#include <src/rtos/utils_common/include/utils_mem.h>

/**
 *******************************************************************************
 *
 * \brief Implementation of function to init plugins()
 *
 *        This function will be called by AlgorithmLink_initAlgPlugins, so as
 *        register plugins of Warp Affine algorithm
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 AlgorithmLink_warpAffine_initPlugin()
{
    AlgorithmLink_FuncTable pluginFunctions;
    UInt32 algId = (UInt32)-1;

    pluginFunctions.AlgorithmLink_AlgPluginCreate =
        AlgorithmLink_warpAffineCreate;
    pluginFunctions.AlgorithmLink_AlgPluginProcess =
        AlgorithmLink_warpAffineProcess;
    pluginFunctions.AlgorithmLink_AlgPluginControl =
        AlgorithmLink_warpAffineControl;
    pluginFunctions.AlgorithmLink_AlgPluginStop =
        AlgorithmLink_warpAffineStop;
    pluginFunctions.AlgorithmLink_AlgPluginDelete =
        AlgorithmLink_warpAffineDelete;

    algId = ALGORITHM_LINK_DSP_ALG_WARP_AFFINE;

    AlgorithmLink_registerPlugin(algId, &pluginFunctions);

    return SYSTEM_LINK_STATUS_SOK;
}

/**
 *******************************************************************************
 *
 * \brief This function allocates memory for the algorithm internal objects
 *
 *
 * \param  numMemRec         [IN] Number of objects
 * \param  memRec            [IN] pointer to the memory records
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
UInt32 AlgorithmLink_warpAffineAlgCreate(AlgorithmLink_WarpAffineChObj *pObj)
{
    /*
     * Intialize the algorithm instance with the allocated memory
     * and user create parameters
     */
    pObj->algWaHandle = AlgIvision_create(
                            &WA_MOMENTA_VISION_FXNS,
                            (IALG_Params*)&pObj->algWaCreateParams
                            );
    UTILS_assert(NULL != pObj->algWaHandle);

    pObj->algWaOutBufSize = WA_ALGLINK_OUT_BUF_SIZE;

#if 0
    pObj->pAlgWaOutBuf =
        Utils_memAlloc(
            UTILS_HEAPID_DDR_CACHED_SR,
            pObj->algWaOutBufSize,
            128
            );
    UTILS_assert(NULL != pObj->pAlgWaOutBuf);

    pObj->algWaOutBufDesc.bufPlanes[0].buf = pObj->pAlgWaOutBuf;
#endif

    return SYSTEM_LINK_STATUS_SOK;
}

/**
 *******************************************************************************
 *
 * \brief This function frees memory for the algorithm internal objects
 *
 *
 * \param  numMemRec         [IN] Number of objects
 * \param  memRec            [IN] pointer to the memory records
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
UInt32 AlgorithmLink_warpAffineAlgDelete(AlgorithmLink_WarpAffineChObj *pObj)
{
    Int32 status;

    status = AlgIvision_delete(pObj->algWaHandle);
    UTILS_assert(SYSTEM_LINK_STATUS_SOK == status);

#if 0
    status = Utils_memFree(
                 UTILS_HEAPID_DDR_CACHED_SR,
                 pObj->pAlgWaOutBuf,
                 pObj->algWaOutBufSize
                 );
    UTILS_assert(SYSTEM_LINK_STATUS_SOK == status);
#endif

    return status;
}

/**
 *******************************************************************************
 *
 * \brief Warp Affine Alg uses the IVISION standard to interact with the
 *        framework. All process/control calls to the algorithm should adhere
 *        to the IVISION standard. This function initializes input and output
 *        buffers
 *
 * \param  pObj              [IN] Algorithm link object handle
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 AlgorithmLink_warpAffineAlgParamsInit(
                                  AlgorithmLink_WarpAffineObj *pLinkObj,
                                  AlgorithmLink_WarpAffineChObj *pObj,
                                  System_LinkChInfo *inChInfo)
{
    pObj->algWaInBufs.size                              = sizeof(pObj->algWaInBufs);
    pObj->algWaInBufs.numBufs                           = 1;
    pObj->algWaInBufs.bufDesc                           = pObj->algWaInBufDescList;
    pObj->algWaInBufDescList[0]                         = &pObj->algWaInBufDesc;

    pObj->algWaOutBufs.size                             = sizeof(pObj->algWaOutBufs);
    pObj->algWaOutBufs.numBufs                          = 1;
    pObj->algWaOutBufs.bufDesc                          = pObj->algWaOutBufDescList;
    pObj->algWaOutBufDescList[0]                        = &pObj->algWaOutBufDesc;

    memset(&pObj->algWaInArgs, 0, sizeof(pObj->algWaInArgs));
    pObj->algWaInArgs.iVisionInArgs.size                = sizeof(pObj->algWaInArgs);
    pObj->algWaInArgs.iVisionInArgs.subFrameInfo        = 0;

    memset(&pObj->algWaOutArgs, 0, sizeof(pObj->algWaOutArgs));
    pObj->algWaOutArgs.iVisionOutArgs.size              = sizeof(pObj->algWaOutArgs);

    memset(&pObj->algWaInBufDesc, 0, sizeof(pObj->algWaInBufDesc));
    pObj->algWaInBufDesc.numPlanes                      = 1;
    /* Will be filled with input pointer later
     */
    pObj->algWaInBufDesc.bufPlanes[0].buf               = NULL;
    pObj->algWaInBufDesc.bufPlanes[0].width             = inChInfo->pitch[0];
    pObj->algWaInBufDesc.bufPlanes[0].height            = inChInfo->height;
    pObj->algWaInBufDesc.formatType                     = 0; /* NOT USED */
    pObj->algWaInBufDesc.bufferId                       = 0xFF; /* NOT USED */

    memset(&pObj->algWaOutBufDesc, 0, sizeof(pObj->algWaOutBufDesc));
    pObj->algWaOutBufDesc.numPlanes                     = 1;
    /* Will be filled with input pointer later
     */
    pObj->algWaOutBufDesc.bufPlanes[0].buf              = NULL;

    pObj->algWaCreateParams.visionParams.algParams.size = sizeof(pObj->algWaCreateParams);
    pObj->algWaCreateParams.visionParams.cacheWriteBack = NULL;

    return SYSTEM_LINK_STATUS_SOK;
}

/**
 *******************************************************************************
 *
 * \brief Implementation of Create Plugin for Warp Affine alg link
 *
 *
 * \param  pObj              [IN] Algorithm link object handle
 * \param  pCreateParams     [IN] Pointer to create time parameters
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 AlgorithmLink_warpAffineCreate(void *pObj, void *pCreateParams)
{
    UInt32                        status;
    UInt32                        prevLinkQId;
    UInt32                        bufId, chId;
    System_LinkInfo               prevLinkInfo;
    AlgorithmLink_WarpAffineObj   *pWAObj;
    AlgorithmLink_OutputQueueInfo *pOutputQInfo;
    AlgorithmLink_InputQueueInfo  *pInputQInfo;
    System_LinkChInfo             *pOutChInfo;
    System_LinkChInfo             *pPrevChInfo;
    System_Buffer                 *pSystemBuffer;
    System_MetaDataBuffer         *pMetaDataBuffer;

    pWAObj = (AlgorithmLink_WarpAffineObj *)
                 Utils_memAlloc(
                     UTILS_HEAPID_DDR_CACHED_LOCAL,
                     sizeof(AlgorithmLink_WarpAffineObj),
                     32
                     );
    UTILS_assert(NULL != pWAObj);

    AlgorithmLink_setAlgorithmParamsObj(pObj, pWAObj);

    pInputQInfo = &pWAObj->inputQInfo;
    pOutputQInfo = &pWAObj->outputQInfo;

    /*
     * Taking copy of needed create time parameters in local object for future
     * reference.
     */
    memcpy(&pWAObj->algLinkCreateParams, pCreateParams, sizeof(pWAObj->algLinkCreateParams));

    pInputQInfo->qMode = ALGORITHM_LINK_QUEUEMODE_NOTINPLACE;
    pOutputQInfo->qMode = ALGORITHM_LINK_QUEUEMODE_NOTINPLACE;

    status = System_linkGetInfo(
                 pWAObj->algLinkCreateParams.inQueParams.prevLinkId,
                 &prevLinkInfo
                 );
    UTILS_assert(SYSTEM_LINK_STATUS_SOK == status);

    UTILS_assert(prevLinkInfo.numQue >= 1);

    prevLinkQId = pWAObj->algLinkCreateParams.inQueParams.prevLinkQueId;

    UTILS_assert(prevLinkInfo.queInfo[prevLinkQId].numCh < WARPAFFINE_LINK_MAX_CH);

    if(pWAObj->algLinkCreateParams.numOutBuffers
        > WARPAFFINE_LINK_MAX_NUM_OUTPUT)
    {
        pWAObj->algLinkCreateParams.numOutBuffers
            = WARPAFFINE_LINK_MAX_NUM_OUTPUT;
    }

    pOutputQInfo->queInfo.numCh = prevLinkInfo.queInfo[prevLinkQId].numCh;

    for(chId=0; chId<pOutputQInfo->queInfo.numCh; chId++)
    {
        pOutChInfo         = &pOutputQInfo->queInfo.chInfo[chId];
        pOutChInfo->flags  = 0;
        pOutChInfo->height = 0;
        pOutChInfo->width  = 0;
    }

    /*
     * Initializations needed for book keeping of buffer handling.
     * Note that this needs to be called only after setting inputQMode and
     * outputQMode.
     */
    AlgorithmLink_queueInfoInit(pObj, 1, pInputQInfo, 1, pOutputQInfo);

    for(chId=0; chId<pOutputQInfo->queInfo.numCh; chId++)
    {
        UTILS_assert(chId < WARPAFFINE_LINK_MAX_CH);
        pPrevChInfo = &prevLinkInfo.queInfo[prevLinkQId].chInfo[chId];

        AlgorithmLink_warpAffineAlgParamsInit(pWAObj, &pWAObj->chObj[chId], pPrevChInfo);

        AlgorithmLink_warpAffineAlgCreate(&pWAObj->chObj[chId]);

        /*
         * Allocate memory for the output buffers and link metadata buffer with
         * system Buffer
         */

        for (bufId = 0; bufId < pWAObj->algLinkCreateParams.numOutBuffers; bufId++)
        {
            pSystemBuffer   = &pWAObj->chObj[chId].buffers[bufId];
            pMetaDataBuffer = &pWAObj->chObj[chId].metaDataBuffers[bufId];

            /*
             * Properties of pSystemBuffer, which do not get altered during
             * run time (frame exchanges) are initialized here
             */
            pSystemBuffer->bufType     = SYSTEM_BUFFER_TYPE_METADATA;
            pSystemBuffer->payload     = pMetaDataBuffer;
            pSystemBuffer->payloadSize = sizeof(System_MetaDataBuffer);
            pSystemBuffer->chNum       = chId;

            pMetaDataBuffer->numMetaDataPlanes = 1;
            pMetaDataBuffer->metaBufSize[0] = pWAObj->chObj[chId].algWaOutBufSize;

            pMetaDataBuffer->bufAddr[0] = Utils_memAlloc(
                                              UTILS_HEAPID_DDR_CACHED_SR,
                                              pMetaDataBuffer->metaBufSize[0],
                                              ALGORITHMLINK_FRAME_ALIGN
                                              );

            pMetaDataBuffer->metaFillLength[0] = pMetaDataBuffer->metaBufSize[0];

            UTILS_assert(NULL != pMetaDataBuffer->bufAddr[0]);

            pMetaDataBuffer->flags = 0;

            status = AlgorithmLink_putEmptyOutputBuffer(pObj, 0, pSystemBuffer);

            UTILS_assert(SYSTEM_LINK_STATUS_SOK == status);
        }
    }

    pWAObj->isFirstFrameRecv = FALSE;

    /* Assign pointer to link stats object */
    pWAObj->linkStatsInfo = Utils_linkStatsCollectorAllocInst(
                                AlgorithmLink_getLinkId(pObj),
                                "ALG_WARP_AFFINE"
                                );
    UTILS_assert(NULL != pWAObj->linkStatsInfo);

    return SYSTEM_LINK_STATUS_SOK;
}

/**
 *******************************************************************************
 *
 * \brief Implementation of Process Plugin for Warp Affine alg link
 *
 *
 * \param  pObj              [IN] Algorithm link object handle
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 AlgorithmLink_warpAffineProcess(void *pObj)
{
    UInt32                               bufId, chId;
    Int32                                status = SYSTEM_LINK_STATUS_SOK;
    Bool                                 bufDropFlag;
    AlgorithmLink_WarpAffineObj          *pWAObj;
    AlgorithmLink_WarpAffineChObj        *pChObj;
    System_BufferList                    inputBufList;
    AlgorithmLink_WarpAffineCreateParams *pLinkCreatePrms;
    System_Buffer                        *pSysOutBuffer;
    System_Buffer                        *pSysInBuffer;
    System_VideoFrameBuffer              *pInVideoBuf;
    System_MetaDataBuffer                *pOutMetaBuf;
    System_BufferList                    bufListReturn;
    System_LinkStatistics                *linkStatsInfo;

    pWAObj = (AlgorithmLink_WarpAffineObj *)
                                AlgorithmLink_getAlgorithmParamsObj(pObj);

    pLinkCreatePrms = &pWAObj->algLinkCreateParams;

    linkStatsInfo = pWAObj->linkStatsInfo;
    UTILS_assert(NULL != linkStatsInfo);

    Utils_linkStatsCollectorProcessCmd(linkStatsInfo);

    if (pWAObj->isFirstFrameRecv == FALSE)
    {
        pWAObj->isFirstFrameRecv = TRUE;

        Utils_resetLinkStatistics(&linkStatsInfo->linkStats, 1, 1);

        Utils_resetLatency(&linkStatsInfo->linkLatency);
        Utils_resetLatency(&linkStatsInfo->srcToLinkLatency);
    }

    linkStatsInfo->linkStats.newDataCmdCount++;

    System_getLinksFullBuffers(
        pLinkCreatePrms->inQueParams.prevLinkId,
        pLinkCreatePrms->inQueParams.prevLinkQueId,
        &inputBufList
        );

    if (inputBufList.numBuf)
    {
        for (bufId = 0; bufId < inputBufList.numBuf; bufId++)
        {
            pSysInBuffer = inputBufList.buffers[bufId];
            if(NULL == pSysInBuffer)
            {
                linkStatsInfo->linkStats.inBufErrorCount++;
                continue;
            }
            if(pSysInBuffer->chNum >= pWAObj->outputQInfo.queInfo.numCh)
            {
                linkStatsInfo->linkStats.inBufErrorCount++;
                continue;
            }

            chId = pSysInBuffer->chNum;

            pChObj = &pWAObj->chObj[chId];

            linkStatsInfo->linkStats.chStats[chId].inBufRecvCount++;

            status = AlgorithmLink_getEmptyOutputBuffer(pObj, 0, chId, &pSysOutBuffer);
            if(status != SYSTEM_LINK_STATUS_SOK)
            {
                linkStatsInfo->linkStats.chStats[chId].inBufDropCount++;
                linkStatsInfo->linkStats.chStats[chId].outBufDropCount[0]++;
            }
            else
            {
                pSysOutBuffer->srcTimestamp = pSysInBuffer->srcTimestamp;
                pSysOutBuffer->frameId = pSysInBuffer->frameId;
                pSysOutBuffer->linkLocalTimestamp = Utils_getCurGlobalTimeInUsec();
                pOutMetaBuf = (System_MetaDataBuffer *)pSysOutBuffer->payload;

                pInVideoBuf = (System_VideoFrameBuffer *)pSysInBuffer->payload;
               
                pChObj->algWaInBufDesc.bufPlanes[0].buf = (Void *)(pInVideoBuf->bufAddr[0]);
                pChObj->pAlgWaOutBuf = pOutMetaBuf->bufAddr[0];
                pChObj->algWaOutBufs.bufDesc[0]->bufPlanes[0].buf = pOutMetaBuf->bufAddr[0];

                Vps_printf("******************WA:DSP start to proc!\n");

                status = AlgIvision_process(
                             pChObj->algWaHandle,
                             &pChObj->algWaInBufs,
                             &pChObj->algWaOutBufs,
                             (IVISION_InArgs*)&pChObj->algWaInArgs,
                             (IVISION_OutArgs *)&pChObj->algWaOutArgs
                             );

                //Vps_printf("******************WA:DSP finish to proc!\n");

                UTILS_assert(IALG_EOK == status);

#if 0
                EDMA_UTILS_memcpy2D(
                    pOutMetaBuf->bufAddr[0],
                    pChObj->pAlgWaOutBuf,
                    WA_ALGLINK_OUT_BUF_WIDTH,
                    WA_ALGLINK_OUT_BUF_HEIGHT * WA_ALGLINK_OUT_BUF_CHANNEL * WA_ALGLINK_OUT_BUF_NUMBER,
                    sizeof(UInt8)*WA_ALGLINK_OUT_BUF_WIDTH,
                    sizeof(UInt8)*WA_ALGLINK_OUT_BUF_WIDTH
                    );

                Cache_wb(
                    (UInt8*)pOutMetaBuf->bufAddr[0],
                    WA_ALGLINK_OUT_BUF_SIZE,
                    Cache_Type_ALLD,
                    TRUE
                    );
#endif

                Cache_wb(
                    (UInt8*)pOutMetaBuf->bufAddr[0],
                    WA_ALGLINK_OUT_BUF_SIZE,
                    Cache_Type_ALLD,
                    TRUE
                    );

                pOutMetaBuf->flags = 0;

                Utils_updateLatency(
                    &linkStatsInfo->linkLatency,
                    pSysOutBuffer->linkLocalTimestamp
                    );
                Utils_updateLatency(
                    &linkStatsInfo->srcToLinkLatency,
                    pSysOutBuffer->srcTimestamp
                    );

                linkStatsInfo->linkStats.chStats[chId].inBufProcessCount++;
                linkStatsInfo->linkStats.chStats[chId].outBufCount[0]++;

                status = AlgorithmLink_putFullOutputBuffer(pObj, 0, pSysOutBuffer);
                UTILS_assert(SYSTEM_LINK_STATUS_SOK == status);

                Vps_printf("******************WA:DSP finish proc, start to send!\n");

                System_sendLinkCmd(
                    pLinkCreatePrms->outQueParams.nextLink,
                    SYSTEM_CMD_NEW_DATA,
                    NULL
                    );

                bufListReturn.numBuf = 1;
                bufListReturn.buffers[0] = pSysOutBuffer;
                status = AlgorithmLink_releaseOutputBuffer(pObj, 0, &bufListReturn);
                UTILS_assert(SYSTEM_LINK_STATUS_SOK == status);
            }

            bufListReturn.numBuf = 1;
            bufListReturn.buffers[0] = pSysInBuffer;
            bufDropFlag = FALSE;
            AlgorithmLink_releaseInputBuffer(
                pObj,
                0,
                pLinkCreatePrms->inQueParams.prevLinkId,
                pLinkCreatePrms->inQueParams.prevLinkQueId,
                &bufListReturn,
                &bufDropFlag
                );
        }
    }
    return status;
}

/**
 *******************************************************************************
 *
 * \brief Implementation of Control Plugin for Warp Affine alg link
 *
 *
 * \param  pObj              [IN] Algorithm link object handle
 * \param  pControlParams    [IN] Pointer to control parameters
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 AlgorithmLink_warpAffineControl(void *pObj, void *pControlParams)
{
    AlgorithmLink_WarpAffineObj *pWAObj;
    AlgorithmLink_ControlParams *pAlgLinkControlPrm;
    Int32                       status = SYSTEM_LINK_STATUS_SOK;

    pWAObj = (AlgorithmLink_WarpAffineObj *)
                        AlgorithmLink_getAlgorithmParamsObj(pObj);

    pAlgLinkControlPrm = (AlgorithmLink_ControlParams *)pControlParams;

    /*
     * There can be other commands to alter the properties of the alg link
     * or properties of the core algorithm.
     * In this simple example, there is just a control command to print
     * statistics and a default call to algorithm control.
     */

    switch(pAlgLinkControlPrm->controlCmd)
    {
        case SYSTEM_CMD_PRINT_STATISTICS:
            AlgorithmLink_warpAffinePrintStatistics(pObj, pWAObj);
            break;

        default:
            break;
    }

    return status;
}

/**
 *******************************************************************************
 *
 * \brief Implementation of Stop Plugin for Warp Affine alg link
 *
 *
 * \param  pObj              [IN] Algorithm link object handle
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 AlgorithmLink_warpAffineStop(void *pObj)
{
    return 0;
}

/**
 *******************************************************************************
 *
 * \brief Implementation of Delete Plugin for Warp Affine alg link
 *
 *
 * \param  pObj              [IN] Algorithm link object handle
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 AlgorithmLink_warpAffineDelete(void *pObj)
{
    Int32                       status = SYSTEM_LINK_STATUS_SOK;
    UInt32                      bufId, chId;
    AlgorithmLink_WarpAffineObj *pWAObj;
    System_MetaDataBuffer       *pMetaDataBuffer;

    pWAObj = (AlgorithmLink_WarpAffineObj *)
                                AlgorithmLink_getAlgorithmParamsObj(pObj);

    status = Utils_linkStatsCollectorDeAllocInst(pWAObj->linkStatsInfo);
    UTILS_assert(SYSTEM_LINK_STATUS_SOK == status);

    for(chId=0; chId<pWAObj->outputQInfo.queInfo.numCh; chId++)
    {
        UTILS_assert(chId < WARPAFFINE_LINK_MAX_CH);
        /*
         * Free allocated memory for alg internal objects
         */
        AlgorithmLink_warpAffineAlgDelete(&pWAObj->chObj[chId]);

        /*
         * Free link buffers
         */
        for (bufId = 0; bufId < pWAObj->algLinkCreateParams.numOutBuffers; bufId++)
        {
            pMetaDataBuffer = &pWAObj->chObj[chId].metaDataBuffers[bufId];

            status = Utils_memFree(
                         UTILS_HEAPID_DDR_CACHED_SR,
                         pMetaDataBuffer->bufAddr[0],
                         pMetaDataBuffer->metaBufSize[0]
                         );
            UTILS_assert(SYSTEM_LINK_STATUS_SOK == status);
        }
    }

    Utils_memFree(
        UTILS_HEAPID_DDR_CACHED_LOCAL,
        pWAObj,
        sizeof(AlgorithmLink_WarpAffineObj)
        );

    return status;
}

Int32 AlgorithmLink_warpAffinePrintStatistics(void *pObj,
                AlgorithmLink_WarpAffineObj *pWAObj)
{
    UTILS_assert(NULL != pWAObj->linkStatsInfo);

    Utils_printLinkStatistics(
        &pWAObj->linkStatsInfo->linkStats,
        "ALG_WARP_AFFINE",
        TRUE
        );

    Utils_printLatency(
        "ALG_WARP_AFFINE",
        &pWAObj->linkStatsInfo->linkLatency,
        &pWAObj->linkStatsInfo->srcToLinkLatency,
        TRUE
        );

    return SYSTEM_LINK_STATUS_SOK;
}

