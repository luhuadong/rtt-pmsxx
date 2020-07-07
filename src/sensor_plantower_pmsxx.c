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

#define DBG_TAG "sensor.plantower.pms"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

/* range */
#define SENSOR_DUST_RANGE_MIN          (0)
#define SENSOR_DUST_RANGE_MAX          (1000)

/* minial period (ms) */
#define SENSOR_DUST_PERIOD_MIN         (200)


static rt_size_t _pmsxx_polling_get_data(struct rt_sensor_device *sensor, void *buf)
{
    struct rt_sensor_data *sensor_data = buf;
    pms_device_t dev = (pms_device_t)sensor->config.intf.user_data;
    struct pms_response resp;

    pms_read(dev, &resp, sizeof(resp), RT_WAITING_FOREVER);

    sensor_data->type = RT_SENSOR_CLASS_DUST;
    sensor_data->data.dust = dev->resp.PM2_5_atm;
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
    pms_device_t dev = (pms_device_t)sensor->config.intf.user_data;

    switch (cmd)
    {
    case RT_SENSOR_CTRL_GET_ID:
        if (args)
        {
            rt_uint8_t *hwid = (rt_uint8_t *)args;
            *hwid = dev->version;
            result = RT_EOK;
        }
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
    case RT_SENSOR_CTRL_PMS_STANDBY:
        LOG_D("Custom command : Set standby mode");
        result = pms_set_mode(dev, PMS_MODE_STANDBY);
        break;
    case RT_SENSOR_CTRL_PMS_NORMAL:
        LOG_D("Custom command : Set normal mode");
        result = pms_set_mode(dev, PMS_MODE_NORMAL);
        break;
    case RT_SENSOR_CTRL_PMS_ACTIVE:
        LOG_D("Custom command : Set active output mode");
        result = pms_set_mode(dev, PMS_MODE_ACTIVE);
        break;
    case RT_SENSOR_CTRL_PMS_PASSIVE:
        LOG_D("Custom command : Set passive output mode");
        result = pms_set_mode(dev, PMS_MODE_PASSIVE);
        break;
    case RT_SENSOR_CTRL_PMS_DUMP:
        LOG_D("Custom command : Dump response");
        if (args)
        {
            rt_uint16_t ret = pms_read(dev, args, sizeof(struct pms_response), rt_tick_from_millisecond(3000));
            if (ret != sizeof(struct pms_response))
                result = -RT_ERROR;
        }
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
static rt_err_t _pmsxx_init(struct rt_sensor_intf *intf)
{
    if (intf->type == RT_SENSOR_INTF_UART)
    {
        pms_device_t dev = pms_create(intf->dev_name);
        if (!dev)
        {
            LOG_E("PMSxx sensor init failed\n");
            return -RT_ERROR;
        }
        intf->user_data = (void *)dev;
    }
    return RT_EOK;
}

/**
 * Call function rt_hw_pms_init for initial and register a pmsxx sensor.
 *
 * @param name  the name will be register into device framework
 * @param cfg   sensor config
 *
 * @return the result
 */
rt_err_t rt_hw_pms_init(const char *name, struct rt_sensor_config *cfg)
{
    int result;
    rt_sensor_t sensor = RT_NULL;

    if (_pmsxx_init(&cfg->intf) != RT_EOK)
    {
        return -RT_ERROR;
    }

    /* pms sensor register */
    {
        sensor = rt_calloc(1, sizeof(struct rt_sensor_device));
        if (sensor == RT_NULL)
        {
            LOG_E("alloc memory failed");
            result = -RT_ENOMEM;
            goto __exit;
        }

        sensor->info.type       = RT_SENSOR_CLASS_DUST;
        sensor->info.vendor     = RT_SENSOR_VENDOR_PLANTOWER;
        sensor->info.model      = "pmsxx";
        sensor->info.unit       = RT_SENSOR_UNIT_NONE;
        sensor->info.intf_type  = RT_SENSOR_INTF_UART;
        sensor->info.range_max  = SENSOR_DUST_RANGE_MAX;
        sensor->info.range_min  = SENSOR_DUST_RANGE_MIN;
        sensor->info.period_min = SENSOR_DUST_PERIOD_MIN;

        rt_memcpy(&sensor->config, cfg, sizeof(struct rt_sensor_config));
        sensor->ops = &sensor_ops;
        
        result = rt_hw_sensor_register(sensor, name, RT_DEVICE_FLAG_RDWR, RT_NULL);
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
    if (sensor)
        rt_free(sensor);

    return result;
}
