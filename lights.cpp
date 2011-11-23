/******************************************************************/
/*	Copyright (C)  ROCK-CHIPS FUZHOU . All Rights Reserved.  	  */
/*******************************************************************
 * File    :   lights.cpp
 * Desc    :   Implement lights adjust HAL
 * Author  :   CMY
 * Date    :   2009-07-22
 * Notes   :   ..............
 *
 * Revision 1.00  2009/07/22 CMY
 *
 * ...................
 * ********************************************************************/

#define LOG_TAG "Lights"

//#include <hardware/hardware.h>
//#include <hardware/lights.h>
#include "lights.h"

#include <fcntl.h>
#include <errno.h>

#include <cutils/log.h>
#include <cutils/atomic.h>

/*****************************************************************************/
#define BACKLIGHT_PATH	"/sys/class/backlight/rk28_bl/brightness"
int g_fd = 0;

static int light_device_open(const struct hw_module_t* module, const char* name, struct hw_device_t** device);

static struct hw_module_methods_t light_module_methods = {
    open: light_device_open
};

struct light_module_t HAL_MODULE_INFO_SYM = {
    common: {
        tag: HARDWARE_MODULE_TAG,
        version_major: 1,
        version_minor: 0,
        id: LIGHTS_HARDWARE_MODULE_ID,
        name: "Lights module",
        author: "Rockchip",
        methods: &light_module_methods,
    }
};

int set_backlight_light(struct light_device_t* dev, struct light_state_t const* state)
{
	unsigned int color = state->color;
	unsigned char brightness = ((77*((color>>16)&0x00ff)) + (150*((color>>8)&0x00ff)) + (29*(color&0x00ff))) >> 8;
	char buf[32];
	int len;

//	LOGI(">>> color=%08X flashMode=%X flashOnMS=%X flashOffMS=%X", state->color, state->flashMode, state->flashOnMS, state->flashOffMS);

	if(g_fd <= 0)
	{
		LOGI(">>> Open file: %s", BACKLIGHT_PATH);
		g_fd = open(BACKLIGHT_PATH, O_RDWR);
		if( g_fd < 0 )
		{
			LOGE(">>> Can't open %s", BACKLIGHT_PATH);
			return -1;
		}else
			LOGI("Open success!");
	}

	len = sprintf(buf, "%d", brightness);

//	LOGI(">>> write %s to fd", buf);
	if( write(g_fd, buf, len) < len )
	{
		LOGE(">>> write failed");
		return -2;
	}
	
	return 0;
}

int set_keyboard_light(struct light_device_t* dev, struct light_state_t const* state)
{
	LOGI(">>> Enter set_keyboard_light");
	return 0;
}

int set_buttons_light(struct light_device_t* dev, struct light_state_t const* state)
{
	LOGI(">>> Enter set_buttons_light");
	return 0;
}

int set_battery_light(struct light_device_t* dev, struct light_state_t const* state)
{
	LOGI(">>> Enter set_battery_light");
	return 0;
}

int set_notifications_light(struct light_device_t* dev, struct light_state_t const* state)
{
	LOGI(">>> Enter set_notifications_light");
	return 0;
}

int set_attention_light(struct light_device_t* dev, struct light_state_t const* state)
{
	LOGI(">>> Enter set_attention_light");
	return 0;
}

static int light_device_close(struct hw_device_t *dev) 
{
    struct light_device_t* ctx = (struct light_device_t*)dev;

	LOGI(">>> Enter light_device_close");
    if (ctx) {
        /* free all resources associated with this device here
         * in particular the overlay_handle_t, outstanding overlay_t, etc...
         */
	if(g_fd <= 0)
	{
		close(g_fd);
                g_fd = 0;
	}
        free(ctx);
    }
    return 0;
}

static int light_device_open(const struct hw_module_t* module, const char* name, struct hw_device_t** device)
{
    int status = 0;
	LOGI(">>> Enter light_device_open");

    struct light_device_t *dev;
    dev = (light_device_t*)malloc(sizeof(*dev));

    /* initialize our state here */
    memset(dev, 0, sizeof(*dev));

    /* initialize the procs */
    dev->common.tag = HARDWARE_DEVICE_TAG;
    dev->common.version = 0;
    dev->common.module = const_cast<hw_module_t*>(module);
    dev->common.close = light_device_close;
    *device = &dev->common;
   
    if (!strcmp(name, LIGHT_ID_BACKLIGHT)) {
        dev->set_light = set_backlight_light;
    }else if(!strcmp(name, LIGHT_ID_KEYBOARD)) {
        dev->set_light = set_keyboard_light;
    }else if(!strcmp(name, LIGHT_ID_BUTTONS)) {
        dev->set_light = set_buttons_light;
    }else if(!strcmp(name, LIGHT_ID_BATTERY)) {
        dev->set_light = set_battery_light;
    }else if(!strcmp(name, LIGHT_ID_NOTIFICATIONS)) {
        dev->set_light = set_notifications_light;
    }else if(!strcmp(name, LIGHT_ID_ATTENTION)) {
        dev->set_light = set_attention_light;
   	}else{
	   	LOGI(">>> undefine light id");
	   	free(dev);
	   	*device = NULL;
   		status = -EINVAL;
	}
   	
    return status;
}

