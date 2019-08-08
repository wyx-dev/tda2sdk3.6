#include "match_template_classic_dsp.h"
#include "math.h"

#define ON_DSP

#include "vlib.h"
#include "vxlib.h"
#include "c6x.h"
#include <float.h>

#include "src/rtos/utils_common/include/utils_mem.h"

#define WARP_MATRIX_NUM     (6)
#define TEMPL_Q_SCALE       (4.0)
//#define USE_TEMPLATE_MEAN    (1)

CODE_SECTION(VLIB_matchTemplate_mine, ".text:optimized")
int32_t VLIB_matchTemplate_mine(uint8_t img[restrict],
                           int32_t imgWidth,
                           int32_t imgHeight,
                           int32_t imgPitch,
                           int16_t tempImg[restrict],
                           int32_t tempImgWidth,
                           int32_t tempImgHeight,
                           int32_t tempImgPitch,
                           VLIB_F32 scaledTempImgVar,
                           uint16_t tempMeanQ2,
                           int32_t* p_x,
                           int32_t* p_y,
                           float* p_score,
                           int32_t outScorePitch,
                           VLIB_F32 outScore[restrict],
                           uint8_t scratch[restrict])
{
    int16_t    outScoreWidth    = imgWidth  - (tempImgWidth - 1);
    int16_t    outScoreHeight   = imgHeight - (tempImgHeight - 1);

    int32_t     i, j, l, k;
    int32_t     normCurWinSq, normCurTempMul;
    VLIB_F32    rcp4;
    VLIB_F32    normCurWinSqF, normCurTempMulF;
    int32_t     curWinAvg;

    uint16_t *restrict    pu16CurWinAvg  = (uint16_t * )scratch; /* Size of outScoreWidth*outScoreHeight */
    uint32_t              totalOutputPlus3 = (outScoreWidth * outScoreHeight) + 3;
    uint32_t              totalOutputPlus1 = (outScoreWidth * outScoreHeight) + 1;
    uint32_t              totalOutputAlign4= totalOutputPlus3 & 0xFFFFFFFCU;
    uint32_t              totalOutputAlign2= totalOutputPlus1 & 0xFFFFFFFEU;

    uint32_t *restrict    pu32CurSqSum   = (uint32_t * )scratch + totalOutputAlign4; /* Size of outScoreWidth*outScoreHeight */
    uint32_t *restrict    pu32CurTempSum = pu32CurSqSum + totalOutputAlign2; /* Size of outScoreWidth*outScoreHeight */

#ifndef USE_TEMPLATE_MEAN
    VLIB_F32    rcpTempArea;
    uint32_t    outputWidthPlus3 = outScoreWidth + 3;
    uint32_t    outputWidthAlign4= outputWidthPlus3 & 0xFFFFFFFCU;
    uint16_t *restrict    pu16CurWinHSum = (uint16_t * )scratch + totalOutputAlign4; /* Size of outScoreWidth*imgHeight   */
    uint8_t  *restrict    pu8TempPtr;
    uint8_t  *restrict    pu8TempPtr1;
    uint8_t  *restrict    pu8TempPtr2;
    uint16_t *restrict    pu16TempPtr;
    uint16_t *restrict    pu16TempPtr1;
    uint16_t *restrict    pu16TempPtr2;
    uint16_t *restrict    pu16CurWinHSumOrig = pu16CurWinHSum;
    uint16_t *restrict    pu16CurWinAvgOrig  = pu16CurWinAvg;
    uint32_t u32temp2, u32temp3, u32temp4;

#endif
    uint8_t  *restrict    pu8temp1;
    int16_t  *restrict    ps16temp1;
    uint64_t              u64temp1, u64temp2, u64temp3;

    uint32_t    u32temp1;

    uint32_t    retVal;
    VLIB_F32    f32temp1;
    __x128_t    u128temp1, u128temp2;
    uint64_t    curWinAvgPkd;
    //unsigned int start,end;
    VLIB_F32    max_score;
    int max_x,max_y,little_left,little_right,little_bottom,little_top;

    TSCL = 0;

    if((tempImgWidth < 4) || (tempImgHeight < 4) || (img == NULL) ||
       (tempImg == NULL) || (scratch == NULL) || (imgPitch < imgWidth) ||
       (tempImgPitch < tempImgWidth) ||
       (tempImgWidth > 128) || (tempImgHeight > 128) ||
       (outScorePitch < outScoreWidth)) {

        retVal = 1;

    } else {

#ifdef USE_TEMPLATE_MEAN
        pu16CurWinAvg[0] = tempMeanQ2;
        rcp4      = _rcpsp(4.0f);
#else
        //start = TSCL;

        rcpTempArea = _rcpsp(tempImgHeight * tempImgWidth);
        rcp4      = _rcpsp(4.0f);

        /*Sliding Horizontal sum of original image pixels. Size of the horzontal
          summed data will be outScoreWidth*imgHeight
         */
        for( l = 0; l < imgHeight; l++ ) {

            pu8TempPtr     = &img[(l * imgPitch) + 0];
            pu16CurWinHSum = &pu16CurWinHSumOrig[l * outScoreWidth];
            u32temp1       = 0;

            /*First four element of each row*/
            for( i = 0; i < tempImgWidth; i++ ) {
                u32temp1 += pu8TempPtr[i];
            }

            *pu16CurWinHSum = u32temp1;
            pu16CurWinHSum++;

            pu8TempPtr1 = &img[(l * imgPitch) + 0];
            pu8TempPtr2 = &img[(l * imgPitch) + tempImgWidth];

            /*4th element to last four byte aligned location*/
            for( k = 1; k < outScoreWidth; k++ ) {

                u32temp1        = (u32temp1 - pu8TempPtr1[0]) + pu8TempPtr2[0];
                *pu16CurWinHSum = u32temp1;

                pu16CurWinHSum++;
                pu8TempPtr1++;
                pu8TempPtr2++;
            }
        }

        /* Vertical sum of the horizontal summed data */
        for( l = 0; l < outputWidthAlign4; l += 4 ) {

            pu16TempPtr   = &pu16CurWinHSumOrig[l];
            pu16CurWinAvg = &pu16CurWinAvgOrig[l];
            u64temp1      = 0;

            u32temp1   = 0;
            u32temp2   = 0;
            u32temp3   = 0;
            u32temp4   = 0;

            /*First four element of each row*/
            for( i = 0; i < tempImgHeight; i++ ) {
                u64temp1       = _mem8_const(pu16TempPtr);
                u32temp1      += (_loll(u64temp1) & 0x0000FFFFU);
                u32temp2      += (_loll(u64temp1) >> 0x10U);
                u32temp3      += (_hill(u64temp1) & 0x0000FFFFU);
                u32temp4      += (_hill(u64temp1) >> 0x10U);
                pu16TempPtr   += outScoreWidth;
            }

            f32temp1       = rcpTempArea * ((VLIB_F32)u32temp1 * 4.0f);
            *pu16CurWinAvg  = (uint16_t)f32temp1;
            pu16CurWinAvg++;

            f32temp1       = rcpTempArea * ((VLIB_F32)u32temp2 * 4.0f);
            *pu16CurWinAvg  = (uint16_t)f32temp1;
            pu16CurWinAvg++;

            f32temp1       = rcpTempArea * ((VLIB_F32)u32temp3 * 4.0f);
            *pu16CurWinAvg  = (uint16_t)f32temp1;
            pu16CurWinAvg++;

            f32temp1       = rcpTempArea * ((VLIB_F32)u32temp4 * 4.0f);
            *pu16CurWinAvg  = (uint16_t)f32temp1;
            pu16CurWinAvg++;

            pu16CurWinAvg += (outScoreWidth - 4);

            pu16TempPtr1 = &pu16CurWinHSumOrig[l + (0 * outScoreWidth)];
            pu16TempPtr2 = &pu16CurWinHSumOrig[l + (tempImgHeight * outScoreWidth)];

            /*4th element to last four byte aligned location*/
            for( k = 1; k < outScoreHeight; k++ ) {

                u64temp1 = _mem8_const(pu16TempPtr1);
                u64temp2 = _mem8_const(pu16TempPtr2);

                u32temp1-= _loll(u64temp1) & 0x0000FFFFU;
                u32temp1+= (_loll(u64temp2) & 0x0000FFFFU);

                u32temp2-= _loll(u64temp1) >> 0x10U;
                u32temp2+= (_loll(u64temp2) >> 0x10U);

                u32temp3-= _hill(u64temp1) & 0x0000FFFFU;
                u32temp3+= (_hill(u64temp2) & 0x0000FFFFU);

                u32temp4-= _hill(u64temp1) >> 0x10U;
                u32temp4+= (_hill(u64temp2) >> 0x10U);

                f32temp1       = rcpTempArea * ((VLIB_F32)u32temp1 * 4.0f);
                *pu16CurWinAvg  = (uint16_t)f32temp1;
                pu16CurWinAvg++;

                f32temp1       = rcpTempArea * ((VLIB_F32)u32temp2 * 4.0f);
                *pu16CurWinAvg  = (uint16_t)f32temp1;
                pu16CurWinAvg++;

                f32temp1       = rcpTempArea * ((VLIB_F32)u32temp3 * 4.0f);
                *pu16CurWinAvg  = (uint16_t)f32temp1;
                pu16CurWinAvg++;

                f32temp1       = rcpTempArea * ((VLIB_F32)u32temp4 * 4.0f);
                *pu16CurWinAvg  = (uint16_t)f32temp1;
                pu16CurWinAvg++;

                pu16TempPtr1   += outScoreWidth;
                pu16TempPtr2   += outScoreWidth;
                pu16CurWinAvg  +=(outScoreWidth - 4);
            }
        }

        pu16CurWinAvg = (uint16_t * )scratch;

        //end = TSCL;
        //Vps_printf("last time 1 is:%d!\n",(end-start));
#endif

        //start = TSCL;

        if((tempImgWidth == 32) && (((uint32_t)tempImg & 0x7U) == 0x0) &&
                  ((tempImgPitch & 0x3U) == 0x0) && (((uint32_t)img & 0x3U) == NULL) &&
                  ((imgPitch & 0x3U) == NULL)) {
            for( l = 0; l < outScoreHeight; l+=2 ) {
                for( k = 0; k < outScoreWidth; k+=2 ) {
                    normCurWinSq   = 0;
                    normCurTempMul = 0;
                    /*Q2 format*/
#ifdef USE_TEMPLATE_MEAN
                    curWinAvg      = pu16CurWinAvg[0];
#else
                    curWinAvg      = pu16CurWinAvg[(l * outScoreWidth) + k];
#endif                    
                    u32temp1       = (curWinAvg << 0x10U) | curWinAvg;
                    curWinAvgPkd   = _itoll(u32temp1, u32temp1);

                    for( i = 0; i < tempImgHeight; i++ ) {
                        pu8temp1  = &img[((l + i) * imgPitch) + (0 + k)];
                        ps16temp1 = &tempImg[(i * tempImgPitch) + 0];
          #pragma UNROLL(8)

                        for( j = 0; j < 32; j += 4 ) {
                            /*Current pixels*/
                            u32temp1  = _mem4_const(pu8temp1);
                            pu8temp1 += 4;
                            /*Template pixels*/
                            u64temp1  = _amem8_const(ps16temp1);
                            ps16temp1+= 4;
                            /*Current pixels, changed to Q2*/
                            u64temp2  = _dshl2(_unpkbu4(u32temp1), (uint32_t)0x2U);
                            /*Mean subtracted current pixels*/
                            u64temp2  = _dsub2(u64temp2, curWinAvgPkd);
                            /*Mean subtracted squared current pixels*/
                            u128temp1 = _dmpy2(u64temp2, u64temp2);
                            u64temp3  = _dadd(_lo128(u128temp1), _hi128(u128temp1));
                            normCurWinSq += (_loll(u64temp3) + _hill(u64temp3));
                            /*Mean subtracted current pixels * template pixel*/
                            u128temp2 = _dmpy2(u64temp2, u64temp1);
                            u64temp3  = _dadd(_lo128(u128temp2), _hi128(u128temp2));
                            normCurTempMul += (_loll(u64temp3) + _hill(u64temp3));
                        }
                    }

                    normCurWinSq   = normCurWinSq   >> 0x2U;
                    normCurTempMul = normCurTempMul >> 0x2U;
                    *pu32CurSqSum   = normCurWinSq;
                    *pu32CurTempSum = normCurTempMul;
                    pu32CurSqSum++;
                    pu32CurTempSum++;

                }
            }
        }
        else
        {
          Vps_printf("Error:Fast mtc width not match!\n");
          return -1;
        }

        //end = TSCL;
        //Vps_printf("last time 2 is:%d!\n",(end-start));

        //start = TSCL;

        pu32CurSqSum   = (uint32_t * )scratch + totalOutputAlign4; /* Size of outScoreWidth*outScoreHeight */
        pu32CurTempSum = pu32CurSqSum + totalOutputAlign2; /* Size of outScoreWidth*outScoreHeight */

        max_score = 0.0;
        max_x = 0;
        max_y = 0;
        for( l = 0; l < outScoreHeight; l+=2 ) {
            for( k = 0; k < outScoreWidth; k+=2 ) {

                normCurWinSq   = *pu32CurSqSum;
                pu32CurSqSum++;
                normCurTempMul = *pu32CurTempSum;
                pu32CurTempSum++;

                normCurWinSqF   = normCurWinSq * rcp4;
                normCurTempMulF = normCurTempMul * rcp4;

                if( normCurWinSqF > FLT_MIN ) {
                    f32temp1 = (normCurTempMulF * scaledTempImgVar) * _rsqrsp(normCurWinSqF);
                } else {
                    f32temp1 = FLT_MAX;
                }

                if (f32temp1 > max_score)
                {
                  max_score = f32temp1;
                  max_x = k;
                  max_y = l;
                }
                
                outScore[(l * outScorePitch) + k] = f32temp1;
            }
        }

        //Vps_printf("coarse max is:x:%d,y:%d,score:%f!\n",max_x,max_y,max_score);

        little_left = (max_x - 2)>0?(max_x - 2):0;
        little_right = (max_x + 2)<outScoreWidth?(max_x + 2):(outScoreWidth-1);
        little_top = (max_y - 2)>0?(max_y - 2):0;
        little_bottom = (max_y + 2)<outScoreHeight?(max_y + 2):(outScoreHeight-1);
        pu32CurSqSum   = (uint32_t * )scratch + totalOutputAlign4; /* Size of outScoreWidth*outScoreHeight */
        pu32CurTempSum = pu32CurSqSum + totalOutputAlign2; /* Size of outScoreWidth*outScoreHeight */
        //second search near max score
        for( l = little_top; l <= little_bottom; l++ ) {
            for( k = little_left; k <= little_right; k++ ) {
                normCurWinSq   = 0;
                normCurTempMul = 0;
                /*Q2 format*/
#ifdef USE_TEMPLATE_MEAN
                    curWinAvg      = pu16CurWinAvg[0];
#else
                    curWinAvg      = pu16CurWinAvg[(l * outScoreWidth) + k];
#endif    
                u32temp1       = (curWinAvg << 0x10U) | curWinAvg;
                curWinAvgPkd   = _itoll(u32temp1, u32temp1);
                curWinAvgPkd = curWinAvgPkd;
                for( i = 0; i < tempImgHeight; i++ ) {
                    pu8temp1  = &img[((l + i) * imgPitch) + (0 + k)];
                    ps16temp1 = &tempImg[(i * tempImgPitch) + 0];
      #pragma UNROLL(8)

                    for( j = 0; j < 32; j += 4 ) {
                        /*Current pixels*/
                        u32temp1  = _mem4_const(pu8temp1);
                        pu8temp1 += 4;
                        /*Template pixels*/
                        u64temp1  = _amem8_const(ps16temp1);
                        ps16temp1+= 4;
                        /*Current pixels, changed to Q2*/
                        u64temp2  = _dshl2(_unpkbu4(u32temp1), (uint32_t)0x2U);
                        /*Mean subtracted current pixels*/
                        u64temp2  = _dsub2(u64temp2, curWinAvgPkd);
                        /*Mean subtracted squared current pixels*/
                        u128temp1 = _dmpy2(u64temp2, u64temp2);
                        u64temp3  = _dadd(_lo128(u128temp1), _hi128(u128temp1));
                        normCurWinSq += (_loll(u64temp3) + _hill(u64temp3));
                        /*Mean subtracted current pixels * template pixel*/
                        u128temp2 = _dmpy2(u64temp2, u64temp1);
                        u64temp3  = _dadd(_lo128(u128temp2), _hi128(u128temp2));
                        normCurTempMul += (_loll(u64temp3) + _hill(u64temp3));
                    }
                }

                normCurWinSq   = normCurWinSq   >> 0x2U;
                normCurTempMul = normCurTempMul >> 0x2U;
                *pu32CurSqSum   = normCurWinSq;
                *pu32CurTempSum = normCurTempMul;
                pu32CurSqSum++;
                pu32CurTempSum++;
            }
        }

        pu32CurSqSum   = (uint32_t * )scratch + totalOutputAlign4; /* Size of outScoreWidth*outScoreHeight */
        pu32CurTempSum = pu32CurSqSum + totalOutputAlign2; /* Size of outScoreWidth*outScoreHeight */
        //Vps_printf("fine zone is:%d,%d,%d,%d!",little_top,little_bottom,little_left,little_right);
        for( l = little_top; l <= little_bottom; l++ ) {
            for( k = little_left; k <= little_right; k++ ) {

                normCurWinSq   = *pu32CurSqSum;
                pu32CurSqSum++;
                normCurTempMul = *pu32CurTempSum;
                pu32CurTempSum++;

                normCurWinSqF   = normCurWinSq * rcp4;
                normCurTempMulF = normCurTempMul * rcp4;

                if( normCurWinSqF > FLT_MIN ) {
                    f32temp1 = (normCurTempMulF * scaledTempImgVar) * _rsqrsp(normCurWinSqF);
                } else {
                    f32temp1 = FLT_MAX;
                }

                if (f32temp1 > max_score)
                {
                  max_score = f32temp1;
                  max_x = k;
                  max_y = l;
                }
                
                outScore[(l * outScorePitch) + k] = f32temp1;
            }
        }

        //Vps_printf("fine max is:x:%d,y:%d,score:%f!\n",max_x,max_y,max_score);

        *p_x = max_x;
        *p_y = max_y;
        *p_score = max_score;

        retVal = 0;
    }

    //end = TSCL;

    //Vps_printf("last time 3 is:%d!\n",(end-start));
    return (retVal);
}

//获取数组中最大值的位置和值
void getMaxLoc(float* array, int width, int height, int* max_x, int * max_y, float* max_value)
{
    int i,j;
    int x=0,y=0;
    float temp_max = array[0];

    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            if (temp_max < array[i*width+j])
            {
                temp_max = array[i*width+j];
                x = j;
                y = i;
            }
        }
    }

    *max_x = x;
    *max_y = y;
    *max_value = temp_max;
    
}

//获取图像均值
float GetImageMean(uint8_t * tmp, int height, int width)
{
    int i,j;
    int sum = 0;
    float result;

    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            sum += tmp[i*width+j];
        }
    }    

    result = ((float)sum)/(height*width);

    return result;
}

//获取图像的标准差
float GetTemplateVar(uint8_t * tmp, int height, int width)
{
    int i,j;
    int element;
    int sum;
    float result;

    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            element = tmp[i*width+j];
            sum += element*element;
        }
    }

    result = (float)sum;
    result = 1.0/sqrt(result);

    return result;
}

//获取去均值以后的模板,并以Q2格式保存，同时返回模板的均方差值
float GetNormedTemplate(uint8_t * in_template, int height, int width, int16_t* out_q_buffer, uint16_t* temp_mean_q2)
{
    int i,j;
    float element;
    float temp_mean;
    float sum = 0.0;
    float result;

    temp_mean = GetImageMean(in_template,height,width);
    *temp_mean_q2 = (uint16_t)(temp_mean*4.0);

    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            element = ((float)in_template[i*width+j]-temp_mean);
            out_q_buffer[i*width+j] = (int16_t)(element*TEMPL_Q_SCALE);
            sum += element*element;
        }
    }

    result = 1.0/sqrt(sum);

    return result;
}

//参数检查
int32_t mt_classic_para_check(MtInParam* param, MtOutResult* out_buf)
{
  int i;

  if ((param == NULL)||(out_buf == NULL))
  {
    return ERR_CODE_1;
  }

  if (param->pad_num > MAX_RECT_NUM)
  {
    return ERR_CODE_2;
  }

  if ((param->cur_image == NULL)||(param->pre_image == NULL))
  {
    return ERR_CODE_3;
  }
  
  for (i = 0; i < param->pad_num; i++)
  {
    //将warpaffine的范围限定在图像以内
    param->templ_pad[i].top = (param->templ_pad[i].top < 0)?0:param->templ_pad[i].top;
    param->templ_pad[i].bottom = (param->templ_pad[i].bottom > ORIGIN_IMG_HEIGHT)?ORIGIN_IMG_HEIGHT:param->templ_pad[i].bottom;
    param->templ_pad[i].left = (param->templ_pad[i].left < 0)?0:param->templ_pad[i].left;
    param->templ_pad[i].right = (param->templ_pad[i].right > ORIGIN_IMG_WIDTH)?ORIGIN_IMG_WIDTH:param->templ_pad[i].right;

    param->cur_pad[i].top = (param->cur_pad[i].top < 0)?0:param->cur_pad[i].top;
    param->cur_pad[i].bottom = (param->cur_pad[i].bottom > ORIGIN_IMG_HEIGHT)?ORIGIN_IMG_HEIGHT:param->cur_pad[i].bottom;
    param->cur_pad[i].left = (param->cur_pad[i].left < 0)?0:param->cur_pad[i].left;
    param->cur_pad[i].right = (param->cur_pad[i].right > ORIGIN_IMG_WIDTH)?ORIGIN_IMG_WIDTH:param->cur_pad[i].right;
  }

  return 0;
}

static uint8_t* temp_buffer = NULL;
static uint8_t* image_buffer = NULL;
static int16_t* templ_q_buffer = NULL;
static float* score_buffer = NULL;
static uint8_t* scrach_buffer = NULL;

void match_template_classic_malloc_buffer()
{
    temp_buffer = (uint8_t*)Utils_memAlloc(UTILS_HEAPID_L2_LOCAL, FIX_TEMPL_WIDTH*MAX_TEMPL_HIGHT, 32);
    image_buffer = (uint8_t*)Utils_memAlloc(UTILS_HEAPID_L2_LOCAL, MAX_CUR_IMG_SIZE, 32);
    templ_q_buffer = (int16_t*)Utils_memAlloc(UTILS_HEAPID_L2_LOCAL, FIX_TEMPL_WIDTH*MAX_TEMPL_HIGHT*sizeof(int16_t), 32);
    score_buffer = (float*)Utils_memAlloc(UTILS_HEAPID_L2_LOCAL, MAX_OUT_SCORE_SIZE*sizeof(float), 32);
    scrach_buffer = (uint8_t*)Utils_memAlloc(UTILS_HEAPID_L2_LOCAL, SCRACH_BUFFER_SIZE, 32);
    //scrach_buffer = (uint8_t*)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, SCRACH_BUFFER_SIZE, 32);

    memset(score_buffer,0,MAX_OUT_SCORE_SIZE*sizeof(float));

    Vps_printf("MTC inner buffer addr is:%x,%x,%x,%x,%x!\n",scrach_buffer,temp_buffer,image_buffer,templ_q_buffer,score_buffer);
}

void match_template_classic_free_buffer()
{
    Int32 status = SYSTEM_LINK_STATUS_SOK;

    status = Utils_memFree(
                UTILS_HEAPID_L2_LOCAL,
                temp_buffer,
                FIX_TEMPL_WIDTH*MAX_TEMPL_HIGHT);
                
    UTILS_assert(SYSTEM_LINK_STATUS_SOK == status);

    status = Utils_memFree(
                UTILS_HEAPID_L2_LOCAL,
                image_buffer,
                MAX_CUR_IMG_SIZE);
                
    UTILS_assert(SYSTEM_LINK_STATUS_SOK == status);

        status = Utils_memFree(
                UTILS_HEAPID_L2_LOCAL,
                templ_q_buffer,
                FIX_TEMPL_WIDTH*MAX_TEMPL_HIGHT*sizeof(int16_t));
                
    UTILS_assert(SYSTEM_LINK_STATUS_SOK == status);

        status = Utils_memFree(
                UTILS_HEAPID_L2_LOCAL,
                score_buffer,
                MAX_OUT_SCORE_SIZE*sizeof(float));
                
    UTILS_assert(SYSTEM_LINK_STATUS_SOK == status);

    status = Utils_memFree(
                UTILS_HEAPID_L2_LOCAL,
                scrach_buffer,
                SCRACH_BUFFER_SIZE);

    // status = Utils_memFree(
    //             UTILS_HEAPID_DDR_CACHED_SR,
    //             scrach_buffer,
    //             SCRACH_BUFFER_SIZE);
                
    UTILS_assert(SYSTEM_LINK_STATUS_SOK == status);

    Vps_printf("MTC inner buffer free!\n");
}

int32_t match_template_classic_dsp(MtInParam* param, MtOutResult* out_buf)
{
  int32_t i;
  int32_t templ_w,templ_h,img_w,img_h,score_w,score_h,max_x,max_y;
  int32_t ret = 0;

  float scaledTempImgVar = 1.0;
  uint16_t temp_mean_q2;
  float scale_x;
  float scale_y;

  VXLIB_F32 warpMatrix[WARP_MATRIX_NUM];
  VXLIB_bufParams2D_t src_param;
  VXLIB_bufParams2D_t dst_param;

  Cache_inv(param, sizeof(MtInParam), Cache_Type_ALL, TRUE);

  //输入参数检查
  ret = mt_classic_para_check(param,out_buf);
  if ((ret != 0) && (ret != ERR_CODE_1))
  {
    Vps_printf("MT CLASSIC Error:paracheck error!Code:%d!\n",ret);
    memset(out_buf,0,sizeof(MtOutResult));
    return ret;
  }

  //两张图像的地址需要在A15上就给转成物理地址传过来
  Cache_inv(param->pre_image, ORIGIN_IMG_WIDTH*ORIGIN_IMG_HEIGHT, Cache_Type_ALL, TRUE);
  Cache_inv(param->cur_image, ORIGIN_IMG_WIDTH*ORIGIN_IMG_HEIGHT, Cache_Type_ALL, TRUE);

  //将检测框强转为int型，和X86保持一致
  Rect_Int rect_int;
  Rect_Int rect_search_int;

  for (i = 0; i < param->pad_num; i++)
  {
    //根据尺寸计算模板的仿射变换矩阵  
    //缩放比例
    rect_int.left = (int)param->templ_pad[i].left;
    rect_int.right = (int)param->templ_pad[i].right;
    rect_int.top = (int)param->templ_pad[i].top;
    rect_int.bottom = (int)param->templ_pad[i].bottom;
  
    scale_x = ((float)rect_int.right-rect_int.left)/FIX_TEMPL_WIDTH;
    templ_w = FIX_TEMPL_WIDTH;  
    if ((rect_int.bottom - rect_int.top)/scale_x > 64.0)
    {
        scale_y = ((float)rect_int.bottom - rect_int.top)/MAX_TEMPL_HIGHT;
    }
    else
    {
        scale_y = scale_x;
    }

    templ_h = (int32_t)(0.5 + (rect_int.bottom - rect_int.top) / scale_y);

    //Vps_printf("in templ rect is:%d,%d,%d,%d!out w-h is:%d,%d!\n",rect_int.left,rect_int.top,rect_int.right,rect_int.bottom,templ_w,templ_h);

    if (templ_h*templ_w > FIX_TEMPL_WIDTH*MAX_TEMPL_HIGHT)
    {
      //对于这种情况，直接用原始的框的位置，不进行模板匹配
      out_buf->result[i].new_rect.left = (float)rect_int.left;
      out_buf->result[i].new_rect.top = (float)rect_int.top;
      out_buf->result[i].new_rect.right = (float)rect_int.right;
      out_buf->result[i].new_rect.bottom = (float)rect_int.bottom;      
      out_buf->result[i].value = 1.0;
      Vps_printf("********DSP:MTC: Unexpect template oversize happen!\n");
      continue;
    }

    warpMatrix[0] = (VXLIB_F32)scale_x;
    warpMatrix[2] = 0.0;
    warpMatrix[4] = (VXLIB_F32)(rect_int.left);
    warpMatrix[1] = 0.0;
    warpMatrix[3] = (VXLIB_F32)scale_y;
    warpMatrix[5] = (VXLIB_F32)(rect_int.top);

    //配置参数src_param，dst_param
    src_param.data_type = VXLIB_UINT8;
    src_param.dim_x = ORIGIN_IMG_WIDTH;
    src_param.dim_y = ORIGIN_IMG_HEIGHT;
    src_param.stride_y = ORIGIN_IMG_WIDTH;

    dst_param.data_type = VXLIB_UINT8;
    dst_param.dim_x = templ_w;
    dst_param.dim_y = templ_h;
    dst_param.stride_y = templ_w;

    //Vps_printf("DSP:MTC: start first wa,buffer:%x,%x,scale:%f,offset:%f,%f!!\n",param->pre_image,temp_buffer,warpMatrix[0],warpMatrix[4],warpMatrix[5]);
    //仿射变换，结果存放在temp_buffer中VXLIB_warpAffineBilinear_i8u_c32f_o8u(param->pre_image,&src_param,temp_buffer,dst_param,warpMatrix，0,0,0,0);
    VXLIB_warpAffineNearest_i8u_c32f_o8u(param->pre_image,&src_param,temp_buffer,&dst_param,warpMatrix,0,0,0,0);
    //Vps_printf("DSP:MTC: end first wa！\n");

    rect_search_int.left = (int)param->cur_pad[i].left;
    rect_search_int.right = (int)param->cur_pad[i].right;
    rect_search_int.top = (int)param->cur_pad[i].top;
    rect_search_int.bottom = (int)param->cur_pad[i].bottom;

    //根据尺寸计算搜索图的仿射变换矩阵
    if (rect_search_int.top < 0)
    {
        rect_search_int.top = 0;
    }
    
    if (rect_search_int.bottom > ORIGIN_IMG_HEIGHT)
    {
        rect_search_int.bottom = ORIGIN_IMG_HEIGHT;
    }
    
    //缩放比例
    img_w = (int32_t)(0.5 + (rect_search_int.right - rect_search_int.left) / scale_x);
    img_h = (int32_t)(0.5 + (rect_search_int.bottom - rect_search_int.top) / scale_y);

#if 1
    //将搜索图的宽度扩展到4的整数倍,考虑边界保护，需要补上黑边
    img_w = ((img_w+CUR_IMG_WIDTH_ALIGN-1) /CUR_IMG_WIDTH_ALIGN)*CUR_IMG_WIDTH_ALIGN;
    if ((img_w*scale_x + rect_search_int.left) >= ORIGIN_IMG_WIDTH)
    {
      //img_w = img_w - CUR_IMG_WIDTH_ALIGN;
      Vps_printf("********DSP:MTC: image width out of range happen!\n");
    }
#endif

    //Vps_printf("in search rect is:%d,%d,%d,%d!out w-h is:%d,%d!\n",rect_search_int.left,rect_search_int.top,rect_search_int.right,rect_search_int.bottom,img_w,img_h);

    if (img_w*img_h > MAX_CUR_IMG_SIZE)
    {
      //对于这种情况，直接用原始的框的位置，不进行模板匹配
      out_buf->result[i].new_rect.left = (float)rect_int.left;
      out_buf->result[i].new_rect.top = (float)rect_int.top;
      out_buf->result[i].new_rect.right = (float)rect_int.right;
      out_buf->result[i].new_rect.bottom = (float)rect_int.bottom;      
      out_buf->result[i].value = 1.0;
      Vps_printf("********DSP:MTC: Unexpect image oversize happen!\n");
      continue;
    }

    warpMatrix[0] = (VXLIB_F32)scale_x;
    warpMatrix[2] = 0.0;
    warpMatrix[4] = (VXLIB_F32)(rect_search_int.left);
    warpMatrix[1] = 0.0;
    warpMatrix[3] = (VXLIB_F32)scale_y;
    warpMatrix[5] = (VXLIB_F32)(rect_search_int.top);

    //配置参数src_param，dst_param
    src_param.data_type = VXLIB_UINT8;
    src_param.dim_x = ORIGIN_IMG_WIDTH;
    src_param.dim_y = ORIGIN_IMG_HEIGHT;
    src_param.stride_y = ORIGIN_IMG_WIDTH;

    dst_param.data_type = VXLIB_UINT8;
    dst_param.dim_x = img_w;
    dst_param.dim_y = img_h;
    dst_param.stride_y = img_w;

    //Vps_printf("DSP:MTC: start second wa,buffer:%x,%x,scale:%f,offset:%f,%f!!\n",param->cur_image,image_buffer,warpMatrix[0],warpMatrix[4],warpMatrix[5]);
    //仿射变换，结果存放在image_buffer中VXLIB_warpAffineBilinear_i8u_c32f_o8u(param->cur_image,&src_param,image_buffer,dst_param,warpMatrix，0,0,0,0);
    VXLIB_warpAffineNearest_i8u_c32f_o8u(param->cur_image,&src_param,image_buffer,&dst_param,warpMatrix,0,0,0,0);
    //Vps_printf("DSP:MTC: end second wa！\n");

    //将模板扩展为SQ13.2,计算模板的scaled var
    scaledTempImgVar = GetNormedTemplate(temp_buffer,templ_h,templ_w,templ_q_buffer,&temp_mean_q2);

    //Vps_printf("Temp var is:%f!\n",scaledTempImgVar);

    score_w = img_w - templ_w + 1;
    score_h = img_h - templ_h + 1;

   //Vps_printf("DSP:MTC start!\n");

#if 0
    //模板匹配进行搜索
    ret = VLIB_matchTemplate_temp(image_buffer,
                                    img_w,
                                    img_h,
                                    img_w,
                                    templ_q_buffer,
                                    templ_w,
                                    templ_h,
                                    templ_w,
                                    scaledTempImgVar,
                                    1,
                                    1,
                                    0,
                                    score_w,
                                    score_buffer,
                                    scrach_buffer);    

    //找出最大值和所在的点
    getMaxLoc(score_buffer,score_w,score_h,&max_x, &max_y,&out_buf->result[i].value);
#endif
#if 1
    ret = VLIB_matchTemplate_mine(image_buffer,
                                    img_w,
                                    img_h,
                                    img_w,
                                    templ_q_buffer,
                                    templ_w,
                                    templ_h,
                                    templ_w,
                                    scaledTempImgVar,
                                    temp_mean_q2,
                                    &max_x,
                                    &max_y,
                                    &out_buf->result[i].value,
                                    score_w,
                                    score_buffer,
                                    scrach_buffer);

    score_h = score_h;
#endif

    //Vps_printf("DSP:MTC end!\n");
  
    //还原到原始图像上的X,Y
    out_buf->result[i].new_rect.left = rect_search_int.left + (int)(max_x*scale_x);
    out_buf->result[i].new_rect.top = rect_search_int.top + (int)(max_y*scale_y);
    out_buf->result[i].new_rect.right = out_buf->result[i].new_rect.left + rect_int.right - rect_int.left;
    out_buf->result[i].new_rect.bottom = out_buf->result[i].new_rect.top + rect_int.bottom - rect_int.top;

#if 0
    Vps_printf("num:%d!max x-y is:%d,%d!max rect is:%f,%f,%f,%f!max value is:%f!\n",i,max_x,max_y,
                out_buf->result[i].new_rect.left,out_buf->result[i].new_rect.top,
                out_buf->result[i].new_rect.right,out_buf->result[i].new_rect.bottom,
                out_buf->result[i].value); 
#endif 
  }

  out_buf->result_num = param->pad_num;
  
  return ret;
}

CODE_SECTION(VLIB_matchTemplate_temp, ".text:optimized")
int32_t VLIB_matchTemplate_temp(uint8_t img[restrict],
                           int32_t imgWidth,
                           int32_t imgHeight,
                           int32_t imgPitch,
                           int16_t tempImg[restrict],
                           int32_t tempImgWidth,
                           int32_t tempImgHeight,
                           int32_t tempImgPitch,
                           VLIB_F32 scaledTempImgVar,
                           uint8_t xDirJump,
                           uint8_t yDirJump,
                           int32_t method,
                           int32_t outScorePitch,
                           VLIB_F32 outScore[restrict],
                           uint8_t scratch[restrict])
{
    int16_t    outScoreWidth    = imgWidth  - (tempImgWidth - 1);
    int16_t    outScoreHeight   = imgHeight - (tempImgHeight - 1);

    int32_t     i, j, l, k;
    int32_t     normCurWinSq, normCurTempMul;
    VLIB_F32    rcpTempArea;
    VLIB_F32    rcp4;
    VLIB_F32    normCurWinSqF, normCurTempMulF;
    int32_t     curWinAvg;

    uint16_t *restrict    pu16CurWinAvg  = (uint16_t * )scratch; /* Size of outScoreWidth*outScoreHeight */
    uint32_t              totalOutputPlus3 = (outScoreWidth * outScoreHeight) + 3;
    uint32_t              totalOutputPlus1 = (outScoreWidth * outScoreHeight) + 1;
    uint32_t              totalOutputAlign4= totalOutputPlus3 & 0xFFFFFFFCU;
    uint32_t              totalOutputAlign2= totalOutputPlus1 & 0xFFFFFFFEU;

    uint16_t *restrict    pu16CurWinHSum = (uint16_t * )scratch + totalOutputAlign4; /* Size of outScoreWidth*imgHeight   */
    uint32_t *restrict    pu32CurSqSum   = (uint32_t * )scratch + totalOutputAlign4; /* Size of outScoreWidth*outScoreHeight */
    uint32_t *restrict    pu32CurTempSum = pu32CurSqSum + totalOutputAlign2; /* Size of outScoreWidth*outScoreHeight */

    uint32_t    outputWidthPlus3 = outScoreWidth + 3;
    uint32_t    outputWidthAlign4= outputWidthPlus3 & 0xFFFFFFFCU;

    uint8_t  *restrict    pu8TempPtr;
    uint8_t  *restrict    pu8TempPtr1;
    uint8_t  *restrict    pu8TempPtr2;
    uint16_t *restrict    pu16TempPtr;
    uint16_t *restrict    pu16TempPtr1;
    uint16_t *restrict    pu16TempPtr2;
    uint16_t *restrict    pu16CurWinHSumOrig = pu16CurWinHSum;
    uint16_t *restrict    pu16CurWinAvgOrig  = pu16CurWinAvg;
    uint8_t  *restrict    pu8temp1;
    int16_t  *restrict    ps16temp1;
    uint64_t              u64temp1, u64temp2, u64temp3;

    uint32_t    u32temp1, u32temp2, u32temp3, u32temp4;
    uint32_t    retVal;
    VLIB_F32    f32temp1;
    __x128_t    u128temp1, u128temp2;
    uint64_t    curWinAvgPkd;
    unsigned int start,end;
    TSCL = 0;

    if((tempImgWidth < 4) || (tempImgHeight < 4) || (img == NULL) ||
       (tempImg == NULL) || (scratch == NULL) || (imgPitch < imgWidth) ||
       (tempImgPitch < tempImgWidth) || (method != 0) ||
       (tempImgWidth > 128) || (tempImgHeight > 128) ||
       (outScorePitch < outScoreWidth)) {

        retVal = 1;

    } else {
        start = TSCL;

        rcpTempArea = _rcpsp(tempImgHeight * tempImgWidth);
        rcp4      = _rcpsp(4.0f);

        /*Sliding Horizontal sum of original image pixels. Size of the horzontal
          summed data will be outScoreWidth*imgHeight
         */
        for( l = 0; l < imgHeight; l++ ) {

            pu8TempPtr     = &img[(l * imgPitch) + 0];
            pu16CurWinHSum = &pu16CurWinHSumOrig[l * outScoreWidth];
            u32temp1       = 0;

            /*First four element of each row*/
            for( i = 0; i < tempImgWidth; i++ ) {
                u32temp1 += pu8TempPtr[i];
            }

            *pu16CurWinHSum = u32temp1;
            pu16CurWinHSum++;

            pu8TempPtr1 = &img[(l * imgPitch) + 0];
            pu8TempPtr2 = &img[(l * imgPitch) + tempImgWidth];

            /*4th element to last four byte aligned location*/
            for( k = 1; k < outScoreWidth; k++ ) {

                u32temp1        = (u32temp1 - pu8TempPtr1[0]) + pu8TempPtr2[0];
                *pu16CurWinHSum = u32temp1;

                pu16CurWinHSum++;
                pu8TempPtr1++;
                pu8TempPtr2++;
            }
        }

        /* Vertical sum of the horizontal summed data */

        for( l = 0; l < outputWidthAlign4; l += 4 ) {

            pu16TempPtr   = &pu16CurWinHSumOrig[l];
            pu16CurWinAvg = &pu16CurWinAvgOrig[l];
            u64temp1      = 0;

            u32temp1   = 0;
            u32temp2   = 0;
            u32temp3   = 0;
            u32temp4   = 0;

            /*First four element of each row*/
            for( i = 0; i < tempImgHeight; i++ ) {
                u64temp1       = _mem8_const(pu16TempPtr);
                u32temp1      += (_loll(u64temp1) & 0x0000FFFFU);
                u32temp2      += (_loll(u64temp1) >> 0x10U);
                u32temp3      += (_hill(u64temp1) & 0x0000FFFFU);
                u32temp4      += (_hill(u64temp1) >> 0x10U);
                pu16TempPtr   += outScoreWidth;
            }

            f32temp1       = rcpTempArea * ((VLIB_F32)u32temp1 * 4.0f);
            *pu16CurWinAvg  = (uint16_t)f32temp1;
            pu16CurWinAvg++;

            f32temp1       = rcpTempArea * ((VLIB_F32)u32temp2 * 4.0f);
            *pu16CurWinAvg  = (uint16_t)f32temp1;
            pu16CurWinAvg++;

            f32temp1       = rcpTempArea * ((VLIB_F32)u32temp3 * 4.0f);
            *pu16CurWinAvg  = (uint16_t)f32temp1;
            pu16CurWinAvg++;

            f32temp1       = rcpTempArea * ((VLIB_F32)u32temp4 * 4.0f);
            *pu16CurWinAvg  = (uint16_t)f32temp1;
            pu16CurWinAvg++;

            pu16CurWinAvg += (outScoreWidth - 4);

            pu16TempPtr1 = &pu16CurWinHSumOrig[l + (0 * outScoreWidth)];
            pu16TempPtr2 = &pu16CurWinHSumOrig[l + (tempImgHeight * outScoreWidth)];

            /*4th element to last four byte aligned location*/
            for( k = 1; k < outScoreHeight; k++ ) {

                u64temp1 = _mem8_const(pu16TempPtr1);
                u64temp2 = _mem8_const(pu16TempPtr2);

                u32temp1-= _loll(u64temp1) & 0x0000FFFFU;
                u32temp1+= (_loll(u64temp2) & 0x0000FFFFU);

                u32temp2-= _loll(u64temp1) >> 0x10U;
                u32temp2+= (_loll(u64temp2) >> 0x10U);

                u32temp3-= _hill(u64temp1) & 0x0000FFFFU;
                u32temp3+= (_hill(u64temp2) & 0x0000FFFFU);

                u32temp4-= _hill(u64temp1) >> 0x10U;
                u32temp4+= (_hill(u64temp2) >> 0x10U);

                f32temp1       = rcpTempArea * ((VLIB_F32)u32temp1 * 4.0f);
                *pu16CurWinAvg  = (uint16_t)f32temp1;
                pu16CurWinAvg++;

                f32temp1       = rcpTempArea * ((VLIB_F32)u32temp2 * 4.0f);
                *pu16CurWinAvg  = (uint16_t)f32temp1;
                pu16CurWinAvg++;

                f32temp1       = rcpTempArea * ((VLIB_F32)u32temp3 * 4.0f);
                *pu16CurWinAvg  = (uint16_t)f32temp1;
                pu16CurWinAvg++;

                f32temp1       = rcpTempArea * ((VLIB_F32)u32temp4 * 4.0f);
                *pu16CurWinAvg  = (uint16_t)f32temp1;
                pu16CurWinAvg++;

                pu16TempPtr1   += outScoreWidth;
                pu16TempPtr2   += outScoreWidth;
                pu16CurWinAvg  +=(outScoreWidth - 4);
            }
        }

        pu16CurWinAvg = (uint16_t * )scratch;

        end = TSCL;
        Vps_printf("last time 1 is:%d!\n",(end-start));

        start = TSCL;

        /*Finding of
         a) mean subtracted squared current pixels
         b) mean subtracted current*template image
         */

        /* Two seperate loop has been implemented to avoid shift operations in core loop.
           (img[((l+i)*imgPitch) + (j+k)]*4 - curWinAvg) * tempImg[i*tempImgPitch + j] is
           Q2 * Q2 signed multiplication which will result into  4 fraction bit. Bits remaining for integer
           magnitude is 31 - 4 = 27 bits excluding one bit for sign. 8 + 8 = 16 bit is required for
           real magnitude after one accumulation, as both multiplication operands absolute value is
           within 8 bit. Hence 2^(27-16) = 2048 elements can be accumulated to fit in 32 bit.
        */

        if((tempImgWidth == 32) && (((uint32_t)tempImg & 0x7U) == 0x0) &&
                  ((tempImgPitch & 0x3U) == 0x0) && (((uint32_t)img & 0x3U) == NULL) &&
                  ((imgPitch & 0x3U) == NULL)) {
            for( l = 0; l < outScoreHeight; l+=yDirJump ) {
                for( k = 0; k < outScoreWidth; k+=xDirJump ) {
                    normCurWinSq   = 0;
                    normCurTempMul = 0;
                    /*Q2 format*/
                    curWinAvg      = pu16CurWinAvg[(l * outScoreWidth) + k];
                    u32temp1       = (curWinAvg << 0x10U) | curWinAvg;
                    curWinAvgPkd   = _itoll(u32temp1, u32temp1);

                    for( i = 0; i < tempImgHeight; i++ ) {
                        pu8temp1  = &img[((l + i) * imgPitch) + (0 + k)];
                        ps16temp1 = &tempImg[(i * tempImgPitch) + 0];
          #pragma UNROLL(8)

                        for( j = 0; j < 32; j += 4 ) {
                            /*Current pixels*/
                            u32temp1  = _mem4_const(pu8temp1);
                            pu8temp1 += 4;
                            /*Template pixels*/
                            u64temp1  = _amem8_const(ps16temp1);
                            ps16temp1+= 4;
                            /*Current pixels, changed to Q2*/
                            u64temp2  = _dshl2(_unpkbu4(u32temp1), (uint32_t)0x2U);
                            /*Mean subtracted current pixels*/
                            u64temp2  = _dsub2(u64temp2, curWinAvgPkd);
                            /*Mean subtracted squared current pixels*/
                            u128temp1 = _dmpy2(u64temp2, u64temp2);
                            u64temp3  = _dadd(_lo128(u128temp1), _hi128(u128temp1));
                            normCurWinSq += (_loll(u64temp3) + _hill(u64temp3));
                            /*Mean subtracted current pixels * template pixel*/
                            u128temp2 = _dmpy2(u64temp2, u64temp1);
                            u64temp3  = _dadd(_lo128(u128temp2), _hi128(u128temp2));
                            normCurTempMul += (_loll(u64temp3) + _hill(u64temp3));
                        }
                    }

                    normCurWinSq   = normCurWinSq   >> 0x2U;
                    normCurTempMul = normCurTempMul >> 0x2U;
                    *pu32CurSqSum   = normCurWinSq;
                    *pu32CurTempSum = normCurTempMul;
                    pu32CurSqSum++;
                    pu32CurTempSum++;

                }
            }
        }
        else
        {
          Vps_printf("not match!\n");
        }

        end = TSCL;
        Vps_printf("last time 2 is:%d!\n",(end-start));

        start = TSCL;

        pu32CurSqSum   = (uint32_t * )scratch + totalOutputAlign4; /* Size of outScoreWidth*outScoreHeight */
        pu32CurTempSum = pu32CurSqSum + totalOutputAlign2; /* Size of outScoreWidth*outScoreHeight */

        for( l = 0; l < outScoreHeight; l+=yDirJump ) {
            for( k = 0; k < outScoreWidth; k+=xDirJump ) {

                normCurWinSq   = *pu32CurSqSum;
                pu32CurSqSum++;
                normCurTempMul = *pu32CurTempSum;
                pu32CurTempSum++;

                normCurWinSqF   = normCurWinSq * rcp4;
                normCurTempMulF = normCurTempMul * rcp4;

                if( normCurWinSqF > FLT_MIN ) {
                    f32temp1 = (normCurTempMulF * scaledTempImgVar) * _rsqrsp(normCurWinSqF);
                } else {
                    f32temp1 = FLT_MAX;
                }

                outScore[(l * outScorePitch) + k] = f32temp1;

            }
        }

        retVal = 0;
    }

    end = TSCL;

    Vps_printf("last time 3 is:%d!\n",(end-start));
    return (retVal);
}
