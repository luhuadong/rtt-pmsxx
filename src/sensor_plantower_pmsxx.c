/*
 * Copyright (c) 2020, RudyLo <luhuadong@163.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-07-04     luhuadong    the first version
 */

#include <board.h>
#include "pmsxx.h"

#define DBG_TAG "sensor.plantower.pmsxx"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

/* range */
#define SENSOR_DUST_RANGE_MIN          (0)
#define SENSOR_DUST_RANGE_MAX          (1000)

/* minial period (ms) */
#define SENSOR_DUST_PERIOD_MIN         (0)
