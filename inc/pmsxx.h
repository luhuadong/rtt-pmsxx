/*
 * Copyright (c) 2020, RudyLo <luhuadong@163.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-06-12     RudyLo       the first version
 */

#ifndef __PMSXX_H__
#define __PMSXX_H__

#include <rtthread.h>
#include <rtdevice.h>
#include <sensor.h>

#define PMSLIB_VERSION       "0.0.1"

#define PMS1003              (0)
#define PMS3003              (1)
#define PMS3003P             (2)
#define PMS5003              (3)
#define PMS5003P             (4)
#define PMS5003T             (5)
#define PMS5003S             (6)
#define PMS5003ST            (7)
#define PMS5003I             (8)
#define PMS6003              (9)
#define PMS6003P             (10)
#define PMS7003              (11)
#define PMS7003P             (12)
#define PMS7003I             (13)
#define PMS7003M             (14)
#define PMS9003M             (15)
#define PMSA003              (16)
#define PMSA003P             (17)
#define PMSA003I             (18)

struct pms_device
{
    rt_device_t serial; 
    struct serial_configure config; 
    rt_uint16_t len;

    rt_uint16_t PM1_0_CF1;
    rt_uint16_t PM2_5_CF1;
    rt_uint16_t PM10_0_CF1;
    rt_uint16_t PM1_0_amb;
    rt_uint16_t PM2_5_amb;
    rt_uint16_t PM10_0_amb;
    rt_uint16_t air_0_3um;
    rt_uint16_t air_0_5um;
    rt_uint16_t air_1_0um;
    rt_uint16_t air_2_5um;
    rt_uint16_t air_5_0um;
    rt_uint16_t air_10_0um;
    
    rt_uint16_t hcho;
    rt_uint16_t temp;
    rt_uint16_t humi;
    
    rt_uint8_t  version;
    rt_uint8_t  errorCode;
    rt_uint16_t checksum;
};
typedef struct pms_device *pms_device_t;


#endif /* __PMSXX_H__ */