#include "warp_affine_dsp.h"
#include "vlib.h"
#include "vxlib.h"
#include "math.h"
#include "src/rtos/utils_common/include/utils_mem.h"

int32_t warp_affine_dsp(uint8_t *in_buf, uint8_t *out_buf)
{
  VXLIB_STATUS status_opt = VXLIB_SUCCESS;
  uint8_t *src = in_buf;
  uint8_t *dst = out_buf;
  uint32_t type = WARP_AFFINE_TYPE_BILINEAR;
  uint32_t channel = 0;
  uint32_t number = 0;
  uint32_t i = 0;
  VXLIB_bufParams2D_t src_addr = {0};
  VXLIB_bufParams2D_t dst_addr = {0};
  float warpMatrix[20][6] = {0.0f};
  int16_t srcOffsetX = 0;
  int16_t srcOffsetY = 0;
  int16_t dstOffsetX = 0;
  int16_t dstOffsetY = 0;
  uint8_t *src_vpe = NULL;

  Cache_inv(src, WARP_AFFINE_PRM_SIZE, Cache_Type_ALL, TRUE);

  type = *((uint32_t*)src);
  src += sizeof(uint32_t);

  channel = *((uint32_t*)src);
  src += sizeof(uint32_t);

  number = *((uint32_t*)src);
  src += sizeof(uint32_t);

  src_addr.data_type = VXLIB_UINT8;
  src_addr.dim_x = *((uint32_t*)src);
  src += sizeof(uint32_t);
  src_addr.dim_y = *((uint32_t*)src);
  src += sizeof(uint32_t);
  src_addr.stride_y = *((int32_t*)src);
  src += sizeof(int32_t);

  dst_addr.data_type = VXLIB_UINT8;
  dst_addr.dim_x = *((uint32_t*)src);
  src += sizeof(uint32_t);
  dst_addr.dim_y = *((uint32_t*)src);
  src += sizeof(uint32_t);
  dst_addr.stride_y = *((int32_t*)src);
  src += sizeof(int32_t);

  memcpy(warpMatrix, src, sizeof(warpMatrix));
  src += sizeof(warpMatrix);

  srcOffsetX = *((int16_t*)src);
  src += sizeof(int16_t);
  srcOffsetY = *((int16_t*)src);
  src += sizeof(int16_t);
  dstOffsetX = *((int16_t*)src);
  src += sizeof(int16_t);
  dstOffsetY = *((int16_t*)src);
  src += sizeof(int16_t);

  Cache_inv(src, sizeof(uint32_t), Cache_Type_ALL, TRUE);
  src_vpe = (uint8_t*)(*((uint32_t*)src));

  Cache_inv(src_vpe, channel * src_addr.dim_y * src_addr.stride_y, Cache_Type_ALL, TRUE);

  if((WARP_AFFINE_TYPE_BILINEAR == type) && (1 == channel))
  {
    for(i=0; i<number; i++)
    {
      status_opt = VXLIB_warpAffineBilinear_i8u_c32f_o8u(src_vpe, &src_addr,
                                                         dst+i*WARP_AFFINE_SINGLE_OUT_BUF_SIZE, &dst_addr, warpMatrix[i],
                                                         srcOffsetX, srcOffsetY, dstOffsetX, dstOffsetY);                                                       
    }
  }
  else if((WARP_AFFINE_TYPE_BILINEAR == type) && (3 == channel))
  {
    for(i=0; i<number; i++)
    {
      //Vps_printf("******************WA:DSP vlib proc!num:%d,src:%d,%d,wamatrix:%f,%f,%f,%f,addr:%x,%x!\n",
      //        i,src_addr.dim_x,src_addr.dim_y,warpMatrix[i][0],warpMatrix[i][3],warpMatrix[i][4],warpMatrix[i][5],
      //        src_vpe,dst);
      status_opt = Now_VXLIB_warpAffineBilinear_i8u_c32f_o8u(src_vpe, &src_addr,
                                                             dst+i*WARP_AFFINE_SINGLE_OUT_BUF_SIZE, &dst_addr, warpMatrix[i],
                                                             srcOffsetX, srcOffsetY, dstOffsetX, dstOffsetY);
    }
  }
  else if((WARP_AFFINE_TYPE_NEAREST == type) && (1 == channel))
  {
    for(i=0; i<number; i++)
    {
      status_opt = VXLIB_warpAffineNearest_i8u_c32f_o8u(src_vpe, &src_addr,
                                                        dst+i*WARP_AFFINE_SINGLE_OUT_BUF_SIZE, &dst_addr, warpMatrix[i],
                                                        srcOffsetX, srcOffsetY, dstOffsetX, dstOffsetY);
    }
  }
  else if((WARP_AFFINE_TYPE_NEAREST == type) && (3 == channel))
  {
    for(i=0; i<number; i++)
    {
      //Vps_printf("******************WA:DSP NEAREST vlib proc!num:%d,src:%d,%d,wamatrix:%f,%f,%f,%f,addr:%x,%x!\n",
      //        i,src_addr.dim_x,src_addr.dim_y,warpMatrix[i][0],warpMatrix[i][3],warpMatrix[i][4],warpMatrix[i][5],
      //        src_vpe,dst);     
      status_opt = Now_VXLIB_warpAffineNearest_i8u_c32f_o8u(src_vpe, &src_addr,
                                                      dst+i*WARP_AFFINE_SINGLE_OUT_BUF_SIZE, &dst_addr, warpMatrix[i],
                                                      srcOffsetX, srcOffsetY, dstOffsetX, dstOffsetY);
    }
  }
  else
  {
    printf("The specified affine type is not supported.");
  }

  return status_opt;
}

