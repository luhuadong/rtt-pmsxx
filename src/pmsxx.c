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

#define DBG_TAG "sensor.plantower.pms"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

static struct pms_cmd preset_commands[] = {
    { 0x42, 0x4d, 0xe2, 0x00, 0x00, 0x01, 0x71 },  /* Read in passive mode */
    { 0x42, 0x4d, 0xe1, 0x00, 0x00, 0x01, 0x70 },  /* Change to passive mode */
    { 0x42, 0x4d, 0xe1, 0x00, 0x01, 0x01, 0x71 },  /* Change to active  mode */
    { 0x42, 0x4d, 0xe4, 0x00, 0x00, 0x01, 0x73 },  /* Change to standby mode */
    { 0x42, 0x4d, 0xe4, 0x00, 0x01, 0x01, 0x74 }   /* Change to normal  mode */
};

static void pms_show_command(pms_cmd_t cmd)
{
    RT_ASSERT(cmd);

    rt_kprintf("[START1] : %02x\n", cmd->START1);
    rt_kprintf("[START2] : %02x\n", cmd->START2);
    rt_kprintf("[CMD]    : %02x\n", cmd->CMD);
    rt_kprintf("[DATAH]  : %02x\n", cmd->DATAH);
    rt_kprintf("[DATAL]  : %02x\n", cmd->DATAL);
    rt_kprintf("[LRCH]   : %02x\n", cmd->LRCH);
    rt_kprintf("[LRCL]   : %02x\n", cmd->LRCL);
}

static void pms_show_response(pms_response_t resp)
{
    RT_ASSERT(resp);

    rt_kprintf("Start : %x %x\n", resp->start1, resp->start2);
    rt_kprintf("Length: %04x\n", resp->length);
    rt_kprintf("[01]  : %04x\n", resp->PM1_0_CF1);
    rt_kprintf("[02]  : %04x\n", resp->PM2_5_CF1);
    rt_kprintf("[03]  : %04x\n", resp->PM10_0_CF1);
    rt_kprintf("[04]  : %04x\n", resp->PM1_0_amb);
    rt_kprintf("[05]  : %04x\n", resp->PM2_5_amb);
    rt_kprintf("[06]  : %04x\n", resp->PM10_0_amb);
    rt_kprintf("[07]  : %04x\n", resp->air_0_3um);
    rt_kprintf("[08]  : %04x\n", resp->air_0_5um);
    rt_kprintf("[09]  : %04x\n", resp->air_1_0um);
    rt_kprintf("[10]  : %04x\n", resp->air_2_5um);
    rt_kprintf("[11]  : %04x\n", resp->air_5_0um);
    rt_kprintf("[12]  : %04x\n", resp->air_10_0um);
    rt_kprintf("Ver   : %02x\n", resp->version);
    rt_kprintf("Err   : %02x\n", resp->errorCode);
    rt_kprintf("Sum   : %04x\n", resp->checksum);
}

/**
 * Receive callback function
 */
static rt_err_t pms_uart_input(rt_device_t dev, rt_size_t size)
{
    RT_ASSERT(dev != RT_NULL);
    pms_device_t pms = (pms_device_t)dev->user_data;

    if (pms)
        rt_sem_release(pms->rx_sem);

    return RT_EOK;
}

static void pms_recv_entry(void *parameter)
{
    pms_device_t dev = (pms_device_t)parameter;
    char ch;
    rt_size_t len = 0;
    struct pms_response resp;
    char *p = (char *)&resp;

    while (1)
    {
        while (rt_device_read(dev->serial, -1, &ch, 1) != 1)
        {
            rt_sem_take(dev->rx_sem, RT_WAITING_FOREVER);
        }
#if 1
        rt_kprintf("%02x ", ch);
#else
        if (len < sizeof(resp))
        {
            *p++ = ch;
        }
        else
        {
            pms_show_response(&resp);
            len = 0;
            p = (char *)&resp;
        }
#endif
    }
}

rt_bool_t pms_measure(pms_device_t dev)
{
    RT_ASSERT(dev);

    rt_device_write(dev->serial, 0, &preset_commands[0], sizeof(struct pms_cmd));
    pms_show_command(&preset_commands[0]);
    
    return RT_TRUE;
}

rt_err_t pms_set_mode(pms_device_t dev, pms_mode_t mode)
{
    RT_ASSERT(dev);

    rt_device_write(dev->serial, 0, &preset_commands[mode], sizeof(struct pms_cmd));
    pms_show_command(&preset_commands[mode]);

    return RT_EOK;
}

/**
 * This function initializes pmsxx registered device driver
 *
 * @param dev the name of pmsxx device
 *
 * @return the pmsxx device.
 */
pms_device_t pms_create(const char *uart_name)
{
    RT_ASSERT(uart_name);

    pms_device_t dev = rt_calloc(1, sizeof(struct pms_device));
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

    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;

    config.baud_rate = BAUD_RATE_9600;
    config.data_bits = DATA_BITS_8;
    config.stop_bits = STOP_BITS_1;
    config.bufsz     = RT_SERIAL_RB_BUFSZ;
    config.parity    = PARITY_NONE;

    rt_device_control(dev->serial, RT_DEVICE_CTRL_CONFIG, &config);
    //rt_device_close(dev->serial);

    /* Dangerous */
    dev->serial->user_data = (void *)dev;

    /* init semaphore */
    dev->rx_sem = rt_sem_create("pms_rx", 0, RT_IPC_FLAG_FIFO);
    if (dev->rx_sem == RT_NULL)
    {
        LOG_E("Can't create semaphore for pmsxx device");
        rt_free(dev);
        return RT_NULL;
    }

    rt_device_open(dev->serial, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);

    /* Set mode */
    rt_device_write(dev->serial, 0, &preset_commands[PMS_MODE_STANDBY], sizeof(struct pms_cmd));
    rt_thread_mdelay(1000);

    rt_device_set_rx_indicate(dev->serial, pms_uart_input);

    /* create thread */
    dev->rx_tid = rt_thread_create("pms_rx", pms_recv_entry, dev, 1024, 25, 10);
    if (dev->rx_tid == RT_NULL)
    {
        LOG_E("Can't create thread for pmsxx device");
        rt_sem_delete(dev->rx_sem);
        rt_free(dev);
        return RT_NULL;
    }
    rt_thread_startup(dev->rx_tid);

    /* init mutex */
    dev->lock = rt_mutex_create("pms_lock", RT_IPC_FLAG_FIFO);
    if (dev->lock == RT_NULL)
    {
        LOG_E("Can't create mutex for pmsxx device");
        rt_thread_delete(dev->rx_tid);
        rt_sem_delete(dev->rx_sem);
        rt_free(dev);
        return RT_NULL;
    }

    return dev;
}

/**
 * This function releases memory and deletes mutex lock
 *
 * @param dev the pointer of device driver structure
 */
void pms_delete(pms_device_t dev)
{
    if (dev)
    {
        dev->serial->user_data = RT_NULL;
        rt_device_close(dev->serial);
        rt_sem_delete(dev->rx_sem);
        rt_mutex_delete(dev->lock);
        rt_thread_delete(dev->rx_tid);
        rt_free(dev);
    }
}
