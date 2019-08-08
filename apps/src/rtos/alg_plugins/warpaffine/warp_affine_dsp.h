#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define WARP_AFFINE_TYPE_BILINEAR        (0)
#define WARP_AFFINE_TYPE_NEAREST         (1)

#define WARP_AFFINE_OUT_BUF_WIDTH                  (112)
#define WARP_AFFINE_OUT_BUF_HEIGHT                 (112)
#define WARP_AFFINE_OUT_BUF_CHANNEL                (3)
#define WARP_AFFINE_OUT_BUF_NUMBER                 (20)
#define WARP_AFFINE_SINGLE_OUT_BUF_SIZE            (WARP_AFFINE_OUT_BUF_WIDTH * \
                                                    WARP_AFFINE_OUT_BUF_HEIGHT * \
                                                    WARP_AFFINE_OUT_BUF_CHANNEL)
#define WARP_AFFINE_OUT_BUF_SIZE                   (WARP_AFFINE_OUT_BUF_WIDTH * \
                                                    WARP_AFFINE_OUT_BUF_HEIGHT * \
                                                    WARP_AFFINE_OUT_BUF_CHANNEL * \
                                                    WARP_AFFINE_OUT_BUF_NUMBER)

#define WARP_AFFINE_PRM_SIZE                       (524)

int32_t warp_affine_dsp(uint8_t *in_buf, uint8_t *out_buf);

