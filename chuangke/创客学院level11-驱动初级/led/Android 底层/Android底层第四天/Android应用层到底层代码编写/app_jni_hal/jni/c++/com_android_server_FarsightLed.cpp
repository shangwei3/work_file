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

#define LOG_TAG "FarsightLed"

#include "utils/Log.h"

#include "jni.h"
#include "JNIHelp.h"
#include "android_runtime/AndroidRuntime.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <hardware/fspad733_led_hal.h>

namespace android{

	struct fspad733_led_hw_device_t *fspad_led_dev;

	static void jni_led_init(JNIEnv *env,jobject clazz)
	{
		int ret;
		struct hw_module_t *module;

		ALOGE("jni c++ : led_init");

		ret = hw_get_module(FSPAD733_LED_HARDWARE_MODULE_ID,(const struct hw_module_t **)&module);
		if(ret < 0){
			ALOGE("Fail to hw_get_module ,not found : %s\n",FSPAD733_LED_HARDWARE_MODULE_ID);
			return;
		}

		ret = module->methods->open(module,NULL,(struct hw_device_t **)&fspad_led_dev);
		if(ret < 0){
			ALOGE("Fail to module->methods->open");
			return;
		}

		return;
	}

	static void jni_led_on(JNIEnv *env,jobject clazz)
	{
		ALOGE("jni c++ : led_on");
		fspad_led_dev->led_on(fspad_led_dev);
		return;
	}

	static void jni_led_off(JNIEnv *env,jobject clazz)
	{
		ALOGE("jni c++ : led_off");
		fspad_led_dev->led_off(fspad_led_dev);
		return;
	}


	static JNINativeMethod method_table[] = {
		{"ledInit", "()V",(void *)jni_led_init},
		{"ledOn",   "()V",(void *)jni_led_on},
		{"ledOff",  "()V",(void *)jni_led_off},
	};

	int register_android_server_FarsightLed(JNIEnv *env)
	{
		jclass clazz = env->FindClass("com/android/server/FarsightLedService");
		if (clazz == NULL) {
			ALOGE("Can't find com/android/server/FarsightLedService");
			return -1;
		}

		return AndroidRuntime::registerNativeMethods(env, "com/android/server/FarsightLedService",
				method_table, NELEM(method_table));
	}

}
