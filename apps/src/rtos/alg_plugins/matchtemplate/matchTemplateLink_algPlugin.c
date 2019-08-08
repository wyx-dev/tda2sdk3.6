/******************************************************************************
*
* Copyright (C) 2018-2019 Momenta Incorporated - https://www.momenta.ai/
*
* ALL RIGHTS RESERVED
*
******************************************************************************/

/**
 *******************************************************************************
 * \file matchTemplateLink_algPlugin.c
 *
 * \brief  This file contains plug in functions for Match Template
 *         Link
 *
 * \version 0.0 (Nov 2018) : [NN] First version
 *
 *******************************************************************************
*/

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */

#include "matchTemplateLink_priv.h"
#include <include/link_api/system_common.h>
#include <src/rtos/utils_common/include/utils_mem.h>

/**
 *******************************************************************************
 *
 * \brief Implementation of function to init plugins()
 *
 *        This function will be called by AlgorithmLink_initAlgPlugins, so as
 *        register plugins of Match Template algorithm
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 AlgorithmLink_matchTemplate_initPlugin()
{
    AlgorithmLink_FuncTable pluginFunctions;
    UInt32 algId = (UInt32)-1;

    pluginFunctions.AlgorithmLink_AlgPluginCreate =
        AlgorithmLink_matchTemplateCreate;
    pluginFunctions.AlgorithmLink_AlgPluginProcess =
        AlgorithmLink_matchTemplateProcess;
    pluginFunctions.AlgorithmLink_AlgPluginControl =
        AlgorithmLink_matchTemplateControl;
    pluginFunctions.AlgorithmLink_AlgPluginStop =
        AlgorithmLink_matchTemplateStop;
    pluginFunctions.AlgorithmLink_AlgPluginDelete =
        AlgorithmLink_matchTemplateDelete;

    algId = ALGORITHM_LINK_DSP_ALG_MATCH_TEMPLATE;

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
UInt32 AlgorithmLink_matchTemplateAlgCreate(AlgorithmLink_MatchTemplateChObj *pObj)
{
    /*
     * Intialize the algorithm instance with the allocated memory
     * and user create parameters
     */
    pObj->algMtHandle = AlgIvision_create(
                            &MT_MOMENTA_VISION_FXNS,
                            (IALG_Params*)&pObj->algMtCreateParams
                            );
    UTILS_assert(NULL != pObj->algMtHandle);

    pObj->algMtOutBufSize = MT_ALGLINK_OUT_BUF_SIZE;

    pObj->pAlgMtOutBuf =
        Utils_memAlloc(
            UTILS_HEAPID_DDR_CACHED_SR,
            pObj->algMtOutBufSize,
            128
            );
    UTILS_assert(NULL != pObj->pAlgMtOutBuf);

    pObj->algMtOutBufDesc.bufPlanes[0].buf = pObj->pAlgMtOutBuf;

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
UInt32 AlgorithmLink_matchTemplateAlgDelete(AlgorithmLink_MatchTemplateChObj *pObj)
{
    Int32 status;

    status = AlgIvision_delete(pObj->algMtHandle);
    UTILS_assert(SYSTEM_LINK_STATUS_SOK == status);

    status = Utils_memFree(
                 UTILS_HEAPID_DDR_CACHED_SR,
                 pObj->pAlgMtOutBuf,
                 pObj->algMtOutBufSize
                 );
    UTILS_assert(SYSTEM_LINK_STATUS_SOK == status);

    return status;
}

/**
 *******************************************************************************
 *
 * \brief Match Template Alg uses the IVISION standard to interact with the
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
Int32 AlgorithmLink_matchTemplateAlgParamsInit(
                                  AlgorithmLink_MatchTemplateObj *pLinkObj,
                                  AlgorithmLink_MatchTemplateChObj *pObj,
                                  System_LinkChInfo *inChInfo)
{
    pObj->algMtInBufs.size                              = sizeof(pObj->algMtInBufs);
    pObj->algMtInBufs.numBufs                           = 1;
    pObj->algMtInBufs.bufDesc                           = pObj->algMtInBufDescList;
    pObj->algMtInBufDescList[0]                         = &pObj->algMtInBufDesc;

    pObj->algMtOutBufs.size                             = sizeof(pObj->algMtOutBufs);
    pObj->algMtOutBufs.numBufs                          = 1;
    pObj->algMtOutBufs.bufDesc                          = pObj->algMtOutBufDescList;
    pObj->algMtOutBufDescList[0]                        = &pObj->algMtOutBufDesc;

    memset(&pObj->algMtInArgs, 0, sizeof(pObj->algMtInArgs));
    pObj->algMtInArgs.iVisionInArgs.size                = sizeof(pObj->algMtInArgs);
    pObj->algMtInArgs.iVisionInArgs.subFrameInfo        = 0;

    memset(&pObj->algMtOutArgs, 0, sizeof(pObj->algMtOutArgs));
    pObj->algMtOutArgs.iVisionOutArgs.size              = sizeof(pObj->algMtOutArgs);

    memset(&pObj->algMtInBufDesc, 0, sizeof(pObj->algMtInBufDesc));
    pObj->algMtInBufDesc.numPlanes                      = 1;
    /* Will be filled with input pointer later
     */
    pObj->algMtInBufDesc.bufPlanes[0].buf               = NULL;
    pObj->algMtInBufDesc.bufPlanes[0].width             = inChInfo->pitch[0];
    pObj->algMtInBufDesc.bufPlanes[0].height            = inChInfo->height;
    pObj->algMtInBufDesc.bufPlanes[0].frameROI.width    = pLinkObj->algLinkCreateParams.imgFrameWidth;
    pObj->algMtInBufDesc.bufPlanes[0].frameROI.height   = pLinkObj->algLinkCreateParams.imgFrameHeight;
    pObj->algMtInBufDesc.formatType                     = 0; /* NOT USED */
    pObj->algMtInBufDesc.bufferId                       = 0xFF; /* NOT USED */

    memset(&pObj->algMtOutBufDesc, 0, sizeof(pObj->algMtOutBufDesc));
    pObj->algMtOutBufDesc.numPlanes                     = 1;
    /* Will be filled with input pointer later
     */
    pObj->algMtOutBufDesc.bufPlanes[0].buf              = NULL;

    pObj->algMtCreateParams.visionParams.algParams.size = sizeof(pObj->algMtCreateParams);
    pObj->algMtCreateParams.visionParams.cacheWriteBack = NULL;
    pObj->algMtCreateParams.imgWidth                    = pLinkObj->algLinkCreateParams.imgFrameWidth;
    pObj->algMtCreateParams.imgHeight                   = pLinkObj->algLinkCreateParams.imgFrameHeight;
    pObj->algMtCreateParams.imgPitch                    = pLinkObj->algLinkCreateParams.imgFramePitch;
    pObj->algMtCreateParams.patchWidth                  = pLinkObj->algLinkCreateParams.patchWidth;
    pObj->algMtCreateParams.patchHeight                 = pLinkObj->algLinkCreateParams.patchHeight;
    pObj->algMtCreateParams.searchWidth                 = pLinkObj->algLinkCreateParams.searchWidth;
    pObj->algMtCreateParams.searchHeight                = pLinkObj->algLinkCreateParams.searchHeight;

    return SYSTEM_LINK_STATUS_SOK;
}

/**
 *******************************************************************************
 *
 * \brief Implementation of Create Plugin for Match Template alg link
 *
 *
 * \param  pObj              [IN] Algorithm link object handle
 * \param  pCreateParams     [IN] Pointer to create time parameters
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 AlgorithmLink_matchTemplateCreate(void *pObj, void *pCreateParams)
{
    UInt32                         status;
    UInt32                         prevLinkQId;
    UInt32                         bufId, chId;
    System_LinkInfo                prevLinkInfo;
    AlgorithmLink_MatchTemplateObj *pMTObj;
    AlgorithmLink_OutputQueueInfo  *pOutputQInfo;
    AlgorithmLink_InputQueueInfo   *pInputQInfo;
    System_LinkChInfo              *pOutChInfo;
    System_LinkChInfo              *pPrevChInfo;
    System_Buffer                  *pSystemBuffer;
    System_MetaDataBuffer          *pMetaDataBuffer;

    pMTObj = (AlgorithmLink_MatchTemplateObj *)
                 Utils_memAlloc(
                     UTILS_HEAPID_DDR_CACHED_LOCAL,
                     sizeof(AlgorithmLink_MatchTemplateObj),
                     32
                     );
    UTILS_assert(NULL != pMTObj);

    AlgorithmLink_setAlgorithmParamsObj(pObj, pMTObj);

    pInputQInfo = &pMTObj->inputQInfo;
    pOutputQInfo = &pMTObj->outputQInfo;

    /*
     * Taking copy of needed create time parameters in local object for future
     * reference.
     */
    memcpy(&pMTObj->algLinkCreateParams, pCreateParams, sizeof(pMTObj->algLinkCreateParams));

    pInputQInfo->qMode = ALGORITHM_LINK_QUEUEMODE_NOTINPLACE;
    pOutputQInfo->qMode = ALGORITHM_LINK_QUEUEMODE_NOTINPLACE;

    status = System_linkGetInfo(
                 pMTObj->algLinkCreateParams.inQueParams.prevLinkId,
                 &prevLinkInfo
                 );
    UTILS_assert(SYSTEM_LINK_STATUS_SOK == status);

    UTILS_assert(prevLinkInfo.numQue >= 1);

    prevLinkQId = pMTObj->algLinkCreateParams.inQueParams.prevLinkQueId;

    UTILS_assert(prevLinkInfo.queInfo[prevLinkQId].numCh < MATCHTEMPLATE_LINK_MAX_CH);

    if(pMTObj->algLinkCreateParams.numOutBuffers
        > MATCHTEMPLATE_LINK_MAX_NUM_OUTPUT)
    {
        pMTObj->algLinkCreateParams.numOutBuffers
            = MATCHTEMPLATE_LINK_MAX_NUM_OUTPUT;
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
        UTILS_assert(chId < MATCHTEMPLATE_LINK_MAX_CH);
        pPrevChInfo = &prevLinkInfo.queInfo[prevLinkQId].chInfo[chId];

        AlgorithmLink_matchTemplateAlgParamsInit(pMTObj, &pMTObj->chObj[chId], pPrevChInfo);

        AlgorithmLink_matchTemplateAlgCreate(&pMTObj->chObj[chId]);

        /*
         * Allocate memory for the output buffers and link metadata buffer with
         * system Buffer
         */

        for (bufId = 0; bufId < pMTObj->algLinkCreateParams.numOutBuffers; bufId++)
        {
            pSystemBuffer   = &pMTObj->chObj[chId].buffers[bufId];
            pMetaDataBuffer = &pMTObj->chObj[chId].metaDataBuffers[bufId];

            /*
             * Properties of pSystemBuffer, which do not get altered during
             * run time (frame exchanges) are initialized here
             */
            pSystemBuffer->bufType     = SYSTEM_BUFFER_TYPE_METADATA;
            pSystemBuffer->payload     = pMetaDataBuffer;
            pSystemBuffer->payloadSize = sizeof(System_MetaDataBuffer);
            pSystemBuffer->chNum       = chId;

            pMetaDataBuffer->numMetaDataPlanes = 1;
            pMetaDataBuffer->metaBufSize[0] = pMTObj->chObj[chId].algMtOutBufSize;

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

    pMTObj->isFirstFrameRecv = FALSE;

    /* Assign pointer to link stats object */
    pMTObj->linkStatsInfo = Utils_linkStatsCollectorAllocInst(
                                AlgorithmLink_getLinkId(pObj),
                                "ALG_MATCH_TEMPLATE"
                                );
    UTILS_assert(NULL != pMTObj->linkStatsInfo);

    return SYSTEM_LINK_STATUS_SOK;
}

/**
 *******************************************************************************
 *
 * \brief Implementation of Process Plugin for Match Template alg link
 *
 *
 * \param  pObj              [IN] Algorithm link object handle
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 AlgorithmLink_matchTemplateProcess(void *pObj)
{
    UInt32                                  bufId, chId;
    Int32                                   status = SYSTEM_LINK_STATUS_SOK;
    Bool                                    bufDropFlag;
    AlgorithmLink_MatchTemplateObj          *pMTObj;
    AlgorithmLink_MatchTemplateChObj        *pChObj;
    System_BufferList                       inputBufList;
    AlgorithmLink_MatchTemplateCreateParams *pLinkCreatePrms;
    System_Buffer                           *pSysOutBuffer;
    System_Buffer                           *pSysInBuffer;
    System_VideoFrameBuffer                 *pInVideoBuf;
    System_MetaDataBuffer                   *pOutMetaBuf;
    System_BufferList                       bufListReturn;
    System_LinkStatistics                   *linkStatsInfo;

    pMTObj = (AlgorithmLink_MatchTemplateObj *)
                                AlgorithmLink_getAlgorithmParamsObj(pObj);

    pLinkCreatePrms = &pMTObj->algLinkCreateParams;

    linkStatsInfo = pMTObj->linkStatsInfo;
    UTILS_assert(NULL != linkStatsInfo);

    Utils_linkStatsCollectorProcessCmd(linkStatsInfo);

    if (pMTObj->isFirstFrameRecv == FALSE)
    {
        pMTObj->isFirstFrameRecv = TRUE;

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
            if(pSysInBuffer->chNum >= pMTObj->outputQInfo.queInfo.numCh)
            {
                linkStatsInfo->linkStats.inBufErrorCount++;
                continue;
            }

            chId = pSysInBuffer->chNum;

            pChObj = &pMTObj->chObj[chId];

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
               
                pChObj->algMtInBufDesc.bufPlanes[0].buf = (Void *)(pInVideoBuf->bufAddr[0]);

                status = AlgIvision_process(
                             pChObj->algMtHandle,
                             &pChObj->algMtInBufs,
                             &pChObj->algMtOutBufs,
                             (IVISION_InArgs*)&pChObj->algMtInArgs,
                             (IVISION_OutArgs *)&pChObj->algMtOutArgs
                             );
                UTILS_assert(IALG_EOK == status);

                Cache_wb(
                    (UInt8*)pChObj->pAlgMtOutBuf,
                    MT_ALGLINK_OUT_BUF_SIZE,
                    Cache_Type_ALLD,
                    TRUE
                    );

                EDMA_UTILS_memcpy2D(
                    pOutMetaBuf->bufAddr[0],
                    pChObj->pAlgMtOutBuf,
                    MT_ALGLINK_OUT_BUF_SIZE,
                    1,
                    MT_ALGLINK_OUT_BUF_SIZE,
                    MT_ALGLINK_OUT_BUF_SIZE
                    );

                Cache_wb(
                    (UInt8*)pOutMetaBuf->bufAddr[0],
                    MT_ALGLINK_OUT_BUF_SIZE,
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
 * \brief Implementation of Control Plugin for Match Template alg link
 *
 *
 * \param  pObj              [IN] Algorithm link object handle
 * \param  pControlParams    [IN] Pointer to control parameters
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 AlgorithmLink_matchTemplateControl(void *pObj, void *pControlParams)
{
    AlgorithmLink_MatchTemplateObj *pMTObj;
    AlgorithmLink_ControlParams    *pAlgLinkControlPrm;
    Int32                          status = SYSTEM_LINK_STATUS_SOK;

    pMTObj = (AlgorithmLink_MatchTemplateObj *)
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
            AlgorithmLink_matchTemplatePrintStatistics(pObj, pMTObj);
            break;

        default:
            break;
    }

    return status;
}

/**
 *******************************************************************************
 *
 * \brief Implementation of Stop Plugin for Match Template alg link
 *
 *
 * \param  pObj              [IN] Algorithm link object handle
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 AlgorithmLink_matchTemplateStop(void *pObj)
{
    return 0;
}

/**
 *******************************************************************************
 *
 * \brief Implementation of Delete Plugin for Match Template alg link
 *
 *
 * \param  pObj              [IN] Algorithm link object handle
 *
 * \return  SYSTEM_LINK_STATUS_SOK on success
 *
 *******************************************************************************
 */
Int32 AlgorithmLink_matchTemplateDelete(void *pObj)
{
    Int32                          status = SYSTEM_LINK_STATUS_SOK;
    UInt32                         bufId, chId;
    AlgorithmLink_MatchTemplateObj *pMTObj;
    System_MetaDataBuffer          *pMetaDataBuffer;

    pMTObj = (AlgorithmLink_MatchTemplateObj *)
                                AlgorithmLink_getAlgorithmParamsObj(pObj);

    status = Utils_linkStatsCollectorDeAllocInst(pMTObj->linkStatsInfo);
    UTILS_assert(SYSTEM_LINK_STATUS_SOK == status);

    for(chId=0; chId<pMTObj->outputQInfo.queInfo.numCh; chId++)
    {
        UTILS_assert(chId < MATCHTEMPLATE_LINK_MAX_CH);
        /*
         * Free allocated memory for alg internal objects
         */
        AlgorithmLink_matchTemplateAlgDelete(&pMTObj->chObj[chId]);

        /*
         * Free link buffers
         */
        for (bufId = 0; bufId < pMTObj->algLinkCreateParams.numOutBuffers; bufId++)
        {
            pMetaDataBuffer = &pMTObj->chObj[chId].metaDataBuffers[bufId];

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
        pMTObj,
        sizeof(AlgorithmLink_MatchTemplateObj)
        );

    return status;
}

Int32 AlgorithmLink_matchTemplatePrintStatistics(void *pObj,
                AlgorithmLink_MatchTemplateObj *pMTObj)
{
    UTILS_assert(NULL != pMTObj->linkStatsInfo);

    Utils_printLinkStatistics(
        &pMTObj->linkStatsInfo->linkStats,
        "ALG_MATCH_TEMPLATE",
        TRUE
        );

    Utils_printLatency(
        "ALG_MATCH_TEMPLATE",
        &pMTObj->linkStatsInfo->linkLatency,
        &pMTObj->linkStatsInfo->srcToLinkLatency,
        TRUE
        );

    return SYSTEM_LINK_STATUS_SOK;
}

