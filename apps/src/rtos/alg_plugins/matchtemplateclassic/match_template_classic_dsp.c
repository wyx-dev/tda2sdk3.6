#include "match_template_classic_dsp.h"
#include "math.h"

#define ON_DSP

#ifdef ON_DSP
#include "vlib.h"
#include "vxlib.h"
#include "src/rtos/utils_common/include/utils_mem.h"

#else
//为了在PC上DEBUG，自己定义相关函数
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"


#define UTILS_HEAPID_DDR_CACHED_SR    0

typedef float VXLIB_F32;

typedef struct VXLIB_bufParams2D_t
{
  int h;
  int w;
}VXLIB_bufParams2D_t;


void* Utils_memAlloc(int32_t mode, int32_t size, int32_t allign)
{
    void * result = malloc(size);
    return result;
}

//使用opencv的仿射变换模拟
void VXLIB_warpAffineBilinear_i8u_c32f_o8u(uint8_t* src, VXLIB_bufParams2D_t* src_para, 
                                            uint8_t* dst, VXLIB_bufParams2D_t* dst_para,
                                            float* warp_matrix, int32_t a, int32_t b, int32_t c, int32_t d)
{
    cv::Mat input = cv::Mat(src_para->h, src_para->w, CV_8UC1, src);
    cv::Mat output;
    cv::Size out_size = cv::Size(dst_para->w,dst_para->h);

    cv::Mat matrix = cv::Mat(2,3,CV_32FC1,warp_matrix);

    cv::warpAffine(input,output,matrix,out_size);

    memcpy(dst,output.data, dst_para->w*dst_para->h);

    return;
}

int32_t VLIB_matchTemplate(uint8_t* i_buffer, int32_t i_w, int32_t i_h, int32_t i_pitch, 
                              int16_t* t_buffer, int32_t t_w, int32_t t_h, int32_t t_pitch,
                              float temp_var, int32_t jump_x, int32_t jump_y, int32_t mode,
                              int32_t score_w, float* score_buffer, uint8_t* scrach)
{
    cv::Mat input = cv::Mat(i_h, i_w, CV_8UC1, i_buffer);
    cv::Mat templ = cv::Mat(t_h, t_w, CV_8UC1);
    cv::Mat result_score;

    int16_t* p_templ_16 = t_buffer;

    //将输入的int16 模板转成uchar
    for (int i = 0; i < t_h; i++)
    {
        for (int j = 0; j < t_w; j++)
        {
            templ.at<uchar>(i,j) = (uchar)((*p_templ_16)>>2);
            p_templ_16++;
        }
    }
    
    //模板匹配
    matchTemplate(input,templ,result_score,CV_TM_CCOEFF_NORMED);

    memcpy((void*)score_buffer,(void*)result_score.data, sizeof(float)*score_w*(i_h-t_h+1));

    cv::imshow("ori img",input);

    cv::imshow("temp img",templ);

    cv::imshow("score",result_score);

    cvWaitKey(1);

    return 0;
}

#endif

#define WARP_MATRIX_NUM     (6)
#define TEMPL_Q_SCALE       (4)

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
    float element;
    float temp_mean;
    float sum = 0.0;
    float result;

    temp_mean = GetImageMean(tmp,height,width);

    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            element = ((float)tmp[i*width+j]-temp_mean);
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
  int templ_h,templ_w,cur_h,cur_w;

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
    param->templ_pad[i].bottom = (param->templ_pad[i].bottom >= ORIGIN_IMG_HEIGHT)?(ORIGIN_IMG_HEIGHT-1):param->templ_pad[i].bottom;
    param->templ_pad[i].left = (param->templ_pad[i].left < 0)?0:param->templ_pad[i].left;
    param->templ_pad[i].right = (param->templ_pad[i].right >= ORIGIN_IMG_WIDTH)?(ORIGIN_IMG_WIDTH-1):param->templ_pad[i].right;

    param->cur_pad[i].top = (param->cur_pad[i].top < 0)?0:param->cur_pad[i].top;
    param->cur_pad[i].bottom = (param->cur_pad[i].bottom >= ORIGIN_IMG_HEIGHT)?(ORIGIN_IMG_HEIGHT-1):param->cur_pad[i].bottom;
    param->cur_pad[i].left = (param->cur_pad[i].left < 0)?0:param->cur_pad[i].left;
    param->cur_pad[i].right = (param->cur_pad[i].right >= ORIGIN_IMG_WIDTH)?(ORIGIN_IMG_WIDTH-1):param->cur_pad[i].right;
    
    //出于优化考虑，由于内部缓存有限，对于尺寸过大的模板或原图，直接拒绝
    templ_h = param->templ_pad[i].bottom - param->templ_pad[i].top + 1;
    templ_w = param->templ_pad[i].right - param->templ_pad[i].left + 1;

    cur_h = param->cur_pad[i].bottom - param->cur_pad[i].top + 1;
    cur_w = param->cur_pad[i].right - param->cur_pad[i].left + 1;

    //固定将宽度resize为32，如果高度超过宽度的两倍，会导致resize以后高度高于64，拒绝
    if (templ_h > templ_w*(MAX_TEMPL_HIGHT/FIX_TEMPL_WIDTH))
    {
      return ERR_CODE_4;
    }
    
    //如果要搜索的图高和宽的乘积超过模板面积的15倍，拒绝
    if (cur_h*cur_w > MAX_CUR_TEMPL_SIZE_RATIO*templ_h*templ_w)
    {
      return ERR_CODE_5;
    }
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
    scrach_buffer = (uint8_t*)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, SCRACH_BUFFER_SIZE, 32);

    temp_buffer = (uint8_t*)Utils_memAlloc(UTILS_HEAPID_L2_LOCAL, FIX_TEMPL_WIDTH*MAX_TEMPL_HIGHT, 32);
    image_buffer = (uint8_t*)Utils_memAlloc(UTILS_HEAPID_L2_LOCAL, MAX_CUR_IMG_SIZE, 32);
    templ_q_buffer = (int16_t*)Utils_memAlloc(UTILS_HEAPID_L2_LOCAL, FIX_TEMPL_WIDTH*MAX_TEMPL_HIGHT*sizeof(int16_t), 32);
    score_buffer = (float*)Utils_memAlloc(UTILS_HEAPID_L2_LOCAL, MAX_OUT_SCORE_SIZE*sizeof(float), 32);

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
                UTILS_HEAPID_DDR_CACHED_SR,
                scrach_buffer,
                SCRACH_BUFFER_SIZE);

    UTILS_assert(SYSTEM_LINK_STATUS_SOK == status);

    Vps_printf("MTC inner buffer free!\n");
}

int32_t match_template_classic_dsp(MtInParam* param, MtOutResult* out_buf)
{
  int32_t i,j;
  int32_t templ_w,templ_h,img_w,img_h,score_w,score_h,max_x,max_y;
  int32_t ret = 0;

  float scaledTempImgVar = 1.0;

  float scale;
  VXLIB_F32 warpMatrix[WARP_MATRIX_NUM];
  VXLIB_bufParams2D_t src_param;
  VXLIB_bufParams2D_t dst_param;

  Cache_inv(param, sizeof(MtInParam), Cache_Type_ALL, TRUE);

  //输入参数检查
  ret = mt_classic_para_check(param,out_buf);
  if (ret != 0)
  {
    Vps_printf("MT CLASSIC Error:paracheck error!Code:%d!\n",ret);
    memset(out_buf,0,sizeof(MtOutResult));
    return ret;
  }

  //两张图像的地址需要在A15上就给转成物理地址传过来
  Cache_inv(param->pre_image, ORIGIN_IMG_WIDTH*ORIGIN_IMG_HEIGHT, Cache_Type_ALL, TRUE);
  Cache_inv(param->cur_image, ORIGIN_IMG_WIDTH*ORIGIN_IMG_HEIGHT, Cache_Type_ALL, TRUE);

  for (i = 0; i < param->pad_num; i++)
  {
    //根据尺寸计算模板的仿射变换矩阵  
    //缩放比例
#ifdef ON_DSP    
    scale = (float)(param->templ_pad[i].right-param->templ_pad[i].left + 1)/FIX_TEMPL_WIDTH;
#else
    scale = FIX_TEMPL_WIDTH/(float)(param->templ_pad[i].right-param->templ_pad[i].left);
#endif

    templ_w = FIX_TEMPL_WIDTH;
    templ_h = (int32_t)((param->templ_pad[i].bottom - param->templ_pad[i].top + 1) / scale);

    //受限于申请的内存空间，模板的面积不得高于2048.由于已经做过参数检查，应该不会发生
    UTILS_assert(templ_h*templ_w <= FIX_TEMPL_WIDTH*MAX_TEMPL_HIGHT);

#ifdef ON_DSP
    warpMatrix[0] = (VXLIB_F32)scale;
    warpMatrix[2] = 0.0;
    warpMatrix[4] = (VXLIB_F32)(param->templ_pad[i].left);
    warpMatrix[1] = 0.0;
    warpMatrix[3] = (VXLIB_F32)scale;
    warpMatrix[5] = (VXLIB_F32)(param->templ_pad[i].top);
#else
    warpMatrix[0] = (VXLIB_F32)scale;
    warpMatrix[1] = 0.0;
    warpMatrix[2] = (VXLIB_F32)(0.0-scale*(param->templ_pad[i].left));
    warpMatrix[3] = 0.0;
    warpMatrix[4] = (VXLIB_F32)scale;
    warpMatrix[5] = (VXLIB_F32)(0.0-scale*(param->templ_pad[i].top));
#endif

    //TODO:配置参数src_param，dst_param
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
    VXLIB_warpAffineBilinear_i8u_c32f_o8u(param->pre_image,&src_param,temp_buffer,&dst_param,warpMatrix,0,0,0,0);
    //Vps_printf("DSP:MTC: end first wa！\n");

    //根据尺寸计算搜索图的仿射变换矩阵
    if (param->cur_pad[i].top < 0)
    {
        param->cur_pad[i].top = 0;
    }
    
    if (param->cur_pad[i].bottom > ORIGIN_IMG_HEIGHT - 1)
    {
        param->cur_pad[i].bottom = ORIGIN_IMG_HEIGHT - 1;
    }
    
    //缩放比例
    img_w = (int32_t)((param->cur_pad[i].right - param->cur_pad[i].left + 1) / scale);
    img_h = (int32_t)((param->cur_pad[i].bottom - param->cur_pad[i].top + 1) / scale);

    //将宽度扩展到4的整数倍
    img_w = ((img_w+CUR_IMG_WIDTH_ALIGN-1) /CUR_IMG_WIDTH_ALIGN)*CUR_IMG_WIDTH_ALIGN;

    //由于已经做过参数检查，应该不会发生
    UTILS_assert(img_w*img_h <= MAX_CUR_IMG_SIZE);

#ifdef ON_DSP
    warpMatrix[0] = (VXLIB_F32)scale;
    warpMatrix[2] = 0.0;
    warpMatrix[4] = (VXLIB_F32)(param->cur_pad[i].left);
    warpMatrix[1] = 0.0;
    warpMatrix[3] = (VXLIB_F32)scale;
    warpMatrix[5] = (VXLIB_F32)(param->cur_pad[i].top);
#else
    warpMatrix[0] = (VXLIB_F32)scale;
    warpMatrix[1] = 0.0;
    warpMatrix[2] = (VXLIB_F32)(0.0 - scale*(param->cur_pad[i].left));
    warpMatrix[3] = 0.0;
    warpMatrix[4] = (VXLIB_F32)scale;
    warpMatrix[5] = (VXLIB_F32)(0.0 - scale*param->cur_pad[i].top);
#endif

    //TODO:配置参数src_param，dst_param
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
    VXLIB_warpAffineBilinear_i8u_c32f_o8u(param->cur_image,&src_param,image_buffer,&dst_param,warpMatrix,0,0,0,0);
    //Vps_printf("DSP:MTC: end second wa！\n");

    //Todo:计算模板的scaled var
    scaledTempImgVar = GetTemplateVar(temp_buffer,templ_h,templ_w);

#if 0
    Vps_printf("var is:%f!data is:%d,%d,%d,%d,%d!\n",scaledTempImgVar,param->pre_image[0],param->pre_image[1],
        param->pre_image[10],param->pre_image[100],param->pre_image[1000]);
#endif

    //将模板扩展为SQ13.2
    for(j = 0;j < templ_h*templ_w; j++)
    {
        templ_q_buffer[j] = temp_buffer[j]*TEMPL_Q_SCALE;
    }

    score_w = img_w - templ_w + 1;
    score_h = img_h - templ_h + 1;

    //Vps_printf("DSP:MTC start!\n");

    //模板匹配进行搜索
    ret = VLIB_matchTemplate(image_buffer,
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

    //Vps_printf("DSP:MTC end!\n");
               
    //找出最大值和所在的点
    getMaxLoc(score_buffer,score_w,score_h,&max_x, &max_y,&out_buf->result[i].value);

    //还原到原始图像上的X,Y
    out_buf->result[i].new_rect.left = param->cur_pad[i].left + (float)max_x*scale;
    out_buf->result[i].new_rect.top = param->cur_pad[i].top + (float)max_y*scale;
    out_buf->result[i].new_rect.right = out_buf->result[i].new_rect.left + param->templ_pad[i].right - param->templ_pad[i].left;
    out_buf->result[i].new_rect.bottom = out_buf->result[i].new_rect.top + param->templ_pad[i].bottom - param->templ_pad[i].top;

#if 0
    Vps_printf("num:%d!max rect is:%f,%f,%f,%f!max value is:%f!\n",i,
                out_buf->result[i].new_rect.left,out_buf->result[i].new_rect.top,
                out_buf->result[i].new_rect.right,out_buf->result[i].new_rect.bottom,
                out_buf->result[i].value); 
#endif 
  }

  out_buf->result_num = param->pad_num;
  
  return ret;
}
