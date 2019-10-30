# F743_OPC BSP 说明

## 简介
 - 工程基于STM32F743VIT6芯片，使用RT-Thread实时操作系统
 - HES : 25MHz 有源晶振
 - LSE : 32.768kHz 石英晶振
 
## 硬件范围
 - AD7739芯片，SPI通信方式，挂载SPI1
 - W25Q256FVEIG芯片，SPI通信方式,挂载SPI4
 - FM25CL64-G芯片，SPI通信方式，挂载SPI4
 - LCD模块，SPI通信，挂载SPI1
 - 调试接口 RS422接口
 
## 特殊说明
  SPI1接口使用了复用功能
