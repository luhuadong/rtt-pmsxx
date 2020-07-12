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

#ifndef PKG_USING_PMSXX_SAMPLE_UART
#define PMSXX_UART_NAME                    "uart3"
#else
#define PMSXX_UART_NAME                    PKG_USING_PMSXX_SAMPLE_UART
#endif

/* cat_pmsxx_passive */
static void cat_pmsxx_passive(void)
{
    rt_uint16_t  loop = 10;
    struct pms_response resp;
    pms_device_t sensor = pms_create(PMSXX_UART_NAME);
    if (!sensor)
    {
        rt_kprintf("(PMS) Init failed\n");
        return;
    }

    while (!pms_is_ready(sensor))
    {
        rt_thread_mdelay(1000);
    }

    pms_set_mode(sensor, PMS_MODE_NORMAL);
    pms_set_mode(sensor, PMS_MODE_PASSIVE);

    while (loop--)
    {
        rt_kprintf("\n[%d] Request...\n", loop);
        pms_read(sensor, &resp, sizeof(resp), rt_tick_from_millisecond(3000));
        pms_show_response(&resp);
        rt_thread_mdelay(3000);
    }
    
    rt_kprintf("(PMS) Finished!\n");
    pms_delete(sensor);
}
#ifdef FINSH_USING_MSH
MSH_CMD_EXPORT(cat_pmsxx_passive, read pmsxx serial sensor data);
#endif


/* cat_pmsxx_active */
static void cat_pmsxx_active(void)
{
    struct pms_response resp;
    pms_device_t sensor = pms_create(PMSXX_UART_NAME);
    if (!sensor)
    {
        rt_kprintf("(PMS) Init failed\n");
        return;
    }

    while (!pms_is_ready(sensor))
    {
        rt_thread_mdelay(1000);
    }

    pms_set_mode(sensor, PMS_MODE_NORMAL);
    pms_set_mode(sensor, PMS_MODE_ACTIVE);

    while (1)
    {
        pms_wait(sensor, &resp, sizeof(resp));
        pms_show_response(&resp);
    }
    
    rt_kprintf("(PMS) Finished!\n");
    pms_delete(sensor);
}
#ifdef FINSH_USING_MSH
MSH_CMD_EXPORT(cat_pmsxx_active, read pmsxx serial sensor data);
#endif
