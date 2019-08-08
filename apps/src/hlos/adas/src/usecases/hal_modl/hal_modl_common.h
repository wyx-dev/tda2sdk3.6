/******************************************************************************
*
* Copyright (C) 2018-2019 Momenta Incorporated - https://www.momenta.ai/
*
* ALL RIGHTS RESERVED
*
******************************************************************************/

/*
 *******************************************************************************
 *
 * \file hal_modl_common.h
 *
 * \brief This file contains common utility functions used by TIDL.
 *
 *******************************************************************************
 */

#ifndef _CHAINS_HAL_TIDL_COMMON_H_
#define _CHAINS_HAL_TIDL_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *  Includes
 *******************************************************************************
 */
#include "itidl_ti.h"

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */

/*******************************************************************************
 *  Function Declarations
 *******************************************************************************
 */
Int32 hal_modl_ReadNet(sTIDL_Network_t *net, char *netFile);
Int32 hal_modl_AllocNetParamsMem(sTIDL_Network_t *net);
Int32 hal_modl_FillNetParamsMem(sTIDL_Network_t *net, char *netPrm);
Int32 hal_modl_FreeNetParamsMem(sTIDL_Network_t *net);
Int32 hal_modl_NetParamsMemPhys2Virt(sTIDL_Network_t *net);
Int32 hal_modl_NetParamsMemVirt2Phys(sTIDL_Network_t *net);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _CHAINS_HAL_TIDL_COMMON_H_ */


