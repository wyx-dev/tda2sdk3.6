#ifndef MATCH_TEMPLATE_CLASSIC_DSP_H
#define MATCH_TEMPLATE_CLASSIC_DSP_H

#include <stdint.h>
#define HAL_MTC_OK              (0)
#define HAL_MTC_EFAIL           (-1)
#define HAL_MTC_ETIMEOUT        (-2)
#define HAL_MTC_EALLOC          (-3)
#define HAL_MTC_EBUSY           (-4)
#define HAL_MTC_EINVALID_PARAMS (-5)

#define MAX_RECT_NUM         (6)
#define ORIGIN_IMG_WIDTH     (1280)
#define ORIGIN_IMG_HEIGHT    (720)

#define MAX_TEMPL_SHORT_EDGE      (48)
#define MAX_TEMPL_LONG_EDGE       (64)
#define MAX_SEARCH_SIZE_RATIO     (9)
#define SEARCH_IMG_WIDTH_ALIGN    (4)

#define MAX_SEARCH_IMG_SIZE    (MAX_TEMPL_LONG_EDGE*MAX_TEMPL_SHORT_EDGE*MAX_SEARCH_SIZE_RATIO)
#define MAX_OUT_SCORE_SIZE  (MAX_TEMPL_LONG_EDGE*2+1)*(MAX_TEMPL_SHORT_EDGE*2+1)
#define SCRACH_BUFFER_SIZE  (MAX_OUT_SCORE_SIZE*20+40)
#define TOTAL_L2_BUFFER    (MAX_SEARCH_IMG_SIZE + MAX_OUT_SCORE_SIZE*sizeof(float) + MAX_TEMPL_LONG_EDGE * MAX_TEMPL_SHORT_EDGE * 3)


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

typedef enum
{
    MODE_W32H64_JUMP = 0x0,
    MODE_W48H48_JUMP = 0x1,
    MODE_W64H48_JUMP = 0x2,

    MODE_W32H64_NOJUMP = 0x10,
    MODE_W48H48_NOJUMP = 0x11,
    MODE_W64H48_NOJUMP = 0x12
}MatchMode;

typedef struct
{
  int left;
  int top;
  int right;
  int bottom;
}Rect;

typedef struct
{
  uint8_t* pre_image;
  uint8_t* cur_image;
  int pad_num;
  Rect templ_pad[MAX_RECT_NUM];
  Rect cur_pad[MAX_RECT_NUM];
  int match_mode[MAX_RECT_NUM];
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



