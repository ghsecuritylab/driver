# STM32H743_OPC

####  项目介绍

按键控制模块

1、集控系统通讯

2、按键数据采集

3、数据存储

RTOS: RT-Thread 

上海创力集团股份有限公司

## 自动控制模块板载资源介绍介绍

- MCU：STM32H743VIT6，主频 400MHz，2MB FLASH ，1MB RAM
- 调试接口：SW

## 外设驱动支持

目前对外设的支持情况如下：

| **驱动**    | **支持情况** | **备注**                              |
| :----------------- | :----------: | :------------------------------------- |
| USART1  |     支持     | FINSH接口 |
| LCD     |   支持   | 挂载SPI1 分辨率240*240 |
| SD CARD     |   暂不支持   | |
| CANOPEN         |   暂不支持   ||
| GPIO              |     支持     | PA0, PA1... PE15 ---> PIN: 0, 1...100 |
| UART              |     支持     | USART3|
| SPI               |   支持   | |
| RTC               |     暂不支持     ||
| PWM               |   支持   ||
| IWG               |     暂不支持     |  |
| FLASH    |     支持     | 挂载SPI4 文件系统路径 “/” |
| FRAM |     支持     | 挂载SPI4 可通过fram_sample测试 |
| AD7739 |     支持     | 挂载SPI1 |


#### 编译下载

双击 project.uvprojx 文件，打开 MDK5 工程，编译并下载程序到开发板。

> 工程默认配置使用 ST_LINK 仿真器下载程序，在通过 ST_LINK 连接开发板的基础上，点击下载按钮即可下载程序到开发板

#### 运行结果

下载程序成功之后，系统会自动运行，LED闪烁。

连接开发板对应串口到 PC , 在终端工具里打开相应的串口（115200-8-1-N），复位设备后，可以看到 RT-Thread 的输出信息:

```bash
 \ | /
- RT -     Thread Operating System
 / | \     4.0.1 build Mar 5 2019
 2006 - 2019 Copyright by rt-thread team
 
+---+____.----------------.____+---+
| X |____|       CL       |____| X |
+---+    |__@__________@__|    +---+
    Intelligence Control System
      V1.0.0 build Mar 5 2019
 2019 Copyright by shanghai chuangli group
 
msh >
```

