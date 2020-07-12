/*
 * Copyright (c) 2020, RudyLo <luhuadong@163.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-07-04     luhuadong    the first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "pmsxx.h"

#define PMSXX_UART_NAME      "uart3"

static void read_dust_entry(void *args)
{
    rt_device_t sensor = RT_NULL;
    struct rt_sensor_data sensor_data;

    sensor = rt_device_find(args);
    if (!sensor) 
    {
        rt_kprintf("Can't find PMS device.\n");
        return;
    }

    if (rt_device_open(sensor, RT_DEVICE_FLAG_RDWR))
    {
        rt_kprintf("Open PMS device failed.\n");
        return;
    }

    rt_uint16_t loop = 10;
    while (loop--)
    {
        if (1 != rt_device_read(sensor, 0, &sensor_data, 1))
        {
            rt_kprintf("Read PMS data failed.\n");
            continue;
        }
        rt_kprintf("[%d] PM2.5: %d ug/m3\n", sensor_data.timestamp, sensor_data.data.dust);

        rt_thread_mdelay(3000);
    }

    rt_device_close(sensor);
}

static void dump_dust_entry(void *args)
{
    rt_device_t sensor = RT_NULL;
    struct rt_sensor_data sensor_data;
    struct pms_response resp;
    rt_err_t ret;

    sensor = rt_device_find(args);
    if (!sensor)
    {
        rt_kprintf("Can't find PMS device.\n");
        return;
    }

    if (rt_device_open(sensor, RT_DEVICE_FLAG_RDWR))
    {
        rt_kprintf("Open PMS device failed.\n");
        return;
    }

    rt_uint16_t loop = 10;
    while (loop--)
    {
        ret = rt_device_control(sensor, RT_SENSOR_CTRL_PMS_DUMP, &resp);
        
        if (ret != RT_EOK)
            rt_kprintf("Dump PMS data failed.\n");
        else
            pms_show_response(&resp);

        rt_thread_mdelay(3000);
    }

    rt_device_close(sensor);
}

static int pms_read_sample(void)
{
    rt_thread_t dust_thread;

    dust_thread = rt_thread_create("pms_read", read_dust_entry, 
                                   "dust_pms", 1024, 
                                    RT_THREAD_PRIORITY_MAX / 2, 20);
    
    if (dust_thread) 
        rt_thread_startup(dust_thread);
}

static int pms_dump_sample(void)
{
    rt_thread_t dust_thread;

    dust_thread = rt_thread_create("pms_dump", dump_dust_entry, 
                                   "dust_pms", 1024, 
                                    RT_THREAD_PRIORITY_MAX / 2, 20);
    
    if (dust_thread) 
        rt_thread_startup(dust_thread);
}

#ifdef FINSH_USING_MSH
MSH_CMD_EXPORT(pms_dump_sample, dump pms response data);
MSH_CMD_EXPORT(pms_read_sample, read PM2.5 data);
#endif

static int rt_hw_pms_port(void)
{
    struct rt_sensor_config cfg;
    
    cfg.intf.type = RT_SENSOR_INTF_UART;
    cfg.intf.dev_name = PMSXX_UART_NAME;
    rt_hw_pms_init("pms", &cfg);
    
    return RT_EOK;
}
INIT_COMPONENT_EXPORT(rt_hw_pms_port);