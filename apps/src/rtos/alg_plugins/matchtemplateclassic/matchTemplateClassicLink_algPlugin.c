/******************************************************************************
*
* Copyright (C) 2019-2020 Momenta Incorporated - https://www.momenta.ai/
*
* ALL RIGHTS RESERVED
*
******************************************************************************/

/**
 *******************************************************************************
 * \file matchTemplateClassicLink_algPlugin.c
 *
 * \brief  This file contains plug in functions for Match Template Classic
 *         Link
 *
 * \version 0.0 (Apr 2019) : [NN] First version
 *
 *******************************************************************************
*/

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */

#include "matchTemplateClassicLink_priv.h"
#include <include/link_api/system_common.h>
#include <src/rtos/utils_common/include/utils_mem.h>

/**
 *******************************************************************************
 *
 * \brief Implementation of function to init plugins()
 *
 *        This function will be called by AlgorithmLink_initAlgPlugins, so as
 *        register plugins of Match Template Classic algorithm
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 AlgorithmLink_matchTemplateClassic_initPlugin()
{
    AlgorithmLink_FuncTable pluginFunctions;
    UInt32 algId = (UInt32)-1;

    pluginFunctions.AlgorithmLink_AlgPluginCreate =
        AlgorithmLink_matchTemplateClassicCreate;
    pluginFunctions.AlgorithmLink_AlgPluginProcess =
        AlgorithmLink_matchTemplateClassicProcess;
    pluginFunctions.AlgorithmLink_AlgPluginControl =
        AlgorithmLink_matchTemplateClassicControl;
    pluginFunctions.AlgorithmLink_AlgPluginStop =
        AlgorithmLink_matchTemplateClassicStop;
    pluginFunctions.AlgorithmLink_AlgPluginDelete =
        AlgorithmLink_matchTemplateClassicDelete;

    algId = ALGORITHM_LINK_DSP_ALG_MATCH_TEMPLATE_CLASSIC;

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
UInt32 AlgorithmLink_matchTemplateClassicAlgCreate(AlgorithmLink_MatchTemplateClassicChObj *pObj)
{
    /*
     * Intialize the algorithm instance with the allocated memory
     * and user create parameters
     */
    pObj->algMtcHandle = AlgIvision_create(
                            &MTC_MOMENTA_VISION_FXNS,
                            (IALG_Params*)&pObj->algMtcCreateParams
                            );
    UTILS_assert(NULL != pObj->algMtcHandle);

    pObj->algMtcOutBufSize = MTC_ALGLINK_OUT_BUF_SIZE;

#if 0
    pObj->pAlgMtcOutBuf =
        Utils_memAlloc(
            UTILS_HEAPID_DDR_CACHED_SR,
            pObj->algMtcOutBufSize,
            128
            );
    UTILS_assert(NULL != pObj->pAlgMtcOutBuf);
#endif

    Vps_printf("MT classic algo create!\n");
    pObj->algMtcOutBufDesc.bufPlanes[0].buf = pObj->pAlgMtcOutBuf;

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
UInt32 AlgorithmLink_matchTemplateClassicAlgDelete(AlgorithmLink_MatchTemplateClassicChObj *pObj)
{
    Int32 status;

    status = AlgIvision_delete(pObj->algMtcHandle);
    UTILS_assert(SYSTEM_LINK_STATUS_SOK == status);

#if 0
    status = Utils_memFree(
                 UTILS_HEAPID_DDR_CACHED_SR,
                 pObj->pAlgMtcOutBuf,
                 pObj->algMtcOutBufSize
                 );
    UTILS_assert(SYSTEM_LINK_STATUS_SOK == status);
#endif

    return status;
}

/**
 *******************************************************************************
 *
 * \brief Match Template Classic Alg uses the IVISION standard to interact with the
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
Int32 AlgorithmLink_matchTemplateClassicAlgParamsInit(
                                  AlgorithmLink_MatchTemplateClassicObj *pLinkObj,
                                  AlgorithmLink_MatchTemplateClassicChObj *pObj,
                                  System_LinkChInfo *inChInfo)
{
    pObj->algMtcInBufs.size                              = sizeof(pObj->algMtcInBufs);
    pObj->algMtcInBufs.numBufs                           = 1;
    pObj->algMtcInBufs.bufDesc                           = pObj->algMtcInBufDescList;
    pObj->algMtcInBufDescList[0]                         = &pObj->algMtcInBufDesc;

    pObj->algMtcOutBufs.size                             = sizeof(pObj->algMtcOutBufs);
    pObj->algMtcOutBufs.numBufs                          = 1;
    pObj->algMtcOutBufs.bufDesc                          = pObj->algMtcOutBufDescList;
    pObj->algMtcOutBufDescList[0]                        = &pObj->algMtcOutBufDesc;

    memset(&pObj->algMtcInArgs, 0, sizeof(pObj->algMtcInArgs));
    pObj->algMtcInArgs.iVisionInArgs.size                = sizeof(pObj->algMtcInArgs);
    pObj->algMtcInArgs.iVisionInArgs.subFrameInfo        = 0;

    memset(&pObj->algMtcOutArgs, 0, sizeof(pObj->algMtcOutArgs));
    pObj->algMtcOutArgs.iVisionOutArgs.size              = sizeof(pObj->algMtcOutArgs);

    memset(&pObj->algMtcInBufDesc, 0, sizeof(pObj->algMtcInBufDesc));
    pObj->algMtcInBufDesc.numPlanes                      = 1;
    /* Will be filled with input pointer later
     */
    pObj->algMtcInBufDesc.bufPlanes[0].buf               = NULL;
    pObj->algMtcInBufDesc.bufPlanes[0].width             = inChInfo->pitch[0];
    pObj->algMtcInBufDesc.bufPlanes[0].height            = inChInfo->height;
    pObj->algMtcInBufDesc.bufPlanes[0].frameROI.width    = inChInfo->pitch[0];
    pObj->algMtcInBufDesc.bufPlanes[0].frameROI.height   = inChInfo->height;
    pObj->algMtcInBufDesc.formatType                     = 0; /* NOT USED */
    pObj->algMtcInBufDesc.bufferId                       = 0xFF; /* NOT USED */

    memset(&pObj->algMtcOutBufDesc, 0, sizeof(pObj->algMtcOutBufDesc));
    pObj->algMtcOutBufDesc.numPlanes                     = 1;
    /* Will be filled with input pointer later
     */
    pObj->algMtcOutBufDesc.bufPlanes[0].buf              = NULL;

    pObj->algMtcCreateParams.visionParams.algParams.size = sizeof(pObj->algMtcCreateParams);
    pObj->algMtcCreateParams.visionParams.cacheWriteBack = NULL;

    return SYSTEM_LINK_STATUS_SOK;
}

/**
 *******************************************************************************
 *
 * \brief Implementation of Create Plugin for Match Template Classic alg link
 *
 *
 * \param  pObj              [IN] Algorithm link object handle
 * \param  pCreateParams     [IN] Pointer to create time parameters
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 AlgorithmLink_matchTemplateClassicCreate(void *pObj, void *pCreateParams)
{
    UInt32                         status;
    UInt32                         prevLinkQId;
    UInt32                         bufId, chId;
    System_LinkInfo                prevLinkInfo;
    AlgorithmLink_MatchTemplateClassicObj *pMTCObj;
    AlgorithmLink_OutputQueueInfo  *pOutputQInfo;
    AlgorithmLink_InputQueueInfo   *pInputQInfo;
    System_LinkChInfo              *pOutChInfo;
    System_LinkChInfo              *pPrevChInfo;
    System_Buffer                  *pSystemBuffer;
    System_MetaDataBuffer          *pMetaDataBuffer;

    pMTCObj = (AlgorithmLink_MatchTemplateClassicObj *)
                 Utils_memAlloc(
                     UTILS_HEAPID_DDR_CACHED_LOCAL,
                     sizeof(AlgorithmLink_MatchTemplateClassicObj),
                     32
                     );
    UTILS_assert(NULL != pMTCObj);

    AlgorithmLink_setAlgorithmParamsObj(pObj, pMTCObj);

    pInputQInfo = &pMTCObj->inputQInfo;
    pOutputQInfo = &pMTCObj->outputQInfo;

    /*
     * Taking copy of needed create time parameters in local object for future
     * reference.
     */
    memcpy(&pMTCObj->algLinkCreateParams, pCreateParams, sizeof(pMTCObj->algLinkCreateParams));

    pInputQInfo->qMode = ALGORITHM_LINK_QUEUEMODE_NOTINPLACE;
    pOutputQInfo->qMode = ALGORITHM_LINK_QUEUEMODE_NOTINPLACE;

    status = System_linkGetInfo(
                 pMTCObj->algLinkCreateParams.inQueParams.prevLinkId,
                 &prevLinkInfo
                 );
    UTILS_assert(SYSTEM_LINK_STATUS_SOK == status);

    UTILS_assert(prevLinkInfo.numQue >= 1);

    prevLinkQId = pMTCObj->algLinkCreateParams.inQueParams.prevLinkQueId;

    UTILS_assert(prevLinkInfo.queInfo[prevLinkQId].numCh < MATCHTEMPLATECLASSIC_LINK_MAX_CH);

    if(pMTCObj->algLinkCreateParams.numOutBuffers
        > MATCHTEMPLATECLASSIC_LINK_MAX_NUM_OUTPUT)
    {
        pMTCObj->algLinkCreateParams.numOutBuffers
            = MATCHTEMPLATECLASSIC_LINK_MAX_NUM_OUTPUT;
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
        UTILS_assert(chId < MATCHTEMPLATECLASSIC_LINK_MAX_CH);
        pPrevChInfo = &prevLinkInfo.queInfo[prevLinkQId].chInfo[chId];

        AlgorithmLink_matchTemplateClassicAlgParamsInit(pMTCObj, &pMTCObj->chObj[chId], pPrevChInfo);

        AlgorithmLink_matchTemplateClassicAlgCreate(&pMTCObj->chObj[chId]);

        /*
         * Allocate memory for the output buffers and link metadata buffer with
         * system Buffer
         */

        for (bufId = 0; bufId < pMTCObj->algLinkCreateParams.numOutBuffers; bufId++)
        {
            pSystemBuffer   = &pMTCObj->chObj[chId].buffers[bufId];
            pMetaDataBuffer = &pMTCObj->chObj[chId].metaDataBuffers[bufId];

            /*
             * Properties of pSystemBuffer, which do not get altered during
             * run time (frame exchanges) are initialized here
             */
            pSystemBuffer->bufType     = SYSTEM_BUFFER_TYPE_METADATA;
            pSystemBuffer->payload     = pMetaDataBuffer;
            pSystemBuffer->payloadSize = sizeof(System_MetaDataBuffer);
            pSystemBuffer->chNum       = chId;

            pMetaDataBuffer->numMetaDataPlanes = 1;
            pMetaDataBuffer->metaBufSize[0] = pMTCObj->chObj[chId].algMtcOutBufSize;

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

    pMTCObj->isFirstFrameRecv = FALSE;

    /* Assign pointer to link stats object */
    pMTCObj->linkStatsInfo = Utils_linkStatsCollectorAllocInst(
                                AlgorithmLink_getLinkId(pObj),
                                "ALG_MATCH_TEMPLATE_CLASSIC"
                                );
    UTILS_assert(NULL != pMTCObj->linkStatsInfo);

    return SYSTEM_LINK_STATUS_SOK;
}

/**
 *******************************************************************************
 *
 * \brief Implementation of Process Plugin for Match Template Classic alg link
 *
 *
 * \param  pObj              [IN] Algorithm link object handle
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 AlgorithmLink_matchTemplateClassicProcess(void *pObj)
{
    UInt32                                  bufId, chId;
    Int32                                   status = SYSTEM_LINK_STATUS_SOK;
    Bool                                    bufDropFlag;
    AlgorithmLink_MatchTemplateClassicObj          *pMTCObj;
    AlgorithmLink_MatchTemplateClassicChObj        *pChObj;
    System_BufferList                       inputBufList;
    AlgorithmLink_MatchTemplateClassicCreateParams *pLinkCreatePrms;
    System_Buffer                           *pSysOutBuffer;
    System_Buffer                           *pSysInBuffer;
    System_VideoFrameBuffer                 *pInVideoBuf;
    System_MetaDataBuffer                   *pOutMetaBuf;
    System_BufferList                       bufListReturn;
    System_LinkStatistics                   *linkStatsInfo;

    pMTCObj = (AlgorithmLink_MatchTemplateClassicObj *)
                                AlgorithmLink_getAlgorithmParamsObj(pObj);

    pLinkCreatePrms = &pMTCObj->algLinkCreateParams;

    linkStatsInfo = pMTCObj->linkStatsInfo;
    UTILS_assert(NULL != linkStatsInfo);

    Utils_linkStatsCollectorProcessCmd(linkStatsInfo);

    if (pMTCObj->isFirstFrameRecv == FALSE)
    {
        pMTCObj->isFirstFrameRecv = TRUE;

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
            if(pSysInBuffer->chNum >= pMTCObj->outputQInfo.queInfo.numCh)
            {
                linkStatsInfo->linkStats.inBufErrorCount++;
                continue;
            }

            chId = pSysInBuffer->chNum;

            pChObj = &pMTCObj->chObj[chId];

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
               
                pChObj->algMtcInBufDesc.bufPlanes[0].buf = (Void *)(pInVideoBuf->bufAddr[0]);

                pChObj->pAlgMtcOutBuf = pOutMetaBuf->bufAddr[0];
                pChObj->algMtcOutBufs.bufDesc[0]->bufPlanes[0].buf = pOutMetaBuf->bufAddr[0];

                //Vps_printf("buf is:%x,%x,%d!\n",pChObj->pAlgMtcOutBuf,pOutMetaBuf->bufAddr[0],pChObj->algMtcOutBufSize);

                status = AlgIvision_process(
                             pChObj->algMtcHandle,
                             &pChObj->algMtcInBufs,
                             &pChObj->algMtcOutBufs,
                             (IVISION_InArgs*)&pChObj->algMtcInArgs,
                             (IVISION_OutArgs *)&pChObj->algMtcOutArgs
                             );
                UTILS_assert(IALG_EOK == status);

#if 0
                Cache_wb(
                    (UInt8*)pChObj->pAlgMtcOutBuf,
                    MTC_ALGLINK_OUT_BUF_SIZE,
                    Cache_Type_ALLD,
                    TRUE
                    );

                EDMA_UTILS_memcpy2D(
                    pOutMetaBuf->bufAddr[0],
                    pChObj->pAlgMtcOutBuf,
                    MTC_ALGLINK_OUT_SCORE_PITCH,
                    MTC_ALGLINK_OUT_SCORE_HEIGHT * sizeof(float),
                    MTC_ALGLINK_OUT_SCORE_PITCH,
                    MTC_ALGLINK_OUT_SCORE_PITCH
                    );
#endif

                Cache_wb(
                    (UInt8*)pOutMetaBuf->bufAddr[0],
                    MTC_ALGLINK_OUT_BUF_SIZE,
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

                System_sendLinkCmd(
                    pLinkCreatePrms->outQueParams.nextLink,
                    SYSTEM_CMD_NEW_DATA,
                    NULL
                    );

                Vps_printf("***************MTC:DSP finish proc,send notify done!\n");

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
 * \brief Implementation of Control Plugin for Match Template Classic alg link
 *
 *
 * \param  pObj              [IN] Algorithm link object handle
 * \param  pControlParams    [IN] Pointer to control parameters
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 AlgorithmLink_matchTemplateClassicControl(void *pObj, void *pControlParams)
{
    AlgorithmLink_MatchTemplateClassicObj *pMTCObj;
    AlgorithmLink_ControlParams    *pAlgLinkControlPrm;
    Int32                          status = SYSTEM_LINK_STATUS_SOK;

    pMTCObj = (AlgorithmLink_MatchTemplateClassicObj *)
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
            AlgorithmLink_matchTemplateClassicPrintStatistics(pObj, pMTCObj);
            break;

        default:
            break;
    }

    return status;
}

/**
 *******************************************************************************
 *
 * \brief Implementation of Stop Plugin for Match Template Classic alg link
 *
 *
 * \param  pObj              [IN] Algorithm link object handle
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 AlgorithmLink_matchTemplateClassicStop(void *pObj)
{
    return 0;
}

/**
 *******************************************************************************
 *
 * \brief Implementation of Delete Plugin for Match Template Classic alg link
 *
 *
 * \param  pObj              [IN] Algorithm link object handle
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 AlgorithmLink_matchTemplateClassicDelete(void *pObj)
{
    Int32                          status = SYSTEM_LINK_STATUS_SOK;
    UInt32                         bufId, chId;
    AlgorithmLink_MatchTemplateClassicObj *pMTCObj;
    System_MetaDataBuffer          *pMetaDataBuffer;

    pMTCObj = (AlgorithmLink_MatchTemplateClassicObj *)
                                AlgorithmLink_getAlgorithmParamsObj(pObj);

    status = Utils_linkStatsCollectorDeAllocInst(pMTCObj->linkStatsInfo);
    UTILS_assert(SYSTEM_LINK_STATUS_SOK == status);

    for(chId=0; chId<pMTCObj->outputQInfo.queInfo.numCh; chId++)
    {
        UTILS_assert(chId < MATCHTEMPLATECLASSIC_LINK_MAX_CH);
        /*
         * Free allocated memory for alg internal objects
         */
        AlgorithmLink_matchTemplateClassicAlgDelete(&pMTCObj->chObj[chId]);

        /*
         * Free link buffers
         */
        for (bufId = 0; bufId < pMTCObj->algLinkCreateParams.numOutBuffers; bufId++)
        {
            pMetaDataBuffer = &pMTCObj->chObj[chId].metaDataBuffers[bufId];

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
        pMTCObj,
        sizeof(AlgorithmLink_MatchTemplateClassicObj)
        );

    return status;
}

Int32 AlgorithmLink_matchTemplateClassicPrintStatistics(void *pObj,
                AlgorithmLink_MatchTemplateClassicObj *pMTCObj)
{
    UTILS_assert(NULL != pMTCObj->linkStatsInfo);

    Utils_printLinkStatistics(
        &pMTCObj->linkStatsInfo->linkStats,
        "ALG_MATCH_TEMPLATE_CLASSIC",
        TRUE
        );

    Utils_printLatency(
        "ALG_MATCH_TEMPLATE_CLASSIC",
        &pMTCObj->linkStatsInfo->linkLatency,
        &pMTCObj->linkStatsInfo->srcToLinkLatency,
        TRUE
        );

    return SYSTEM_LINK_STATUS_SOK;
}

