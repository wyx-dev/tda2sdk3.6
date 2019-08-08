/******************************************************************************
*
* Copyright (C) 2019-2020 Momenta Incorporated - https://www.momenta.ai/
*
* ALL RIGHTS RESERVED
*
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <unistd.h>

#include "hal_warp_affine.h"

#define MOMENTA_WA_IMAGE_DATA              "/mnt/WA/IN.BIN"

#define MOMENTA_MMCSD_MOUNT_PT             "mnt"

#define MOMENTA_WA_PRM_SIZE                (sizeof(HAL_WA_WarpMatrix))

unsigned int width;
unsigned int height;
unsigned int channel;
unsigned int pitch;
unsigned int number;
unsigned int data_size;
unsigned int img_size;

hal_warp_affine_handle_t wa_handle[HAL_WA_PROC_ID_MAX] = {0};
static char *wa_data = NULL;
static char *img_data = NULL;

static void system_mount()
{
    char cmd[128U];

    sprintf(cmd, "mount /dev/disk/by-path/platform-4809c000.mmc-part1 /%s",\
                                                            MOMENTA_MMCSD_MOUNT_PT);

    /* Mount the SD card */
    system(cmd);
}

static void system_unmount()
{
    char cmd[128U];

    sprintf(cmd, "umount /%s",MOMENTA_MMCSD_MOUNT_PT);

    /* Unmount the SD card */
    system(cmd);
}

static int read_input()
{
    FILE *fp = NULL;
    int ret = 0;
    int i = 0;
    char *data = NULL;
    unsigned int read_size = 0;
    HAL_WA_WarpMatrix matrix = {0};

    ret = hal_warp_affine_malloc_continuous_aligned((void**)&wa_data, data_size, 32);
    assert(HAL_WA_OK == ret);
    ret = hal_warp_affine_malloc_continuous_aligned((void**)&img_data, img_size, 32);
    assert(HAL_WA_OK == ret);

    printf("wa ******addr is:%x,%x!\n",(unsigned int)wa_data,(unsigned int)img_data);
    data = wa_data;

    // x0 = x * warpMatrix[0] + y * warpMatrix[2] + warpMatrix[4];
    // y0 = x * warpMatrix[1] + y * warpMatrix[3] + warpMatrix[5];

    // dst(x,y) = BilinearInterpolate(src(x0, y0))

    // Warp Affine Type
    // 
    // HAL_WA_TYPE_BILINEAR
    // HAL_WA_TYPE_NEAREST

    // shearing
    //matrix.type = HAL_WA_TYPE_BILINEAR;
    //matrix.channel = 1;
    //matrix.number = 1;
    //matrix.src_dim_x = 1280;
    //matrix.src_dim_y = 720;
    //matrix.src_stride_y = 1280;
    //matrix.dst_dim_x = 320;
    //matrix.dst_dim_y = 180;
    //matrix.dst_stride_y = 1280;
    //matrix.warpMatrix[0][0] = 1.0f;
    //matrix.warpMatrix[0][1] = 0.0f;
    //matrix.warpMatrix[0][2] = 0.0f;
    //matrix.warpMatrix[0][3] = 1.0f;
    //matrix.warpMatrix[0][4] = 0.0f;
    //matrix.warpMatrix[0][5] = 0.0f;
    //matrix.srcOffsetX = 0;
    //matrix.srcOffsetY = 0;
    //matrix.dstOffsetX = 0;
    //matrix.dstOffsetY = 0;

    // shearing + shifting
    //matrix.type = HAL_WA_TYPE_BILINEAR;
    //matrix.channel = 1;
    //matrix.number = 1;
    //matrix.src_dim_x = 1280;
    //matrix.src_dim_y = 720;
    //matrix.src_stride_y = 1280;
    //matrix.dst_dim_x = 640;
    //matrix.dst_dim_y = 360;
    //matrix.dst_stride_y = 1280;
    //matrix.warpMatrix[0][0] = 1.0f;
    //matrix.warpMatrix[0][1] = 0.0f;
    //matrix.warpMatrix[0][2] = 0.0f;
    //matrix.warpMatrix[0][3] = 1.0f;
    //matrix.warpMatrix[0][4] = 0.0f;
    //matrix.warpMatrix[0][5] = 0.0f;
    //matrix.srcOffsetX = 0;
    //matrix.srcOffsetY = 0;
    //matrix.dstOffsetX = -320;
    //matrix.dstOffsetY = -180;

    // scaling
    //matrix.type = HAL_WA_TYPE_BILINEAR;
    //matrix.channel = 1;
    //matrix.number = 1;
    //matrix.src_dim_x = 1280;
    //matrix.src_dim_y = 720;
    //matrix.src_stride_y = 1280;
    //matrix.dst_dim_x = 320;
    //matrix.dst_dim_y = 180;
    //matrix.dst_stride_y = 1280;
    //matrix.warpMatrix[0][0] = 4.0f;
    //matrix.warpMatrix[0][1] = 0.0f;
    //matrix.warpMatrix[0][2] = 0.0f;
    //matrix.warpMatrix[0][3] = 4.0f;
    //matrix.warpMatrix[0][4] = 0.0f;
    //matrix.warpMatrix[0][5] = 0.0f;
    //matrix.srcOffsetX = 0;
    //matrix.srcOffsetY = 0;
    //matrix.dstOffsetX = 0;
    //matrix.dstOffsetY = 0;

    // scaling + shifting
    matrix.type = HAL_WA_TYPE_BILINEAR;
    matrix.channel = 1;
    matrix.number = 1;
    matrix.src_dim_x = 1280;
    matrix.src_dim_y = 720;
    matrix.src_stride_y = 1280;
    matrix.dst_dim_x = 640;
    matrix.dst_dim_y = 360;
    matrix.dst_stride_y = 1280;
    matrix.warpMatrix[0][0] = 4.0f;
    matrix.warpMatrix[0][1] = 0.0f;
    matrix.warpMatrix[0][2] = 0.0f;
    matrix.warpMatrix[0][3] = 4.0f;
    matrix.warpMatrix[0][4] = 0.0f;
    matrix.warpMatrix[0][5] = 0.0f;
    matrix.srcOffsetX = 0;
    matrix.srcOffsetY = 0;
    matrix.dstOffsetX = -320;
    matrix.dstOffsetY = -180;

    // rotation
    //matrix.type = HAL_WA_TYPE_BILINEAR;
    //matrix.channel = 1;
    //matrix.number = 1;
    //matrix.src_dim_x = 1280;
    //matrix.src_dim_y = 720;
    //matrix.src_stride_y = 1280;
    //matrix.dst_dim_x = 640;
    //matrix.dst_dim_y = 360;
    //matrix.dst_stride_y = 1280;
    //matrix.warpMatrix[0][0] = 0.707106781f;
    //matrix.warpMatrix[0][1] = 0.707106781f;
    //matrix.warpMatrix[0][2] = 0.707106781f;
    //matrix.warpMatrix[0][3] = 0.707106781f;
    //matrix.warpMatrix[0][4] = 0.0f;
    //matrix.warpMatrix[0][5] = 0.0f;
    //matrix.srcOffsetX = 0;
    //matrix.srcOffsetY = 0;
    //matrix.dstOffsetX = 0;
    //matrix.dstOffsetY = 0;

    memcpy(data, &matrix, MOMENTA_WA_PRM_SIZE);

    ret = hal_warp_affine_cache_wb((void*)data, MOMENTA_WA_PRM_SIZE);
    assert(HAL_WA_OK == ret);

    data += MOMENTA_WA_PRM_SIZE;
    *((unsigned int*)data) = hal_warp_affine_virt2phys(img_data);

    printf("wa phy ******addr is:%x!\n",*((unsigned int*)data));

    ret = hal_warp_affine_cache_wb((void*)data, sizeof(unsigned int));
    assert(HAL_WA_OK == ret);

    fp = fopen(MOMENTA_WA_IMAGE_DATA, "rb");
    assert(NULL != fp);

    char *temp_img_data = img_data;

    for (i = 0; i < height * number; i++)
    {
        read_size = fread(img_data, 1, width * channel, fp);
        if (read_size != (width * channel))
        {
            fclose(fp);
            return -1;
        }
        ret = hal_warp_affine_cache_wb((void*)img_data, width * channel);
        assert(HAL_WA_OK == ret);
        img_data += pitch;
    }

    fclose(fp);

    img_data = temp_img_data;
    return ret;
}

static void output_handle(hal_warp_affine_handle_t handle, HAL_WA_OutputBuffer *output)
{

    static int cnt = 0;
    char name[64] = {0};
    FILE *fp = NULL;

    sprintf(name, "/mnt/WA/out_%d.bin", cnt);
    cnt++;
    fp = fopen(name, "wb");

    fwrite(output->data, 1, output->size, fp);
    fclose(fp);

    system("sync");
}

int hal_warp_affine_run(void)
{
    int ret = 0;
    HAL_WA_CreateParams create_params;

    system_mount();

    memset(&create_params, 0, sizeof(HAL_WA_CreateParams));

    width = 1280;
    height = 720;
    channel = 1;
    pitch = width * channel;
    number = 1;
    data_size = MOMENTA_WA_PRM_SIZE + sizeof(unsigned int);
    img_size = pitch * height * number;

    ret = read_input();
    assert(HAL_WA_OK == ret);

    ret = hal_warp_affine_init_process_level();
    assert(HAL_WA_OK == ret);

    create_params.handle = output_handle;
    ret = hal_warp_affine_init_thread_level(&wa_handle[HAL_WA_PROC_ID_DSP1_INST0], HAL_WA_PROC_ID_DSP1_INST0, &create_params, sizeof(HAL_WA_CreateParams));
    assert(HAL_WA_OK == ret);
    ret = hal_warp_affine_init_thread_level(&wa_handle[HAL_WA_PROC_ID_DSP2_INST0], HAL_WA_PROC_ID_DSP2_INST0, &create_params, sizeof(HAL_WA_CreateParams));
    assert(HAL_WA_OK == ret);

    hal_warp_affine_put_input_buffer(wa_handle[HAL_WA_PROC_ID_DSP1_INST0], wa_data);
    hal_warp_affine_wait_output_buffer(wa_handle[HAL_WA_PROC_ID_DSP1_INST0]);

    hal_warp_affine_put_input_buffer(wa_handle[HAL_WA_PROC_ID_DSP2_INST0], wa_data);
    hal_warp_affine_wait_output_buffer(wa_handle[HAL_WA_PROC_ID_DSP2_INST0]);

    hal_warp_affine_deinit_thread_level(wa_handle[HAL_WA_PROC_ID_DSP1_INST0]);
    hal_warp_affine_deinit_thread_level(wa_handle[HAL_WA_PROC_ID_DSP2_INST0]);

    hal_warp_affine_deinit_process_level();

    ret = hal_warp_affine_free_continuous_aligned((void*)wa_data, data_size);
    assert(HAL_WA_OK == ret);
    ret = hal_warp_affine_free_continuous_aligned((void*)img_data, img_size);
    assert(HAL_WA_OK == ret);



    system_unmount();

    return ret;
}
