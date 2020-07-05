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

/* cat_pmsxx_passive */
static void cat_pmsxx_passive(void)
{
    rt_uint16_t  loop = 5;
    pms_device_t pms = pms_create(PMSXX_UART_NAME);
    if (!pms)
    {
        rt_kprintf("(PMS) Init failed\n");
        return;
    }

    pms_set_mode(pms, PMS_MODE_NORMAL);
    rt_thread_mdelay(1000);
    pms_set_mode(pms, PMS_MODE_PASSIVE);
    rt_thread_mdelay(1000);

    while (loop--)
    {
        rt_kprintf("\n==>\n");
        rt_kprintf("01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32\n");
        if (pms_measure(pms))
        {
            rt_thread_mdelay(3000);
        }
    }
    
    rt_kprintf("(PMS) Finished!\n");
    pms_delete(pms);
}
#ifdef FINSH_USING_MSH
MSH_CMD_EXPORT(cat_pmsxx_passive, read pmsxx serial sensor data);
#endif


/* cat_pmsxx_active */
static void cat_pmsxx_active(void)
{
    rt_uint16_t  loop = 5;
    pms_device_t pms = pms_create(PMSXX_UART_NAME);
    if (!pms)
    {
        rt_kprintf("(PMS) Init failed\n");
        return;
    }

    pms_set_mode(pms, PMS_MODE_NORMAL);
    rt_thread_mdelay(1000);
    pms_set_mode(pms, PMS_MODE_ACTIVE);
    rt_thread_mdelay(1000);
    rt_kprintf("01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32\n");

    while (loop--)
    {
        rt_kprintf("\n==>\n");
        rt_thread_mdelay(3000);
    }
    
    rt_kprintf("(PMS) Finished!\n");
    pms_delete(pms);
}
#ifdef FINSH_USING_MSH
MSH_CMD_EXPORT(cat_pmsxx_active, read pmsxx serial sensor data);
#endif
