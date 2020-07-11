# rtt-pmsxx
Plantower PMSxx family sensor driver for RT-Thread



## 1、介绍

pmsxx 软件包适用于攀藤 PMS 系列数字式通用颗粒物传感器模块，支持 PMS1003、PMS3003、PMS5003、PMS7003、PMS9003、PMSA003 等多种型号传感器。

PMS 系列传感器基于激光散射原理，可连续采集并计算单位体积内空气中不同粒径的悬浮颗粒物浓度，并通过串口输出数据，比如 PM1.0、PM2.5、PM10 浓度等数据。



### 1.1 特性

- 支持多实例
- 支持静态和动态分配内存
- 支持 sensor 设备驱动框架
- 支持串口中断和 DMA 模式
- 线程安全



### 1.2 工作模式

| 设备         | 空气颗粒物浓度 |
| :----------- | :------------- |
| **通信接口** |                |
| UART          | √              |
| **工作模式** |                |
| 主动式（中断） | √              |
| 被动式（轮询）  | √ |

自定义接口支持主动式和被动式两种工作模式，Sensor 驱动框架接口目前只支持被动式（主机轮询）工作模式。



### 1.3 目录结构

| 名称     | 说明                           |
| -------- | ------------------------------ |
| docs     | 文档目录                       |
| examples | 例子目录（提供两种操作示例）   |
| inc      | 头文件目录                     |
| src      | 源代码目录（提供两种驱动接口） |

驱动源代码提供两种接口，分别是自定义接口，以及 RT-Thread 设备驱动接口（open/read/control/close）。



### 1.4 许可证

pmsxx 软件包遵循 Apache license v2.0 许可，详见 `LICENSE` 文件。



### 1.5 依赖

- RT-Thread 4.0+
- 使用动态创建方式需要开启动态内存管理模块（`RT_USING_HEAP`）
- 使用 sensor 设备接口需要开启 sensor 设备驱动框架模块



## 2、获取 pmsxx 软件包

使用 pmsxx package 需要在 RT-Thread 的包管理器中选择它，具体路径如下：

```
RT-Thread online packages --->
    peripheral libraries and drivers --->
        [*] sensors drivers  --->
            [*] PMSxx: Plantower pms serial PM2.5 sensor driver.
```

然后让 RT-Thread 的包管理器自动更新，或者使用 `pkgs --update` 命令更新包到 BSP 中。



## 3、使用 pmsxx 软件包

### 3.1 版本说明

| 版本   | 说明                                                |
| ------ | --------------------------------------------------- |
| latest | 支持主动式和被动式工作模式，支持串口中断和 DMA 模式 |

目前处于公测阶段，建议开发者使用 latest 版本。



### 3.2 配置选项

```
(2000) The waiting time after send command (ms) 
[ ]   Enable initialize by thread
[ ]   Enable serial device DMA RX
[ ]   Debug command and response
[ ]     Show commands
[ ]     Show response data
[ ]     Dump raw response data
[ ]       Show ruler for raw response data
[ ]   Enable pmsxx sample
```

选项说明：

- 串口发送命令等待时间（并非所有模式设置命令都有响应，建议设置此参数，避免设置失败）
- 使能初始化线程（该软件包会将 pms 传感器设置为被动模式，使能该选项可以消除等待时间的影响）
- 使能串口 DMA 功能（开启该选项时请确保所使用的串口支持 DMA，并且已开启相关配置）
- 调试命令和响应
    - 当设置传感器模式时，在 msh 终端显示该命令内容
    - 当接收到响应数据时，在 msh 终端显示该响应内容
    - 当接收到响应数据时，以原始数据的形式显示
        - 显示标尺，以方便识别原始数据
- 使能 pmsxx 示例代码



## 4、API 说明

### 4.1 自定义接口

#### 创建对象

要操作传感器模块，首先需要创建一个传感器对象。

```c
pms_device_t pms_create(const char *uart_name);
```

调用这个函数时，会从动态堆内存中分配一个 pms_device_t 句柄，并按给定参数初始化。

| 参数         | 描述                         |
| ------------ | ---------------------------- |
| uart_name    | 串口名称                     |
| **返回**     | ——                           |
| pms_device_t | 创建成功，返回传感器对象句柄 |
| RT_NULL      | 创建失败                     |



#### 删除对象

对于使用 `pms_create()` 创建出来的对象，当不需要使用，或者运行出错时，请使用下面的函数接口将其删除，避免内存泄漏。

```c
void pms_delete(pms_device_t dev);
```

| **参数** | **描述**               |
| -------- | ---------------------- |
| dev      | 要删除的传感器对象句柄 |
| **返回** | ——                     |
| 无       |                        |



#### 读取数据

pms_read 接口适用于被动式，pms_wait 接口适用于主动式。

```c
rt_uint16_t pms_read(pms_device_t dev, void *buf, rt_uint16_t size, rt_int32_t time);
rt_uint16_t pms_wait(pms_device_t dev, void *buf, rt_uint16_t size);
```

| 参数        | 描述                                                         |
| ----------- | ------------------------------------------------------------ |
| dev         | 传感器对象句柄                                               |
| buf         | 数据存储空间，通常传入大小为 `sizeof(struct pms_response)` 的内存空间 |
| size        | buf 的大小，通常为 `sizeof(struct pms_response)`             |
| time        | 指定的等待时间，单位是操作系统时钟节拍，`RT_WAITING_FOREVER` 表示永远阻塞 |
| **返回**    | ——                                                           |
| rt_uint16_t | 读取成功，返回读取到的字节数                                 |
| 0           | 读取失败                                                     |



#### 设置模式

```c
rt_err_t pms_set_mode(pms_device_t dev, pms_mode_t mode);
```

| **参数**  | **描述**       |
| --------- | -------------- |
| dev       | 传感器对象句柄 |
| mode      | 设定的模式     |
| **返回**  | ——             |
| RT_EOK    | 设置成功       |
| -RT_ERROR | 设置失败       |

目前支持以下模式：

- `PMS_MODE_PASSIVE`：设置为被动模式
- `PMS_MODE_ACTIVE`：设置为主动模式
- `PMS_MODE_STANDBY`：设置为待机模式
- `PMS_MODE_NORMAL`：设置为正常模式



#### 打印数据

为了方便调试，提供了三个数据打印函数，pms_show_command 函数用于显示命令帧格式，pms_show_response 函数用于显示响应数据，pms_dump 用于显示原始的比特位数据。

```c
void pms_show_command(pms_cmd_t cmd);
void pms_show_response(pms_response_t resp);
void pms_dump(const char *buf, rt_uint16_t size);
```

显示效果：

【命令】

```
+-----------------------------------------------------+
| HEAD1 | HEAD2 |  CMD  | DATAH | DATAL | LRCH | LRCL |
 ----------------------------------------------------- 
|   42  |   4d  |   e2  |   00  |   00  |  01  |  71  |
+-----------------------------------------------------+
```

【响应】

```
Response => len: 32 bytes, version: 9A, Error: 00
+-----------------------------------------------------+
|  CF=1  | PM1.0 = 5    | PM2.5 = 6    | PM10  = 9    |
|  atm.  | PM1.0 = 6    | PM2.5 = 7    | PM10  = 10   |
|        | 0.3um = 1065 | 0.5um = 331  | 1.0um = 33   |
|        | 2.5um = 3    | 5.0um = 3    | 10um  = 3    |
+-----------------------------------------------------+
```

【原始响应数据】

```
42 4d 00 1c 00 03 00 04 00 07 00 04 00 05 00 08 03 1b 00 f9 00 1b 00 06 00 03 00 03 9a 00 02 a2
```



### 4.2 Sensor 接口

pmsxx 软件包已对接 sensor 驱动框架，操作传感器模块之前，只需调用下面接口注册传感器设备即可。

```c
rt_err_t rt_hw_pms_init(const char *name, struct rt_sensor_config *cfg);
```

| 参数      | 描述            |
| --------- | --------------- |
| name      | 传感器设备名称  |
| cfg       | sensor 配置信息 |
| **返回**  | ——              |
| RT_EOK    | 创建成功        |
| -RT_ERROR | 创建失败        |

#### 初始化示例

```c
#define PMSXX_UART_NAME      "uart3"

static int rt_hw_pms_port(void)
{
    struct rt_sensor_config cfg;
    
    cfg.intf.type = RT_SENSOR_INTF_UART;
    cfg.intf.dev_name = PMSXX_UART_NAME;
    rt_hw_pms_init("pms", &cfg);
    
    return RT_EOK;
}
INIT_COMPONENT_EXPORT(rt_hw_pms_port);
```



#### 读取示例

由于攀藤 PMS 系列传感器输出的数据中包含多种不同粒径的悬浮颗粒物的浓度和单位体积内的数量等数据。考虑 PM2.5 指标较为重要，因此使用 RT-Thread 设备驱动接口 rt_device_read 读取数据时，默认返回的是大气环境中的 PM2.5 浓度。

```c
static void pms_read_sample(void)
{
    struct rt_sensor_data sensor_data;

    rt_device_t dev = rt_device_find("uart3");
    rt_device_open(dev, RT_DEVICE_FLAG_RDWR);
    
    rt_device_read(dev, 0, &sensor_data, 1);
    rt_kprintf("PM2.5: %d ug/m3\n", sensor_data.data.dust);

    rt_device_close(dev);
}
```

如需使用更多数据，请使用 rt_device_control 接口，命令字为 `RT_SENSOR_CTRL_PMS_DUMP`。

```c
static void pms_dump_sample(void)
{
    struct pms_response resp;
    struct rt_sensor_data sensor_data;

    rt_device_t dev = rt_device_find("uart3");
    rt_device_open(dev, RT_DEVICE_FLAG_RDWR);
    
    rt_device_control(dev, RT_SENSOR_CTRL_PMS_DUMP, &resp);
    pms_show_response(&resp);

    rt_device_close(dev);
}
```



## 5、注意事项

1. 为传感器对象提供静态创建和动态创建两种方式，如果使用动态创建，请记得在使用完毕释放对应的内存空间。
2. PMS 系列传感器上电默认进入主动式数据传输模式，如需使用被动式（主机轮询），请在传感器初始化后切换工作模式。
3. PMS 系列传感器的增强版支持采集额外的环境数据，比如温度、湿度、甲醛浓度等，相关兼容工作将在后续进行。



## 6、相关文档

见 docs 目录。



## 7、联系方式

- 维护：luhuadong@163.com
- 主页：<https://github.com/luhuadong/rtt-pmsxx>

