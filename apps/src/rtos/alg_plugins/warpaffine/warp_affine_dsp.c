#include "warp_affine_dsp.h"
#include "vlib.h"
#include "vxlib.h"
#include "math.h"
#include "src/rtos/utils_common/include/utils_mem.h"

#define FIX_WA_LENGTH     (128)
#define FIX_CONCAT_NUM    (6)
#define FIX_CHANNEL_NUM   (3)

void WarpAffineNearest_concat3x6x128x128(const uint8_t src[restrict],
                                uint32_t src_stride,
                                uint8_t dst[restrict],
                                const VXLIB_F32 warpMatrix[6],
                                uint8_t image_idx);

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

  //fast optimized code , compute and directly return
  if ((dst_addr.dim_x == FIX_WA_LENGTH * FIX_CONCAT_NUM) 
      && (dst_addr.dim_y == FIX_WA_LENGTH) 
      && (3 == channel))
  {
    //must concat 6 images
    for(i = 0; i < number; i++)
    {
      WarpAffineNearest_concat3x6x128x128(src_vpe,src_addr.stride_y,dst,warpMatrix[i],i);      
    }
    for (i = number; i < FIX_CONCAT_NUM; i++)
    {
      WarpAffineNearest_concat3x6x128x128(src_vpe,src_addr.stride_y,dst,warpMatrix[number - 1], i); 
    }

    return status_opt;
  }
  
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
    for(i = 0; i < number; i++)
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

CODE_SECTION(WarpAffineNearest_concat3x6x128x128,   ".text:optimized")
CODE_SECTION(WarpAffineNearest_concat_core,         ".text:optimized")

static inline void WarpAffineNearest_concat_core(const uint8_t src[restrict],
                                                       uint8_t dst[restrict],
                                                       __float2_t m10,
                                                       __float2_t m32,
                                                       __float2_t m54,
                                                       uint32_t dst_y,
                                                       uint32_t src_stride)
{
    uint32_t    x;
    __float2_t remap_0f, remap_1f, remap_2f, remap_3f;
    int32_t remap_0, remap_1, remap_2, remap_3;
    int32_t offset;
    uint32_t out0_0, out0_1, out0_2;
    uint32_t out1_0, out1_1, out1_2;
    uint32_t out2_0, out2_1, out2_2;
    uint32_t out3_0, out3_1, out3_2;

    uint32_t out01, out23;
    uint32_t offsetCalc = ((src_stride*FIX_CHANNEL_NUM) << 16) + FIX_CHANNEL_NUM;

    /* Prepare registers for y coordiate SIMD computation */
    __float2_t y2     = _dintspu(_itoll(dst_y, dst_y));

    /* Affine equations: */
    /*
     *       | Inside  | Outside loop   |
     *   x' = x * m[0] + y * m[2] + m[4];
     *   y' = x * m[1] + y * m[3] + m[5];
     *
     * The y and offset terms can be computed outside the loop
     */
    __float2_t ym32    = _dmpysp(y2, m32);
    __float2_t ym32m54 = _daddsp(ym32, m54);

    /* Case 1: SIMD of 4 pixels at a time */
    /* 6/4 = 1.5 cycles per pixel */
    for( x=0; x < FIX_WA_LENGTH; x+=4 ) {
        /* Prepare registers for x coordiate SIMD computation */
        __float2_t x2_0  = _dintspu(_itoll(x, x));
        __float2_t x2_1  = _dintspu(_itoll(x+1U, x+1U));
        __float2_t x2_2  = _dintspu(_itoll(x+2U, x+2U));
        __float2_t x2_3  = _dintspu(_itoll(x+3U, x+3U));

        /* Add x terms of affine function to complete equation */
        remap_0f = _daddsp(_dmpysp(x2_0, m10), ym32m54);
        remap_1f = _daddsp(_dmpysp(x2_1, m10), ym32m54);
        remap_2f = _daddsp(_dmpysp(x2_2, m10), ym32m54);
        remap_3f = _daddsp(_dmpysp(x2_3, m10), ym32m54);

        /* Round coordinates to nearest neighbor */
        remap_0 = (int32_t)_dspinth(remap_0f);
        remap_1 = (int32_t)_dspinth(remap_1f);
        remap_2 = (int32_t)_dspinth(remap_2f);
        remap_3 = (int32_t)_dspinth(remap_3f);

        /* Fetch pixels from src buffer */
        offset = _dotp2(remap_0, (int32_t)offsetCalc);
        //offset = offset * FIX_CHANNEL_NUM;       
        out0_0 = src[offset];
        out0_1 = src[offset + 1];
        out0_2 = src[offset + 2];
        offset = _dotp2(remap_1, (int32_t)offsetCalc);
        //offset = offset * FIX_CHANNEL_NUM;       
        out1_0 = src[offset];
        out1_1 = src[offset + 1];
        out1_2 = src[offset + 2];
        offset = _dotp2(remap_2, (int32_t)offsetCalc);
        //offset = offset * FIX_CHANNEL_NUM;       
        out2_0 = src[offset];
        out2_1 = src[offset + 1];
        out2_2 = src[offset + 2];
        offset = _dotp2(remap_3, (int32_t)offsetCalc);
        //offset = offset * FIX_CHANNEL_NUM;       
        out3_0 = src[offset];
        out3_1 = src[offset + 1];
        out3_2 = src[offset + 2];
        /* Pack output pixels together before writing */

        // out01 = (out1 << 16) | out0;
        // out23 = (out3 << 16) | out2;
        // _mem4(&dst[x]) = _spacku4((int32_t)out23, (int32_t)out01);

        out01 = (out1_0 << 16) | out0_0;
        out23 = (out3_0 << 16) | out2_0;
        _mem4(&dst[x]) = _spacku4((int32_t)out23, (int32_t)out01);

        out01 = (out1_1 << 16) | out0_1;
        out23 = (out3_1 << 16) | out2_1;
        _mem4(&dst[x + FIX_WA_LENGTH*FIX_WA_LENGTH*FIX_CONCAT_NUM]) = _spacku4((int32_t)out23, (int32_t)out01);

        out01 = (out1_2 << 16) | out0_2;
        out23 = (out3_2 << 16) | out2_2;
        _mem4(&dst[x + FIX_WA_LENGTH*FIX_WA_LENGTH*FIX_CONCAT_NUM*2]) = _spacku4((int32_t)out23, (int32_t)out01);
    }
}

//jianghao: optimized code, param MUST be checked by outside user
void WarpAffineNearest_concat3x6x128x128(const uint8_t src[restrict],
                                uint32_t src_stride,
                                uint8_t dst[restrict],
                                const VXLIB_F32 warpMatrix[6],
                                uint8_t image_idx)
{
    uint32_t    y;
    uint32_t    x_offset = FIX_WA_LENGTH * image_idx;
    uint32_t    dst_stride = FIX_WA_LENGTH * FIX_CONCAT_NUM;

    VXLIB_F32 matrix_45[2];
    matrix_45[0] = warpMatrix[4]+0.5;
    matrix_45[1] = warpMatrix[5]+0.5;

    /* Fetch affine coordinates */
    __float2_t m10    = _mem8_f2_const(&warpMatrix[0]);
    __float2_t m32    = _mem8_f2_const(&warpMatrix[2]);
    __float2_t m54    = _mem8_f2_const(&matrix_45[0]);

    for (y = 0; y < FIX_WA_LENGTH; y++) 
    {
        uint8_t *dst_t  = (uint8_t *)&dst[y * dst_stride + x_offset];
        WarpAffineNearest_concat_core(src, dst_t, m10, m32, m54, y, src_stride);
    }
}
