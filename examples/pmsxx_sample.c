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

#define PMSXX_UART_DEV_NAME      "uart1"

/* cat_pmsxx */
static void cat_pmsxx(void)
{
    /* do something */
}
#ifdef FINSH_USING_MSH
MSH_CMD_EXPORT(cat_pmsxx, read pmsxx serial sensor data);
#endif

static int pmsxx_uart_dev_init(void)
{
    rt_device_t serial = rt_device_find(PMSXX_UART_DEV_NAME);
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;

    config.baud_rate = 9600;
    config.data_bits = DATA_BITS_8;
    config.stop_bits = STOP_BITS_1;
    config.bufsz     = 256;
    config.parity    = PARITY_NONE;

    rt_device_control(serial, RT_DEVICE_CTRL_CONFIG, &config);
    rt_device_close(serial);

    return RT_EOK;
}
INIT_DEVICE_EXPORT(pmsxx_uart_dev_init);
