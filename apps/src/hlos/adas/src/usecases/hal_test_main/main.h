/******************************************************************************

Copyright (c) [2018 - 2019] MOMENTA Incorporated. All rights reserved.

******************************************************************************/


/*******************************************************************************
 *
 * 文件: main.h
 *
 * 描述: 该文件提供HAL 模块测试各个模块功能的接口函数
 *
 ******************************************************************************/

#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <src/hlos/adas/src/usecases/hal_arch/hal_arch.h>
#include <src/hlos/adas/src/usecases/hal_camera/hal_camera.h>
#include <src/hlos/adas/src/usecases/hal_sonar/hal_sonar.h>
#include <src/hlos/adas/src/usecases/hal_can/hal_can.h>
#include <src/hlos/adas/src/usecases/hal_system/hal_system.h>

void camera_run(void);
void sonar_run(void);
void can_run(void);
void system_run(void);

#endif //__MAIN_H__

