# rtt-pmsxx
Plantower PMSxx family sensor driver for RT-Thread



## 1、介绍

pmsxx 软件包适用于攀藤 PMS 系列数字式通用颗粒物传感器模块，支持 PMS1003、PMS3003、PMS5003、PMS7003、PMS9003、PMSA003 等多种型号传感器。

PMS 系列传感器基于激光散射原理，可连续采集并计算单位体积内空气中不同粒径的悬浮颗粒物浓度，并通过串口输出数据，比如 PM1.0、PM2.5、PM10 浓度等数据。



### 1.1 特性

- 支持多实例。
- 支持静态和动态分配内存。
- 支持 sensor 设备驱动框架。
- 线程安全。



### 1.2 工作模式

| 设备         | 空气颗粒物浓度 |
| :----------- | :------------- |
| **通信接口** |                |
| UART          | √              |
| **工作模式** |                |
| 主动式（轮询）   | √              |
| 被动式（中断）    | √ |



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
- 使用动态创建方式需要开启动态内存管理模块
- 使用 sensor 设备接口需要开启 sensor 设备驱动框架模块



## 2、获取 pmsxx 软件包

使用 pmsxx package 需要在 RT-Thread 的包管理器中选择它，具体路径如下：

```
RT-Thread online packages --->
    peripheral libraries and drivers --->
        [*] sensors drivers  --->
            [*] PMSxx: Plantower PMSxx family sensor driver.
```

然后让 RT-Thread 的包管理器自动更新，或者使用 `pkgs --update` 命令更新包到 BSP 中。



## 3、使用 pmsxx 软件包

### 3.1 版本说明

| 版本   | 说明                         |
| ------ | ---------------------------- |
| v0.8.0 | 支持自定义接口和 sensor 框架 |
| latest | 进一步优化                   |

目前处于公测阶段，建议开发者使用 latest 版本。



### 3.2 配置选项



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

更新后的数据将保存在 dev 对象的 resp 结构体中。

```c
rt_bool_t pms_measure(pms_device_t dev);
```

| 参数     | 描述           |
| -------- | -------------- |
| dev      | 传感器对象句柄 |
| **返回** | ——             |
| RT_TRUE  | 读取成功       |
| RT_FALSE | 读取失败       |



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



## 5、注意事项

1. 为传感器对象提供静态创建和动态创建两种方式，如果使用动态创建，请记得在使用完毕释放对应的内存空间。
2. PMS 系列传感器上电默认进入主动式数据传输模式，如需使用被动式（主机轮询），请在传感器初始化后切换工作模式。
3. PMS 系列传感器的增强版支持采集额外的环境数据，比如温度、湿度、甲醛浓度等，相关兼容工作将在后续进行。



## 6、相关文档

见 docs 目录。



## 7、联系方式

- 维护：luhuadong@163.com
- 主页：<https://github.com/luhuadong/rtt-pmsxx>

