#include "match_template_dsp.h"
#include "vlib.h"
#include "math.h"
#include "src/rtos/utils_common/include/utils_mem.h"

static VLIB_F32 *outScore;
static uint8_t *scratch1;
static float * raw_scores;

void match_template_malloc_buffer(uint32_t outScorePitch, uint32_t outScoreHeight, uint32_t imgHeight)
{
    outScore = (VLIB_F32 *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, outScorePitch * outScoreHeight*sizeof(VLIB_F32), 32);
    scratch1 = (uint8_t *)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, ((outScorePitch * (outScoreHeight * 10)) + 20) * sizeof(uint16_t), 32);
    raw_scores = (float*)Utils_memAlloc(UTILS_HEAPID_DDR_CACHED_SR, imgHeight * sizeof(float), 32);
    Vps_printf("MT buffer addr is:%x,%x,%x!\n",outScore,scratch1,raw_scores);
}

void match_template_free_buffer(uint32_t outScorePitch, uint32_t outScoreHeight, uint32_t imgHeight)
{
    Int32 status = SYSTEM_LINK_STATUS_SOK;

    status = Utils_memFree(
                UTILS_HEAPID_DDR_CACHED_SR,
                outScore,
                outScorePitch * outScoreHeight * sizeof(VLIB_F32));
                
    UTILS_assert(SYSTEM_LINK_STATUS_SOK == status);

    status = Utils_memFree(
                UTILS_HEAPID_DDR_CACHED_SR,
                scratch1,
                ((outScorePitch * (outScoreHeight * 10)) + 20) * sizeof(uint16_t));
                
    UTILS_assert(SYSTEM_LINK_STATUS_SOK == status);

        status = Utils_memFree(
                UTILS_HEAPID_DDR_CACHED_SR,
                raw_scores,
                imgHeight * sizeof(float));

    Vps_printf("MT buffer free!\n");
}

float calc_variance(float *score_buffer, int width, int height, int dst_x) {
    int i;
    double mean = 0.0;
    double variance = 0.0;

    if (height < 1)
        return 0.0;

    for (i = 0; i < height; i++) {
        raw_scores[i] = score_buffer[i*width + dst_x];
    }

    for (i = 0; i < height; i++)
        mean += raw_scores[i];

    mean = mean/height;

    for (i = 0; i < height; i++) {
        variance += (raw_scores[i] - mean) * (raw_scores[i] - mean);
    }

    variance = sqrt(variance / height);

    return (float)variance;
}

extern int32_t VLIB_matchTemplate_mmt(uint8_t img[restrict],
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
                           uint8_t scratch[restrict]);

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
    VLIB_F32 out_variance;

	// output ncc scores
	int32_t outScoreWidth = imgWidth - tempImgWidth + 1;
	int32_t outScoreHeight = imgHeight - tempImgHeight + 1;
	int32_t outScorePitch = outScoreWidth;

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
				tempImgI16[(i * tempImgPitch) + j] = ((int32_t)tempImgU08[(i * tempImgPitch) + j] * 4) - tempImgAvg;
				tempImgVar += (tempImgI16[(i * tempImgPitch) + j] * tempImgI16[(i * tempImgPitch) + j]);
			}
		}
		VLIB_F32 tempImgVarF = (1.0f / sqrt(((VLIB_F32)tempImgVar) / (4.0 * 4.0)));

		if( tempImgVar <= FLT_MIN ) {
			tempImgVarF = FLT_MAX;
		}

        Vps_printf("remote matchTemplate begin\n");
		// call VLIB_matchTemplate
		int32_t status = VLIB_matchTemplate_mmt(img,
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
        Vps_printf("Remote matchTemplate status: %u\n", status);

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

    out_variance = calc_variance(outScore,outScorePitch,outScoreHeight,maxLocX);
    //Vps_printf("Remote MTC:tw:%d,th:%d,w:%d,h:%d,var:%f!\n",tempImgWidth,tempImgHeight,imgWidth,imgHeight,out_variance);
	// output
    if (maxScore > 0) {
        result[0] = maxLocX;
        result[1] = maxLocY;
        result[2] = (int32_t)(maxScore * 1024);
        result[3] = (int32_t)(out_variance * 1024);
        return 0;
    }

    result[0] = 0;
    result[1] = 0;
    result[2] = 0;
    result[3] = 0;
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



int32_t MT_resize_image_h8(uint8_t src[],
                           uint32_t srcWidth,
                           uint32_t srcHeight,
                           uint32_t srcPitch,
                           uint8_t dst[],
                           uint32_t dstWidth,
                           uint32_t dstHeight,
                           uint32_t dstPitch)
{
    if (srcWidth != dstWidth*8 ||  srcHeight != dstHeight) {
        printf("Error: wrong image size!\n");
        return 1;
    }
    uint32_t i, j;
    for (i=0; i<dstHeight; i++) {
        for (j=0; j<dstWidth; j++) {
            dst[i*dstPitch+j] = src[i*srcPitch+j*8];
        }
    }
    return 0;
}
