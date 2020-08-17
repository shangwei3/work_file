/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "fspad733_led_hw_default"
//#define LOG_NDEBUG 0
#include <errno.h>
#include <stdint.h>
#include <sys/time.h>
#include <linux/time.h>
#include <cutils/log.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <hardware/hardware.h>
#include <hardware/fspad733_led_hal.h>

void fspad733_led_on(struct fspad733_led_hw_device_t *fspad_led_dev)
{

    int ret;
	ret = ioctl(fspad_led_dev->fd,LED_ON);
	if(ret < 0){
		ALOGE("Fail to ioctl LED_ON : %s\n",strerror(errno));
		return;
	}

	return;
}

void fspad733_led_off(struct fspad733_led_hw_device_t *fspad_led_dev)
{
    int ret;
	ret = ioctl(fspad_led_dev->fd,LED_OFF);
	if(ret < 0){
		ALOGE("Fail to ioctl LED_ON : %s\n",strerror(errno));
		return;
	}

	return;
}


static int fspad733_led_close(hw_device_t *device)
{
    free(device);
    return 0;
}

static int fspad733_led_open(const hw_module_t* module, const char* name,
                     hw_device_t** device)
{
    struct fspad733_led_hw_device_t *fspad_led_dev;
    int ret;

    fspad_led_dev = calloc(1, sizeof(struct fspad733_led_hw_device_t));
    if (!fspad_led_dev)
        return -ENOMEM;

    fspad_led_dev->common.tag = HARDWARE_DEVICE_TAG;
    fspad_led_dev->common.version = 0;
    fspad_led_dev->common.module = (struct hw_module_t *) module;
    fspad_led_dev->common.close = fspad733_led_close;

	fspad_led_dev->led_on = fspad733_led_on;
	fspad_led_dev->led_off = fspad733_led_off;
	
    *device = &fspad_led_dev->common;
	
	fspad_led_dev->fd = open("/dev/led",O_RDWR);
	if(fspad_led_dev->fd < 0){
		ALOGE("Fail to open /dev/led : %s\n",strerror(errno));	
		return -ENODEV;
	}
	
    return 0;
}

static struct hw_module_methods_t hal_module_methods = {
    .open = fspad733_led_open,
};

struct fspad733_led_module_t HAL_MODULE_INFO_SYM = {
    .common = {
        .tag = HARDWARE_MODULE_TAG,
        .version_major = 1,
        .version_minor = 0,
        .id = FSPAD733_LED_HARDWARE_MODULE_ID,
        .name = "Default fspad733_led HW HAL",
        .author = "The Farsight Open Source Project",
        .methods = &hal_module_methods,
    },
};
