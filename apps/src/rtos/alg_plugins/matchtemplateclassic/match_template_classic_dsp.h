#ifndef MATCH_TEMPLATE_CLASSIC_DSP_H
#define MATCH_TEMPLATE_CLASSIC_DSP_H

#include <stdint.h>

#define MAX_RECT_NUM         (6)
#define FIX_TEMPL_WIDTH      (32)
#define ORIGIN_IMG_WIDTH     (1280)
#define ORIGIN_IMG_HEIGHT    (720)

#define MAX_TEMPL_HIGHT    (64)
#define MAX_CUR_TEMPL_SIZE_RATIO    (9)
#define CUR_IMG_WIDTH_ALIGN    (4)

#define MAX_CUR_IMG_SIZE    (FIX_TEMPL_WIDTH*MAX_TEMPL_HIGHT*MAX_CUR_TEMPL_SIZE_RATIO)
#define MAX_OUT_SCORE_SIZE  (FIX_TEMPL_WIDTH*2+1)*(MAX_TEMPL_HIGHT*2+1)
#define SCRACH_BUFFER_SIZE  (MAX_OUT_SCORE_SIZE*20+40)
#define TOTAL_L2_BUFFER    (MAX_CUR_IMG_SIZE + MAX_OUT_SCORE_SIZE*sizeof(float) + FIX_TEMPL_WIDTH * MAX_TEMPL_HIGHT * 3)

typedef enum
{
    ERR_CODE_1 = -1,
    ERR_CODE_2 = -2,
    ERR_CODE_3 = -3,
    ERR_CODE_4 = -4,
    ERR_CODE_5 = -5,
    ERR_CODE_6 = -6,
    PARA_CHECK_OK = 0,

    ERR_CODE_NUM = 7
}ErrorCode;

typedef struct
{
  float left;
  float top;
  float right;
  float bottom;
}Rect;


typedef struct
{
  int left;
  int top;
  int right;
  int bottom;
}Rect_Int;

typedef struct
{
  uint8_t* pre_image;
  uint8_t* cur_image;
  int pad_num;
  Rect templ_pad[MAX_RECT_NUM];
  Rect cur_pad[MAX_RECT_NUM];
}MtInParam;

typedef struct
{
  Rect new_rect;
  float value;
}MatchResult;

typedef struct
{
  int result_num;
  MatchResult result[MAX_RECT_NUM];
}MtOutResult;

int32_t match_template_classic_dsp(MtInParam* param, MtOutResult* out_buf);

void match_template_classic_malloc_buffer();
void match_template_classic_free_buffer();

#endif



