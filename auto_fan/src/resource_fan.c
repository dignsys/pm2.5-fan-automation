/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "st_things.h"
#include "log.h"
#include <peripheral_io.h>

static peripheral_gpio_h g_gpio_h = NULL;

static bool _init_gpio(void)
{
	int pin = 26; /* GPIO8 */
	int ret = PERIPHERAL_ERROR_NONE;

	ret = peripheral_gpio_open(pin, &g_gpio_h);
	if (ret != PERIPHERAL_ERROR_NONE) {
		ERR("peripheral_gpio_open failed [%d]", ret);
		return false;
	}

	ret = peripheral_gpio_set_direction(g_gpio_h, PERIPHERAL_GPIO_DIRECTION_OUT_INITIALLY_LOW);
	if (ret != PERIPHERAL_ERROR_NONE) {
		ERR("peripheral_gpio_set_direction failed [%d]", ret);
		return false;
	}

	ret = peripheral_gpio_set_edge_mode(g_gpio_h, PERIPHERAL_GPIO_EDGE_NONE);
	if (ret != PERIPHERAL_ERROR_NONE) {
		ERR("peripheral_gpio_set_edge_mode failed [%d]", ret);
		return false;
	}

	return true;
}

bool release_gpio(void)
{
	int ret = PERIPHERAL_ERROR_NONE;

	if (g_gpio_h != NULL) {
		ret = peripheral_gpio_close(g_gpio_h);
		if (ret != PERIPHERAL_ERROR_NONE) {
			ERR("peripheral_gpio_close failed [%d]", ret);
			return false;
		}
	}

	return true;
}

bool set_fan_control(uint32_t value)
{
	int ret = PERIPHERAL_ERROR_NONE;

	if (g_gpio_h == NULL) {
		if (_init_gpio() == false) {
			ERR("init_gpio failed");
			return false;
		}
	}

	ret = peripheral_gpio_write(g_gpio_h, value);
	if (ret != PERIPHERAL_ERROR_NONE) {
		ERR("peripheral_gpio_write failed [%d]", ret);
		return false;
	}

	return true;
}

