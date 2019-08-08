#include "match_template_dsp.h"
#include "vlib.h"
#include "math.h"
#include "src/rtos/utils_common/include/utils_mem.h"

int32_t MT_match_template_dsp(uint8_t img[],
                              uint32_t imgWidth,
                              uint32_t imgHeight,
                              uint32_t imgPitch,
                              uint8_t tempImgU08[],
                              int16_t tempImgI16[],
                              uint32_t tempImgWidth,
                              uint32_t tempImgHeight,
                              uint32_t tempImgPitch,
                              int32_t result[])
{
	VLIB_F32 maxScore = 0;
    int32_t maxLocX = 0;
    int32_t maxLocY = 0;

	// output ncc scores
	int32_t outScoreWidth = imgWidth - tempImgWidth + 1;
	int32_t outScoreHeight = imgHeight - tempImgHeight + 1;
	int32_t outScorePitch = outScoreWidth;

	// Allocate buffers
    VLIB_F32 *outScore = (VLIB_F32 *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, outScorePitch*outScoreHeight*sizeof(VLIB_F32), 32);
    uint8_t *scratch1 = (uint8_t *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, ((outScoreWidth * (outScoreHeight * 10)) + 20) * sizeof(uint16_t), 32);

	// only run if the buffer allocations fit in the heap
	if (outScore && scratch1) {

		// calculate scaledTempImgVar needed by VLIB_matchTemplate
		// mean
		int32_t i, j, tempImgAvg=0;
		for (i=0; i<tempImgHeight; i++) {
			for (j=0; j<tempImgWidth; j++) {
				tempImgAvg += tempImgU08[tempImgPitch*i + j];
			}
		}
		tempImgAvg = (tempImgAvg * 4) / (tempImgWidth * tempImgHeight);

		// variance
		int32_t tempImgVar = 0;
		for( i = 0; i < tempImgHeight; i++ ) {
			for( j = 0; j < tempImgWidth; j++ ) {
				tempImgI16[(i * tempImgPitch) + j] = (tempImgU08[(i * tempImgPitch) + j] * 4) - tempImgAvg;
				tempImgVar += ((tempImgU08[(i * tempImgPitch) + j] * 4 - tempImgAvg) * (tempImgU08[(i * tempImgPitch) + j] * 4 - tempImgAvg));
			}
		}
		VLIB_F32 tempImgVarF = (1.0f / sqrt(((VLIB_F32)tempImgVar) / (4.0 * 4.0)));

		if( tempImgVar <= FLT_MIN ) {
			tempImgVarF = FLT_MAX;
		}

        //printf("VLIB_matchTemplate begin\n");
		// call VLIB_matchTemplate
		int32_t status = VLIB_matchTemplate(img,
		                                    imgWidth,
		                                    imgHeight,
		                                    imgPitch,
		                                    tempImgI16,
		                                    tempImgWidth,
		                                    tempImgHeight,
		                                    tempImgPitch,
		                                    tempImgVarF,
		                                    1,
		                                    1,
		                                    0x0,
		                                    outScorePitch,
		                                    outScore,
		                                    scratch1);
        //printf("VLIB_matchTemplate status: %u\n", status);

		if (status != 1) {
			// find the location of the maximal element in outScore
			for (i=0; i<outScoreHeight; i++) {
				for (j=0; j<outScoreWidth; j++) {
					if (outScore[outScorePitch*i +j] > maxScore) {
						maxScore = outScore[outScorePitch*i +j];
						maxLocX = j;
						maxLocY = i;
					}
				}
			}
		}
	}

	// free buffers
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, outScore, outScorePitch*outScoreHeight*sizeof(VLIB_F32));
    Utils_memFree(UTILS_HEAPID_DDR_CACHED_SR, scratch1, ((outScoreWidth * (outScoreHeight * 10)) + 20) * sizeof(uint16_t));

	// output
    if (maxScore > 0) {
        result[0] = maxLocX;
        result[1] = maxLocY;
        result[2] = (int32_t)(maxScore * 1024);
        return 0;
    }

    result[0] = 0;
    result[1] = 0;
    result[2] = 0;
	return 1;

}



int32_t MT_crop_image(uint8_t src[],
                      uint32_t srcWidth,
                      uint32_t srcHeight,
                      uint32_t srcPitch,
                      uint8_t dst[],
                      uint32_t dstWidth,
                      uint32_t dstHeight,
                      uint32_t dstPitch,
                      uint32_t uStart,
                      uint32_t vStart)
{
    if ((uStart+dstWidth)>srcWidth || (vStart+dstHeight)>srcHeight) {
        printf("Error: wrong image size\n");
        return 1;
    }

    uint32_t i, j;
    for (i=0; i<dstHeight; i++) {
        for (j=0; j<dstWidth; j++) {
            dst[i*dstPitch+j] = src[(i+vStart)*srcPitch + (j+uStart)];
        }
    }

    return 0;
}



int32_t MT_resize_image_h4(uint8_t src[],
                           uint32_t srcWidth,
                           uint32_t srcHeight,
                           uint32_t srcPitch,
                           uint8_t dst[],
                           uint32_t dstWidth,
                           uint32_t dstHeight,
                           uint32_t dstPitch)
{
    if (srcWidth != dstWidth*4 ||  srcHeight != dstHeight) {
        printf("Error: wrong image size!\n");
        return 1;
    }
    uint32_t i, j;
    for (i=0; i<dstHeight; i++) {
        for (j=0; j<dstWidth; j++) {
            dst[i*dstPitch+j] = src[i*srcPitch+j*4];
        }
    }
    return 0;
}
