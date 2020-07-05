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

/* cat_pmsxx */
static void cat_pmsxx(void)
{
    struct pms_device pmx9003m;
    if (pmsxx_init(&pmx9003m, PMSXX_UART_NAME) != RT_EOK)
    {
        rt_kprintf("(PMS) Init failed\n");
    }

    if (pmsxx_measure(&pms9003m))
    {
        rt_thread_mdelay(5000);
    }

    pmsxx_deinit(&pms9003m);
}
#ifdef FINSH_USING_MSH
MSH_CMD_EXPORT(cat_pmsxx, read pmsxx serial sensor data);
#endif

