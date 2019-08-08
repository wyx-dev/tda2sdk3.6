#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

int32_t MT_match_template_dsp(uint8_t img[],
                              uint32_t imgWidth,
                              uint32_t imgHeight,
                              uint32_t imgPitch,
                              uint8_t tempImgU08[],
                              int16_t tempImgI16[],
                              uint32_t tempImgWidth,
                              uint32_t tempImgHeight,
                              uint32_t tempImgPitch,
                              int32_t result[]);

int32_t MT_crop_image(uint8_t src[],
                      uint32_t srcWidth,
                      uint32_t srcHeight,
                      uint32_t srcPitch,
                      uint8_t dst[],
                      uint32_t dstWidth,
                      uint32_t dstHeight,
                      uint32_t dstPitch,
                      uint32_t uStart,
                      uint32_t vStart);

int32_t MT_resize_image_h4(uint8_t src[],
                           uint32_t srcWidth,
                           uint32_t srcHeight,
                           uint32_t srcPitch,
                           uint8_t dst[],
                           uint32_t dstWidth,
                           uint32_t dstHeight,
                           uint32_t dstPitch);
