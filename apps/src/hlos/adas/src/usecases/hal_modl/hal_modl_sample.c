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
#include "hal_modl.h"

#define MOMENTA_TIDL_NET_BIN               "/mnt/TDA2x/NET.BIN"
#define MOMENTA_TIDL_PRM_BIN               "/mnt/TDA2x/PRM.BIN"
#define MOMENTA_TIDL_IMAGE_DATA            "/mnt/TDA2x/IN.RGB"

#define MOMENTA_MMCSD_MOUNT_PT             "mnt"

#define MOMENTA_TIDL_PRM_SIZE              64

unsigned int width;
unsigned int height;
unsigned int channel;
unsigned int pitch;
unsigned int number;
unsigned int dataSize;

hal_modl_handle_t handle[MODL_PROC_ID_MAX] = {0};
static char *tidl_net = NULL;
unsigned int tidl_net_size = 0;
static char *tidl_param = NULL;
unsigned int tidl_param_size = 0;
static char *tidl_data = NULL;

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

static int read_net()
{
    FILE *fp = NULL;
    int ret = 0;
    unsigned int size = 0;

    fp = fopen(MOMENTA_TIDL_NET_BIN, "rb");
    assert(NULL != fp);

    fseek(fp, 0L, SEEK_END);
    tidl_net_size = ftell(fp);
    rewind(fp);

    if(0 != tidl_net_size)
    {
        ret = hal_modl_malloc_continuous_aligned((void**)&tidl_net, tidl_net_size, 32);
        assert(HAL_MODL_OK == ret);
    
        size = fread(tidl_net, 1, tidl_net_size, fp);
        assert(size == tidl_net_size);
    }

    if(NULL != fp)
        fclose(fp);

    return ret;
}

static int read_param()
{
    FILE *fp = NULL;
    int ret = 0;
    unsigned int size = 0;

    fp = fopen(MOMENTA_TIDL_PRM_BIN, "rb");
    assert(NULL != fp);

    fseek(fp, 0L, SEEK_END);
    tidl_param_size = ftell(fp);
    rewind(fp);

    if(0 != tidl_param_size)
    {
        ret = hal_modl_malloc_continuous_aligned((void**)&tidl_param, tidl_param_size, 32);
        assert(HAL_MODL_OK == ret);

        size = fread(tidl_param, 1, tidl_param_size, fp);
        assert(size == tidl_param_size);
    }

    if(NULL != fp)
        fclose(fp);

    return ret;
}

static int read_input()
{
    FILE *fp = NULL;
    int ret = 0;
    int i = 0;
    char *data = NULL;
    unsigned int readSize = 0;

    ret = hal_modl_malloc_continuous_aligned((void**)&tidl_data, dataSize, 32);
    assert(HAL_MODL_OK == ret);
    
    fp = fopen(MOMENTA_TIDL_IMAGE_DATA, "rb");
    assert(NULL != fp);

    data = tidl_data;
    for (i = 0; i < height * number; i++)
    {
        readSize = fread(data, 1, width * channel, fp);
        if (readSize != (width * channel))
        {
            fclose(fp);
            return -1;
        }
        ret = hal_modl_cache_wb((void*)data, width * channel);
        assert(HAL_MODL_OK == ret);
        data += pitch;
    }

    readSize = fread(data, 1, MOMENTA_TIDL_PRM_SIZE, fp);
    if (readSize != MOMENTA_TIDL_PRM_SIZE)
    {
        fclose(fp);
        return -1;
    }
    ret = hal_modl_cache_wb((void*)data, MOMENTA_TIDL_PRM_SIZE);
    assert(HAL_MODL_OK == ret);

    fclose(fp);

    return ret;
}

static void output_handle(hal_modl_handle_t handle, HAL_MODL_OutputBuffer *output, void *user_arg)
{
    static int cnt = 0;
    char name[64] = {0};
    FILE *fp = NULL;

    sprintf(name, "/mnt/TDA2x/out_%d.bin", cnt);
    cnt++;
    fp = fopen(name, "wb");

    fwrite(output->frame, 1, output->size, fp);
    fclose(fp);

    system("sync");
}

int hal_modl_run(void)
{
    int i = 0;
    int ret = 0;
    HAL_MODL_Network modlNet;

    system_mount();

    memset(&modlNet, 0, sizeof(HAL_MODL_Network));

    width = 456;
    height = 456;
    channel = 3;
    pitch = width * channel;
    number = 2;
    dataSize = pitch * height * number + MOMENTA_TIDL_PRM_SIZE;

    ret = read_net();
    assert(HAL_MODL_OK == ret);

    ret = read_param();
    assert(HAL_MODL_OK == ret);

    ret = read_input();
    assert(HAL_MODL_OK == ret);

    ret = hal_modl_init_process_level();
    assert(HAL_MODL_OK == ret);

    modlNet.currCoreId = 1;
    modlNet.currLayersGroupId = 1;
    modlNet.pNetwork = tidl_net;
    modlNet.handle = output_handle;
    ret = hal_modl_create_net(&handle[MODL_PROC_ID_EVE1_INST0], MODL_PROC_ID_EVE1_INST0, &modlNet, sizeof(HAL_MODL_Network), tidl_param, tidl_param_size);
    assert(HAL_MODL_OK == ret);
    ret = hal_modl_create_net(&handle[MODL_PROC_ID_EVE1_INST1], MODL_PROC_ID_EVE1_INST1, &modlNet, sizeof(HAL_MODL_Network), tidl_param, tidl_param_size);
    assert(HAL_MODL_OK == ret);
    ret = hal_modl_create_net(&handle[MODL_PROC_ID_EVE1_INST2], MODL_PROC_ID_EVE1_INST2, &modlNet, sizeof(HAL_MODL_Network), tidl_param, tidl_param_size);
    assert(HAL_MODL_OK == ret);
    ret = hal_modl_create_net(&handle[MODL_PROC_ID_EVE1_INST3], MODL_PROC_ID_EVE1_INST3, &modlNet, sizeof(HAL_MODL_Network), tidl_param, tidl_param_size);
    assert(HAL_MODL_OK == ret);
    ret = hal_modl_create_net(&handle[MODL_PROC_ID_EVE2_INST0], MODL_PROC_ID_EVE2_INST0, &modlNet, sizeof(HAL_MODL_Network), tidl_param, tidl_param_size);
    assert(HAL_MODL_OK == ret);
    ret = hal_modl_create_net(&handle[MODL_PROC_ID_EVE2_INST1], MODL_PROC_ID_EVE2_INST1, &modlNet, sizeof(HAL_MODL_Network), tidl_param, tidl_param_size);
    assert(HAL_MODL_OK == ret);
    ret = hal_modl_create_net(&handle[MODL_PROC_ID_EVE2_INST2], MODL_PROC_ID_EVE2_INST2, &modlNet, sizeof(HAL_MODL_Network), tidl_param, tidl_param_size);
    assert(HAL_MODL_OK == ret);
    ret = hal_modl_create_net(&handle[MODL_PROC_ID_EVE2_INST3], MODL_PROC_ID_EVE2_INST3, &modlNet, sizeof(HAL_MODL_Network), tidl_param, tidl_param_size);
    assert(HAL_MODL_OK == ret);
    ret = hal_modl_create_net(&handle[MODL_PROC_ID_EVE3_INST0], MODL_PROC_ID_EVE3_INST0, &modlNet, sizeof(HAL_MODL_Network), tidl_param, tidl_param_size);
    assert(HAL_MODL_OK == ret);
    ret = hal_modl_create_net(&handle[MODL_PROC_ID_EVE3_INST1], MODL_PROC_ID_EVE3_INST1, &modlNet, sizeof(HAL_MODL_Network), tidl_param, tidl_param_size);
    assert(HAL_MODL_OK == ret);
    ret = hal_modl_create_net(&handle[MODL_PROC_ID_EVE3_INST2], MODL_PROC_ID_EVE3_INST2, &modlNet, sizeof(HAL_MODL_Network), tidl_param, tidl_param_size);
    assert(HAL_MODL_OK == ret);
    ret = hal_modl_create_net(&handle[MODL_PROC_ID_EVE3_INST3], MODL_PROC_ID_EVE3_INST3, &modlNet, sizeof(HAL_MODL_Network), tidl_param, tidl_param_size);
    assert(HAL_MODL_OK == ret);
    ret = hal_modl_create_net(&handle[MODL_PROC_ID_EVE4_INST0], MODL_PROC_ID_EVE4_INST0, &modlNet, sizeof(HAL_MODL_Network), tidl_param, tidl_param_size);
    assert(HAL_MODL_OK == ret);
    ret = hal_modl_create_net(&handle[MODL_PROC_ID_EVE4_INST1], MODL_PROC_ID_EVE4_INST1, &modlNet, sizeof(HAL_MODL_Network), tidl_param, tidl_param_size);
    assert(HAL_MODL_OK == ret);
    ret = hal_modl_create_net(&handle[MODL_PROC_ID_EVE4_INST2], MODL_PROC_ID_EVE4_INST2, &modlNet, sizeof(HAL_MODL_Network), tidl_param, tidl_param_size);
    assert(HAL_MODL_OK == ret);
    ret = hal_modl_create_net(&handle[MODL_PROC_ID_EVE4_INST3], MODL_PROC_ID_EVE4_INST3, &modlNet, sizeof(HAL_MODL_Network), tidl_param, tidl_param_size);
    assert(HAL_MODL_OK == ret);

    for (i = 0; i < 5; i++)
    {
        hal_modl_put_input_buffer(handle[MODL_PROC_ID_EVE1_INST0], tidl_data);
        hal_modl_wait_output_buffer(handle[MODL_PROC_ID_EVE1_INST0], NULL);
        hal_modl_put_input_buffer(handle[MODL_PROC_ID_EVE1_INST1], tidl_data);
        hal_modl_wait_output_buffer(handle[MODL_PROC_ID_EVE1_INST1], NULL);
        hal_modl_put_input_buffer(handle[MODL_PROC_ID_EVE1_INST2], tidl_data);
        hal_modl_wait_output_buffer(handle[MODL_PROC_ID_EVE1_INST2], NULL);
        hal_modl_put_input_buffer(handle[MODL_PROC_ID_EVE1_INST3], tidl_data);
        hal_modl_wait_output_buffer(handle[MODL_PROC_ID_EVE1_INST3], NULL);
        hal_modl_put_input_buffer(handle[MODL_PROC_ID_EVE2_INST0], tidl_data);
        hal_modl_wait_output_buffer(handle[MODL_PROC_ID_EVE2_INST0], NULL);
        hal_modl_put_input_buffer(handle[MODL_PROC_ID_EVE2_INST1], tidl_data);
        hal_modl_wait_output_buffer(handle[MODL_PROC_ID_EVE2_INST1], NULL);
        hal_modl_put_input_buffer(handle[MODL_PROC_ID_EVE2_INST2], tidl_data);
        hal_modl_wait_output_buffer(handle[MODL_PROC_ID_EVE2_INST2], NULL);
        hal_modl_put_input_buffer(handle[MODL_PROC_ID_EVE2_INST3], tidl_data);
        hal_modl_wait_output_buffer(handle[MODL_PROC_ID_EVE2_INST3], NULL);
        hal_modl_put_input_buffer(handle[MODL_PROC_ID_EVE3_INST0], tidl_data);
        hal_modl_wait_output_buffer(handle[MODL_PROC_ID_EVE3_INST0], NULL);
        hal_modl_put_input_buffer(handle[MODL_PROC_ID_EVE3_INST1], tidl_data);
        hal_modl_wait_output_buffer(handle[MODL_PROC_ID_EVE3_INST1], NULL);
        hal_modl_put_input_buffer(handle[MODL_PROC_ID_EVE3_INST2], tidl_data);
        hal_modl_wait_output_buffer(handle[MODL_PROC_ID_EVE3_INST2], NULL);
        hal_modl_put_input_buffer(handle[MODL_PROC_ID_EVE3_INST3], tidl_data);
        hal_modl_wait_output_buffer(handle[MODL_PROC_ID_EVE3_INST3], NULL);
        hal_modl_put_input_buffer(handle[MODL_PROC_ID_EVE4_INST0], tidl_data);
        hal_modl_wait_output_buffer(handle[MODL_PROC_ID_EVE4_INST0], NULL);
        hal_modl_put_input_buffer(handle[MODL_PROC_ID_EVE4_INST1], tidl_data);
        hal_modl_wait_output_buffer(handle[MODL_PROC_ID_EVE4_INST1], NULL);
        hal_modl_put_input_buffer(handle[MODL_PROC_ID_EVE4_INST2], tidl_data);
        hal_modl_wait_output_buffer(handle[MODL_PROC_ID_EVE4_INST2], NULL);
        hal_modl_put_input_buffer(handle[MODL_PROC_ID_EVE4_INST3], tidl_data);
        hal_modl_wait_output_buffer(handle[MODL_PROC_ID_EVE4_INST3], NULL);
    }

    hal_modl_destroy_net(handle[MODL_PROC_ID_EVE1_INST0]);
    hal_modl_destroy_net(handle[MODL_PROC_ID_EVE1_INST1]);
    hal_modl_destroy_net(handle[MODL_PROC_ID_EVE1_INST2]);
    hal_modl_destroy_net(handle[MODL_PROC_ID_EVE1_INST3]);
    hal_modl_destroy_net(handle[MODL_PROC_ID_EVE2_INST0]);
    hal_modl_destroy_net(handle[MODL_PROC_ID_EVE2_INST1]);
    hal_modl_destroy_net(handle[MODL_PROC_ID_EVE2_INST2]);
    hal_modl_destroy_net(handle[MODL_PROC_ID_EVE2_INST3]);
    hal_modl_destroy_net(handle[MODL_PROC_ID_EVE3_INST0]);
    hal_modl_destroy_net(handle[MODL_PROC_ID_EVE3_INST1]);
    hal_modl_destroy_net(handle[MODL_PROC_ID_EVE3_INST2]);
    hal_modl_destroy_net(handle[MODL_PROC_ID_EVE3_INST3]);
    hal_modl_destroy_net(handle[MODL_PROC_ID_EVE4_INST0]);
    hal_modl_destroy_net(handle[MODL_PROC_ID_EVE4_INST1]);
    hal_modl_destroy_net(handle[MODL_PROC_ID_EVE4_INST2]);
    hal_modl_destroy_net(handle[MODL_PROC_ID_EVE4_INST3]);

    hal_modl_deinit_process_level();

    ret = hal_modl_free_continuous_aligned((void*)tidl_net, tidl_net_size);
    assert(HAL_MODL_OK == ret);
    if(0 != tidl_param_size)
    {
        ret = hal_modl_free_continuous_aligned((void*)tidl_param, tidl_param_size);
        assert(HAL_MODL_OK == ret);
    }
    ret = hal_modl_free_continuous_aligned((void*)tidl_data, dataSize);
    assert(HAL_MODL_OK == ret);

    system_unmount();

    return ret;
}
