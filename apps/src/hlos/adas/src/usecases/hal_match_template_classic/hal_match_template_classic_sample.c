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
#include <math.h>
#include "hal_match_template_classic.h"
#include <unistd.h>

#define MOMENTA_MTC_IMAGE_DATA_0            "/mnt/MT/pre.BIN"
#define MOMENTA_MTC_IMAGE_DATA_1            "/mnt/MT/cur.BIN"

#define MOMENTA_MMCSD_MOUNT_PT              "mnt"

#define MOMENTA_MTC_PRM_SIZE                (sizeof(HAL_MTC_MatchParam))

unsigned int width;
unsigned int height;
unsigned int channel;
unsigned int pitch;
unsigned int number;
unsigned int width_temp;
unsigned int height_temp;
unsigned int channel_temp;
unsigned int pitch_temp;
unsigned int number_temp;

unsigned int data_size;

hal_match_template_classic_handle_t mtc_handle[HAL_MTC_PROC_ID_MAX] = {0};
//static char *mtc_data = NULL;
//static char *mtc_data_temp = NULL;

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

    //在内存的分配上，有意让图像和参数的内存不连续，模拟实际的情况
    ret = hal_match_template_classic_malloc_continuous_aligned((void**)&mt_data[0], sizeof(MtInParam)*2, 32);
    assert(HAL_MTC_OK == ret);

    ret = hal_match_template_classic_malloc_continuous_aligned((void**)&mt_data[1], data_size, 32);
    assert(HAL_MTC_OK == ret);
    
    printf("mtc ******addr is:%x,%x!\n",(unsigned int)mt_data[0],(unsigned int)mt_data[1]);

    fp = fopen(MOMENTA_MTC_IMAGE_DATA_0, "rb");
    assert(NULL != fp);

    data = mt_data[1];
    for (i = 0; i < 720; i++)
    {
        read_size = fread(data, 1, 1280, fp);
        if (read_size != 1280)
        {
            fclose(fp);
            return -1;
        }

        data+=1280;
    }

    fclose(fp);

    fp = fopen(MOMENTA_MTC_IMAGE_DATA_1, "rb");
    assert(NULL != fp);

    data = mt_data[1] + 1280*720*2;
    for (i = 0; i < 720; i++)
    {
        read_size = fread(data, 1, 1280, fp);
        if (read_size != 1280)
        {
            fclose(fp);
            return -1;
        }

        data+=1280;
    }

    fclose(fp);
    return ret;
}

static void output_handle(hal_match_template_classic_handle_t handle, HAL_MTC_OutputBuffer *output)
{
    int i;
    MtOutResult* result = (MtOutResult*)output->data;

    printf("A15 get MTC result:num:%d!\n",result->result_num);

    for ( i = 0; i < result->result_num; i++)
    {
        printf("A15 get MTC result:left:%f,top:%f,right:%f,bottom:%f,value:%f!\n",
            result->result[i].new_rect.left,result->result[i].new_rect.top,
            result->result[i].new_rect.right,result->result[i].new_rect.bottom,result->result[i].value);
    }
    
}

int hal_match_template_classic_run(void)
{
    int ret = 0;
    HAL_MTC_CreateParams create_params;

    system_mount();

    memset(&create_params, 0, sizeof(HAL_MTC_CreateParams));

    width = 1280;
    height = 720;
    channel = 1;
    pitch = width * channel;
    number = 1;
    data_size = pitch * height * 3;

    ret = read_input();
    assert(HAL_MTC_OK == ret);

    MtInParam* para = (MtInParam*)mt_data[0];
    para->pre_image = (uint8_t*)hal_match_template_classic_virt2phys((void*)mt_data[1]);
    para->cur_image = (uint8_t*)hal_match_template_classic_virt2phys((void*)(mt_data[1] + pitch*height*2));
    printf("mtc phy ******addr is:%x!\n",(unsigned int)para->pre_image);

    //set test case
    para->pad_num = 6;

    para->templ_pad[0].left = 264;
    para->templ_pad[0].top = 293;
    para->templ_pad[0].right = 492;
    para->templ_pad[0].bottom = 531;

    para->templ_pad[1].left = 658;
    para->templ_pad[1].top = 335;
    para->templ_pad[1].right = 704;
    para->templ_pad[1].bottom = 381;

    para->templ_pad[2].left = 750;
    para->templ_pad[2].top = 341;
    para->templ_pad[2].right = 798;
    para->templ_pad[2].bottom = 441;

    para->templ_pad[3].left = 829;
    para->templ_pad[3].top = 500;
    para->templ_pad[3].right = 953;
    para->templ_pad[3].bottom = 720;

    para->templ_pad[4].left = 1018;
    para->templ_pad[4].top = 312;
    para->templ_pad[4].right = 1280;
    para->templ_pad[4].bottom = 580;

    para->templ_pad[5].left = 0;
    para->templ_pad[5].top = 0;
    para->templ_pad[5].right = 100;
    para->templ_pad[5].bottom = 100;

    para->cur_pad[0].left = 200;
    para->cur_pad[0].top = 200;
    para->cur_pad[0].right = 550;
    para->cur_pad[0].bottom = 600;

    para->cur_pad[1].left = 612;
    para->cur_pad[1].top = 289;
    para->cur_pad[1].right = 750;
    para->cur_pad[1].bottom = 427;

    para->cur_pad[2].left = 702;
    para->cur_pad[2].top = 241;
    para->cur_pad[2].right = 846;
    para->cur_pad[2].bottom = 541;

    para->cur_pad[3].left = 800;
    para->cur_pad[3].top = 500;
    para->cur_pad[3].right = 1000;
    para->cur_pad[3].bottom = 900;

    para->cur_pad[4].left = 950;
    para->cur_pad[4].top = 280;
    para->cur_pad[4].right = 1280;
    para->cur_pad[4].bottom = 620;

    para->cur_pad[5].left = 0;
    para->cur_pad[5].top = 0;
    para->cur_pad[5].right = 200;
    para->cur_pad[5].bottom = 200;

    ret = hal_match_template_classic_init_process_level();
    assert(HAL_MTC_OK == ret);

    create_params.handle = output_handle;
    ret = hal_match_template_classic_init_thread_level(&mtc_handle[HAL_MTC_PROC_ID_DSP1_INST0], HAL_MTC_PROC_ID_DSP1_INST0, &create_params, sizeof(HAL_MTC_CreateParams));
    assert(HAL_MTC_OK == ret);

    ret = hal_match_template_classic_init_thread_level(&mtc_handle[HAL_MTC_PROC_ID_DSP2_INST0], HAL_MTC_PROC_ID_DSP2_INST0, &create_params, sizeof(HAL_MTC_CreateParams));
    assert(HAL_MTC_OK == ret);


    ret = hal_match_template_classic_cache_wb((void*)mt_data[0], sizeof(MtInParam));
    assert(HAL_MTC_OK == ret);

    ret = hal_match_template_classic_cache_wb((void*)mt_data[1], data_size);
    assert(HAL_MTC_OK == ret);
    
    //try twice
    usleep(10000);

    hal_match_template_classic_put_input_buffer(mtc_handle[HAL_MTC_PROC_ID_DSP1_INST0], mt_data[0]);
    hal_match_template_classic_wait_output_buffer(mtc_handle[HAL_MTC_PROC_ID_DSP1_INST0]);
    // hal_match_template_classic_put_input_buffer(mtc_handle[HAL_MTC_PROC_ID_DSP2_INST0], mt_data[0]);
    // hal_match_template_classic_wait_output_buffer(mtc_handle[HAL_MTC_PROC_ID_DSP2_INST0]);

    // hal_match_template_classic_put_input_buffer(mtc_handle[HAL_MTC_PROC_ID_DSP1_INST0], mt_data[0]);
    // hal_match_template_classic_wait_output_buffer(mtc_handle[HAL_MTC_PROC_ID_DSP1_INST0]);
    // hal_match_template_classic_put_input_buffer(mtc_handle[HAL_MTC_PROC_ID_DSP2_INST0], mt_data[0]);
    // hal_match_template_classic_wait_output_buffer(mtc_handle[HAL_MTC_PROC_ID_DSP2_INST0]);

    hal_match_template_classic_deinit_thread_level(mtc_handle[HAL_MTC_PROC_ID_DSP1_INST0]);
    hal_match_template_classic_deinit_thread_level(mtc_handle[HAL_MTC_PROC_ID_DSP2_INST0]);

#if 0    
    ret = hal_match_template_classic_init_thread_level(&mtc_handle[HAL_MTC_PROC_ID_DSP2_INST0], HAL_MTC_PROC_ID_DSP2_INST0, &create_params, sizeof(HAL_MTC_CreateParams));
    assert(HAL_MTC_OK == ret);

    hal_match_template_classic_put_input_buffer(mtc_handle[HAL_MTC_PROC_ID_DSP2_INST0], mt_data[0]);
    hal_match_template_classic_wait_output_buffer(mtc_handle[HAL_MTC_PROC_ID_DSP2_INST0]);


    hal_match_template_classic_deinit_thread_level(mtc_handle[HAL_MTC_PROC_ID_DSP2_INST0]);
#endif

    hal_match_template_classic_deinit_process_level();

    ret = hal_match_template_classic_free_continuous_aligned((void*)mt_data[0], sizeof(MtInParam)*2);
    assert(HAL_MTC_OK == ret);

    ret = hal_match_template_classic_free_continuous_aligned((void*)mt_data[1], data_size);
    assert(HAL_MTC_OK == ret);

    system_unmount();

    return ret;
}
