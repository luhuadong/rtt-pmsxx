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
#define SENSOR_DUST_PERIOD_MIN         (200)


static struct pms_cmd preset_commands[] = {
    { 0x42, 0x4d, 0xe2, 0x00, 0x00, 0x01, 0x71 },  /* Read in passive mode */
    { 0x42, 0x4d, 0xe1, 0x00, 0x00, 0x01, 0x70 },  /* Change to passive mode */
    { 0x42, 0x4d, 0xe1, 0x00, 0x01, 0x01, 0x71 },  /* Change to active  mode */
    { 0x42, 0x4d, 0xe4, 0x00, 0x00, 0x01, 0x73 },  /* Change to standby mode */
    { 0x42, 0x4d, 0xe4, 0x00, 0x01, 0x01, 0x74 }   /* Change to normal  mode */
};

static rt_size_t _pmsxx_polling_get_data(struct rt_sensor_device *sensor, void *buf)
{
    struct rt_sensor_data *sensor_data = buf;
    rt_uint32_t dust = 0;

    sensor_data->type = RT_SENSOR_CLASS_DUST;
    sensor_data->data.dust = dust;
    sensor_data->timestamp = rt_sensor_get_ts();

    return 1;
}

static rt_size_t pmsxx_fetch_data(struct rt_sensor_device *sensor, void *buf, rt_size_t len)
{
    if (sensor->config.mode == RT_SENSOR_MODE_POLLING)
    {
        return _pmsxx_polling_get_data(sensor, buf);
    }
    else
        return 0;
}

static rt_err_t pmsxx_control(struct rt_sensor_device *sensor, int cmd, void *args)
{
    rt_err_t result = RT_EOK;

    switch (cmd)
    {
    case RT_SENSOR_CTRL_GET_ID:
        break;
    case RT_SENSOR_CTRL_SET_MODE:
        sensor->config.mode = (rt_uint32_t)args & 0xFF;
        break;
    case RT_SENSOR_CTRL_SET_RANGE:
        break;
    case RT_SENSOR_CTRL_SET_ODR:
        break;
    case RT_SENSOR_CTRL_SET_POWER:
        break;
    case RT_SENSOR_CTRL_SELF_TEST:
        break;
    default:
        break;
    }

    return result;
}

static struct rt_sensor_ops sensor_ops =
{
    pmsxx_fetch_data,
    pmsxx_control
};

/**
 * This function will init dhtxx sensor device.
 *
 * @param intf  interface 
 *
 * @return RT_EOK
 */
static rt_err_t _pmsxx_init(void)
{
    return RT_EOK;
}

/**
 * Call function rt_hw_pmsxx_init for initial and register a pmsxx sensor.
 *
 * @param name  the name will be register into device framework
 * @param cfg   sensor config
 *
 * @return the result
 */
rt_err_t rt_hw_pmsxx_init(const char *name, struct rt_sensor_config *cfg)
{
    int result;
    rt_sensor_t sensor_dust = RT_NULL;

    if (_pms_init() != RT_EOK)
    {
        LOG_E("device init failed");
        result = -RT_ERROR;
        goto __exit;
    }

    /* dust sensor register */
    {
        sensor_dust = rt_calloc(1, sizeof(struct rt_sensor_device));
        if (sensor_dust == RT_NULL)
        {
            LOG_E("alloc memory failed");
            result = -RT_ENOMEM;
            goto __exit;
        }

        sensor_dust->info.type       = RT_SENSOR_CLASS_DUST;
        sensor_dust->info.vendor     = RT_SENSOR_VENDOR_PLANTOWER;
        sensor_dust->info.model      = "pmsxx";
        sensor_dust->info.unit       = RT_SENSOR_UNIT_NONE;
        sensor_dust->info.intf_type  = RT_SENSOR_INTF_UART;
        sensor_dust->info.range_max  = SENSOR_DUST_RANGE_MAX;
        sensor_dust->info.range_min  = SENSOR_DUST_RANGE_MIN;
        sensor_dust->info.period_min = SENSOR_DUST_PERIOD_MIN;

        rt_memcpy(&sensor_dust->config, cfg, sizeof(struct rt_sensor_config));
        sensor_dust->ops = &sensor_ops;
        
        result = rt_hw_sensor_register(sensor_dust, name, RT_DEVICE_FLAG_RDWR, RT_NULL);
        if (result != RT_EOK)
        {
            LOG_E("device register err code: %d", result);
            result = -RT_ERROR;
            goto __exit;
        }
    }

    LOG_I("sensor init success");
    return RT_EOK;

__exit:
    if (sensor_dust)
        rt_free(sensor_dust);

    return result;
}
