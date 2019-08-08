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
#include "hal_sparse_optical_flow.h"

#define MOMENTA_SOF_IMAGE_DATA_0           "/mnt/SOF/IN_0.BIN"
#define MOMENTA_SOF_IMAGE_DATA_1           "/mnt/SOF/IN_1.BIN"

#define MOMENTA_MMCSD_MOUNT_PT             "mnt"

#define MOMENTA_SOF_PRM_SIZE               20

unsigned int width;
unsigned int height;
unsigned int channel;
unsigned int pitch;
unsigned int number;
unsigned int data_size;

hal_sparse_optical_flow_handle_t sof_handle[HAL_SOF_PROC_ID_MAX] = {0};
static char *sof_data[2] = {NULL};

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

    ret = hal_sparse_optical_flow_malloc_continuous_aligned((void**)&sof_data[0], data_size, 32);
    assert(HAL_SOF_OK == ret);
    ret = hal_sparse_optical_flow_malloc_continuous_aligned((void**)&sof_data[1], data_size, 32);
    assert(HAL_SOF_OK == ret);
    
    fp = fopen(MOMENTA_SOF_IMAGE_DATA_0, "rb");
    assert(NULL != fp);

    data = sof_data[0];
    for (i = 0; i < height * number; i++)
    {
        read_size = fread(data, 1, width * channel, fp);
        if (read_size != (width * channel))
        {
            fclose(fp);
            return -1;
        }
        ret = hal_sparse_optical_flow_cache_wb((void*)data, width * channel);
        assert(HAL_SOF_OK == ret);
        data += pitch;
    }

    read_size = fread(data, 1, MOMENTA_SOF_PRM_SIZE, fp);
    if (read_size != MOMENTA_SOF_PRM_SIZE)
    {
        fclose(fp);
        return -1;
    }
    ret = hal_sparse_optical_flow_cache_wb((void*)data, MOMENTA_SOF_PRM_SIZE);
    assert(HAL_SOF_OK == ret);

    fp = fopen(MOMENTA_SOF_IMAGE_DATA_1, "rb");
    assert(NULL != fp);

    data = sof_data[1];
    for (i = 0; i < height * number; i++)
    {
        read_size = fread(data, 1, width * channel, fp);
        if (read_size != (width * channel))
        {
            fclose(fp);
            return -1;
        }
        ret = hal_sparse_optical_flow_cache_wb((void*)data, width * channel);
        assert(HAL_SOF_OK == ret);
        data += pitch;
    }

    read_size = fread(data, 1, MOMENTA_SOF_PRM_SIZE, fp);
    if (read_size != MOMENTA_SOF_PRM_SIZE)
    {
        fclose(fp);
        return -1;
    }
    ret = hal_sparse_optical_flow_cache_wb((void*)data, MOMENTA_SOF_PRM_SIZE);
    assert(HAL_SOF_OK == ret);

    fclose(fp);

    return ret;
}

static void output_handle(hal_sparse_optical_flow_handle_t handle, HAL_SOF_OutputBuffer *output)
{
    static int cnt = 0;
    char name[64] = {0};
    FILE *fp = NULL;

    sprintf(name, "/mnt/SOF/out_%d.bin", cnt);
    cnt++;
    fp = fopen(name, "wb");

    fwrite(output->data, 1, output->size, fp);
    fclose(fp);

    system("sync");
}

int hal_sparse_optical_flow_run(void)
{
    int ret = 0;
    HAL_SOF_CreateParams create_params;

    system_mount();

    memset(&create_params, 0, sizeof(HAL_SOF_CreateParams));

    width = 1920;
    height = 1080;
    channel = 1;
    pitch = width * channel;
    number = 1;
    data_size = pitch * height * number + MOMENTA_SOF_PRM_SIZE;

    ret = read_input();
    assert(HAL_SOF_OK == ret);

    ret = hal_sparse_optical_flow_init_process_level();
    assert(HAL_SOF_OK == ret);

    create_params.startX = 0;
    create_params.startY = 0;
    create_params.width = width;
    create_params.height = height;
    create_params.handle = output_handle;
    ret = hal_sparse_optical_flow_init_thread_level(&sof_handle[HAL_SOF_PROC_ID_DSP1_INST0], HAL_SOF_PROC_ID_DSP1_INST0, &create_params, sizeof(HAL_SOF_CreateParams));
    assert(HAL_SOF_OK == ret);
    ret = hal_sparse_optical_flow_init_thread_level(&sof_handle[HAL_SOF_PROC_ID_DSP2_INST0], HAL_SOF_PROC_ID_DSP2_INST0, &create_params, sizeof(HAL_SOF_CreateParams));
    assert(HAL_SOF_OK == ret);

    hal_sparse_optical_flow_put_input_buffer(sof_handle[HAL_SOF_PROC_ID_DSP1_INST0], sof_data[0]);
    hal_sparse_optical_flow_wait_output_buffer(sof_handle[HAL_SOF_PROC_ID_DSP1_INST0]);
    hal_sparse_optical_flow_put_input_buffer(sof_handle[HAL_SOF_PROC_ID_DSP1_INST0], sof_data[1]);
    hal_sparse_optical_flow_wait_output_buffer(sof_handle[HAL_SOF_PROC_ID_DSP1_INST0]);

    hal_sparse_optical_flow_put_input_buffer(sof_handle[HAL_SOF_PROC_ID_DSP2_INST0], sof_data[0]);
    hal_sparse_optical_flow_wait_output_buffer(sof_handle[HAL_SOF_PROC_ID_DSP2_INST0]);
    hal_sparse_optical_flow_put_input_buffer(sof_handle[HAL_SOF_PROC_ID_DSP2_INST0], sof_data[1]);
    hal_sparse_optical_flow_wait_output_buffer(sof_handle[HAL_SOF_PROC_ID_DSP2_INST0]);

    hal_sparse_optical_flow_deinit_thread_level(sof_handle[HAL_SOF_PROC_ID_DSP1_INST0]);
    hal_sparse_optical_flow_deinit_thread_level(sof_handle[HAL_SOF_PROC_ID_DSP2_INST0]);

    hal_sparse_optical_flow_deinit_process_level();

    ret = hal_sparse_optical_flow_free_continuous_aligned((void*)sof_data[0], data_size);
    assert(HAL_SOF_OK == ret);
    ret = hal_sparse_optical_flow_free_continuous_aligned((void*)sof_data[1], data_size);
    assert(HAL_SOF_OK == ret);

    system_unmount();

    return ret;
}
