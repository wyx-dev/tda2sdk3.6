/******************************************************************************
*
* Copyright (C) 2018-2019 Momenta Incorporated - https://www.momenta.ai/
*
* ALL RIGHTS RESERVED
*
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include "hal_match_template.h"

#define MOMENTA_MT_IMAGE_DATA_0            "/mnt/MT/IN_0.BIN"
#define MOMENTA_MT_IMAGE_DATA_1            "/mnt/MT/IN_1.BIN"

#define MOMENTA_MMCSD_MOUNT_PT             "mnt"

#define MOMENTA_MT_PRM_SIZE                16

unsigned int width;
unsigned int height;
unsigned int channel;
unsigned int pitch;
unsigned int number;
unsigned int data_size;

hal_match_template_handle_t mt_handle[HAL_MT_PROC_ID_MAX] = {0};
static char *mt_data[2] = {NULL};

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

    ret = hal_match_template_malloc_continuous_aligned((void**)&mt_data[0], data_size, 32);
    assert(HAL_MT_OK == ret);
    ret = hal_match_template_malloc_continuous_aligned((void**)&mt_data[1], data_size, 32);
    assert(HAL_MT_OK == ret);
    
    fp = fopen(MOMENTA_MT_IMAGE_DATA_0, "rb");
    assert(NULL != fp);

    data = mt_data[0];
    for (i = 0; i < height * number; i++)
    {
        read_size = fread(data, 1, width * channel, fp);
        if (read_size != (width * channel))
        {
            fclose(fp);
            return -1;
        }
        ret = hal_match_template_cache_wb((void*)data, width * channel);
        assert(HAL_MT_OK == ret);
        data += pitch;
    }

    read_size = fread(data, 1, MOMENTA_MT_PRM_SIZE, fp);
    if (read_size != MOMENTA_MT_PRM_SIZE)
    {
        fclose(fp);
        return -1;
    }
    ret = hal_match_template_cache_wb((void*)data, MOMENTA_MT_PRM_SIZE);
    assert(HAL_MT_OK == ret);

    fclose(fp);

    fp = fopen(MOMENTA_MT_IMAGE_DATA_1, "rb");
    assert(NULL != fp);

    data = mt_data[1];
    for (i = 0; i < height * number; i++)
    {
        read_size = fread(data, 1, width * channel, fp);
        if (read_size != (width * channel))
        {
            fclose(fp);
            return -1;
        }
        ret = hal_match_template_cache_wb((void*)data, width * channel);
        assert(HAL_MT_OK == ret);
        data += pitch;
    }

    read_size = fread(data, 1, MOMENTA_MT_PRM_SIZE, fp);
    if (read_size != MOMENTA_MT_PRM_SIZE)
    {
        fclose(fp);
        return -1;
    }
    ret = hal_match_template_cache_wb((void*)data, MOMENTA_MT_PRM_SIZE);
    assert(HAL_MT_OK == ret);

    fclose(fp);

    return ret;
}

static void output_handle(hal_match_template_handle_t handle, HAL_MT_OutputBuffer *output)
{
    static int cnt = 0;
    char name[64] = {0};
    FILE *fp = NULL;

    sprintf(name, "/mnt/MT/out_%d.bin", cnt);
    cnt++;
    fp = fopen(name, "wb");

    fwrite(output->data, 1, output->size, fp);
    fclose(fp);

    system("sync");
}

int hal_match_template_run(void)
{
    int ret = 0;
    HAL_MT_CreateParams create_params;

    system_mount();

    memset(&create_params, 0, sizeof(HAL_MT_CreateParams));

    width = 1280;
    height = 720;
    channel = 1;
    pitch = width * channel;
    number = 1;
    data_size = pitch * height * number + MOMENTA_MT_PRM_SIZE;

    ret = read_input();
    assert(HAL_MT_OK == ret);

    ret = hal_match_template_init_process_level();
    assert(HAL_MT_OK == ret);

    create_params.width = width;
    create_params.height = height;
    create_params.pitch = pitch;
    create_params.patch_width = 128;
    create_params.patch_height = 48;
    create_params.search_width = 140;
    create_params.search_height = 72;
    create_params.handle = output_handle;
    ret = hal_match_template_init_thread_level(&mt_handle[HAL_MT_PROC_ID_DSP1_INST0], HAL_MT_PROC_ID_DSP1_INST0, &create_params, sizeof(HAL_MT_CreateParams));
    assert(HAL_MT_OK == ret);
    ret = hal_match_template_init_thread_level(&mt_handle[HAL_MT_PROC_ID_DSP2_INST0], HAL_MT_PROC_ID_DSP2_INST0, &create_params, sizeof(HAL_MT_CreateParams));
    assert(HAL_MT_OK == ret);

    hal_match_template_put_input_buffer(mt_handle[HAL_MT_PROC_ID_DSP1_INST0], mt_data[0]);
    hal_match_template_wait_output_buffer(mt_handle[HAL_MT_PROC_ID_DSP1_INST0]);
    hal_match_template_put_input_buffer(mt_handle[HAL_MT_PROC_ID_DSP1_INST0], mt_data[1]);
    hal_match_template_wait_output_buffer(mt_handle[HAL_MT_PROC_ID_DSP1_INST0]);

    hal_match_template_put_input_buffer(mt_handle[HAL_MT_PROC_ID_DSP2_INST0], mt_data[0]);
    hal_match_template_wait_output_buffer(mt_handle[HAL_MT_PROC_ID_DSP2_INST0]);
    hal_match_template_put_input_buffer(mt_handle[HAL_MT_PROC_ID_DSP2_INST0], mt_data[1]);
    hal_match_template_wait_output_buffer(mt_handle[HAL_MT_PROC_ID_DSP2_INST0]);

    hal_match_template_deinit_thread_level(mt_handle[HAL_MT_PROC_ID_DSP1_INST0]);
    hal_match_template_deinit_thread_level(mt_handle[HAL_MT_PROC_ID_DSP2_INST0]);

    hal_match_template_deinit_process_level();

    ret = hal_match_template_free_continuous_aligned((void*)mt_data[0], data_size);
    assert(HAL_MT_OK == ret);
    ret = hal_match_template_free_continuous_aligned((void*)mt_data[1], data_size);
    assert(HAL_MT_OK == ret);

    system_unmount();

    return ret;
}
