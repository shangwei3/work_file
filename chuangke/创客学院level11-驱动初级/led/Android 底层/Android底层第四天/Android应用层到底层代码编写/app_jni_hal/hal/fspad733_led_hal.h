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


#ifndef ANDROID_FSPAD733_LED_HAL_INTERFACE_H
#define ANDROID_FSPAD733_LED_HAL_INTERFACE_H

#include <stdint.h>

#include <hardware/hardware.h>

__BEGIN_DECLS

/**
 * The id of this module
 */
#define FSPAD733_LED_HARDWARE_MODULE_ID "fspad733_led"

#define LED_ON 	_IO('k', 0)
#define LED_OFF _IO('k', 1)


/**
 * Every hardware module must have a data structure named HAL_MODULE_INFO_SYM
 * and the fields of this data structure must begin with hw_module_t
 * followed by module specific information.
 */
struct fspad733_led_module_t {
    struct hw_module_t common;
};

struct fspad733_led_hw_device_t {
    /**
     * Common methods of the local time hardware device.  This *must* be the first member of
     * local_time_hw_device as users of this structure will cast a hw_device_t to
     * local_time_hw_device pointer in contexts where it's known the hw_device_t references a
     * local_time_hw_device.
     */
    struct hw_device_t common;
	void (*led_on)(struct fspad733_led_hw_device_t *);
	void (*led_off)(struct fspad733_led_hw_device_t *);
	int fd;
};

__END_DECLS

#endif  // ANDROID_LOCAL_TIME_INTERFACE_H
