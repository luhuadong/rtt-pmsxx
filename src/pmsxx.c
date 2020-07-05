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
#include <string.h>
#include "pmsxx.h"

#define DBG_TAG "sensor.plantower.pmsxx"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

static struct pms_cmd preset_commands[] = {
    { 0x42, 0x4d, 0xe2, 0x00, 0x00, 0x01, 0x71 },  /* Read in passive mode */
    { 0x42, 0x4d, 0xe1, 0x00, 0x00, 0x01, 0x70 },  /* Change to passive mode */
    { 0x42, 0x4d, 0xe1, 0x00, 0x01, 0x01, 0x71 },  /* Change to active  mode */
    { 0x42, 0x4d, 0xe4, 0x00, 0x00, 0x01, 0x73 },  /* Change to standby mode */
    { 0x42, 0x4d, 0xe4, 0x00, 0x01, 0x01, 0x74 }   /* Change to normal  mode */
};

/**
 * Receive callback function
 */
static rt_err_t pmsxx_uart_input(rt_device_t dev, rt_size_t size)
{
    pms_device_t pms_dev = (pms_device_t)dev;
    rt_sem_release(&pms_dev->rx_sem);
    return RT_EOK;
}

static void pmsxx_recv_thread_entry(void *parameter)
{
    char ch;

    while (1)
    {
        while (rt_device_read(dev->serial, -1, &ch, 1) != 1)
        {
            rt_sem_take(&dev->rx_sem, RT_WAITING_FOREVER);
        }
        rt_kprintf("%c ", ch);
    }
}

static rt_err_t sensor_init(pms_device_t dev)
{
    /* Change to passive mode */
    rt_device_open(dev->serial, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
    rt_device_set_rx_indicate(dev->serial, pmsxx_uart_input);

    rt_device_write(dev->serial, 0, preset_commands[1], sizeof(struct pms_cmd));

    //rt_device_close(dev->serial);

    return RT_EOK;
}

rt_bool_t pmsxx_measure(pms_device_t dev)
{
    rt_device_write(dev->serial, 0, preset_commands[0], sizeof(struct pms_cmd));
    return RT_TRUE;
}

rt_err_t pmsxx_init(struct pms_device *dev, const char *uart_name)
{
    RT_ASSERT(uart_name);

    /* init uart */
    dev->serial = rt_device_find(uart_name);
    if (dev->serial == RT_NULL)
    {
        LOG_E("Can't find pmsxx device on '%s' ", uart_name);
        return -RT_ERROR;
    }

    rt_device_t serial = rt_device_find(PMSXX_UART_NAME);
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;

    config.baud_rate = BAUD_RATE_9600;
    config.data_bits = DATA_BITS_8;
    config.stop_bits = STOP_BITS_1;
    config.bufsz     = RT_SERIAL_RB_BUFSZ;
    config.parity    = PARITY_NONE;

    rt_device_control(dev->serial, RT_DEVICE_CTRL_CONFIG, &config);
    rt_device_close(dev->serial);

    /* init semaphore */
    rt_sem_init(&dev->rx_sem, "rx_sem", 0, RT_IPC_FLAG_FIFO);

    /* init mutex */
    dev->lock = rt_mutex_create("pmsxx", RT_IPC_FLAG_FIFO);
    if (dev->lock == RT_NULL)
    {
        LOG_E("Can't create mutex for pmsxx device on '%s' ", uart_name);
        return -RT_ERROR;
    }

    return sensor_init(dev);
}

void pmsxx_deinit(struct pms_device *dev)
{
    if (dev)
    {
        rt_mutex_delete(dev->lock);
    }
}

/**
 * This function initializes pmsxx registered device driver
 *
 * @param dev the name of pmsxx device
 *
 * @return the pmsxx device.
 */
pms_device_t pmsxx_create(const char *uart_name)
{
    RT_ASSERT(uart_name);

    pmsxx_device_t dev = rt_calloc(1, sizeof(struct pmsxx_device));
    if (dev == RT_NULL)
    {
        LOG_E("Can't allocate memory for pmsxx device on '%s' ", uart_name);
        return RT_NULL;
    }

    /* init uart */
    dev->serial = rt_device_find(uart_name);
    if (dev->serial == RT_NULL)
    {
        LOG_E("Can't find pmsxx device on '%s' ", uart_name);
        rt_free(dev);
        return RT_NULL;
    }

    rt_device_t serial = rt_device_find(PMSXX_UART_NAME);
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;

    config.baud_rate = BAUD_RATE_9600;
    config.data_bits = DATA_BITS_8;
    config.stop_bits = STOP_BITS_1;
    config.bufsz     = RT_SERIAL_RB_BUFSZ;
    config.parity    = PARITY_NONE;

    rt_device_control(dev->serial, RT_DEVICE_CTRL_CONFIG, &config);
    rt_device_close(dev->serial);

    /* init semaphore */
    rt_sem_init(&dev->rx_sem, "rx_sem", 0, RT_IPC_FLAG_FIFO);

    /* init mutex */
    dev->lock = rt_mutex_create("pmsxx", RT_IPC_FLAG_FIFO);
    if (dev->lock == RT_NULL)
    {
        LOG_E("Can't create mutex for pmsxx device on '%s' ", uart_name);
        rt_free(dev);
        return RT_NULL;
    }

    if (sensor_init(dev) != RT_EOK)
        return RT_NULL;
    else
        return dev;
}

/**
 * This function releases memory and deletes mutex lock
 *
 * @param dev the pointer of device driver structure
 */
void pmsxx_delete(pms_device_t dev)
{
    if (dev)
    {
        rt_mutex_delete(dev->lock);
        rt_free(dev);
    }
}
