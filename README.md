# CH579 Template Project

基于 WCH CH579M 微控制器的嵌入式开发模板项目。

## 硬件规格

- **芯片**: CH579M
- **架构**: ARM Cortex-M0
- **供应商**: WCH (武汉光环)
- **时钟**: 外部晶振 PLL 输出 32MHz
- **Flash**: 256KB
- **RAM**: 32KB

## 功能特性

### 通信接口
- **以太网**: 内置 10/100M Ethernet 控制器 (CH57xNET)
- **串口**: UART0, UART1, UART2, UART3 (支持多串口通信)

### 网络功能
- TCP Server (支持最多 4 个客户端连接)
- DHCP 自动获取 IP
- DNS 解析

### 外设
- GPIO
- 定时器 (Timer0, Timer1, Timer2, Timer3)
- SPI
- PWM
- ADC
- USB Device
- USB Host
- LCD

## 项目结构

```
ch579_template/
├── library/                    # 外设驱动库
│   ├── CMSIS/                  # ARM CMSIS 核心文件
│   ├── StdPeriphDriver/       # WCH 标准外设驱动
│   │   ├── inc/                # 头文件
│   │   └── *.c                 # 驱动源文件
│   ├── sct/                   # 链接脚本
│   └── net_library/           # 网络库 (CH57xNET)
├── mem/                       # 内存管理
│   ├── BufferManage.c/h       # 缓冲区管理
│   └── LoopList.c/h           # 循环链表
├── net/                       # 网络功能
│   ├── net_init.c/h           # 网络初始化
│   ├── net_dhcp.c/h           # DHCP 客户端
│   ├── net_dns.c/h            # DNS 解析
│   ├── net_tcp_client.c/h     # TCP 客户端
│   └── net_tcp_server.c/h     # TCP 服务器
├── user/                      # 用户应用代码
│   ├── main.c                 # 主程序入口
│   ├── uart.c/h               # 串口驱动
│   ├── time.c/h               # 定时器
│   ├── led.c/h                # LED 控制
│   ├── sys_tick.c/h           # 系统滴答
│   └── main.h                 # 主头文件
└── project/                   # Keil uVision 工程文件
    ├── project.uvprojx        # 工程文件
    ├── Objects/               # 编译输出
    └── Listings/              # 列表文件
```

## 快速开始

### 开发环境
- Keil uVision 5
- ARM MDK

### 编译

1. 使用 Keil uVision 打开 `project/project.uvprojx`
2. 选择目标芯片 CH579M
3. 编译项目 (F7)

### 烧录

支持以下烧录工具：
- J-Link
- WCH-Link
- CH579 专用烧录器

## TCP Server 使用说明

### 配置

在 `user/main.c` 中配置服务器参数：

```c
net_tcp_server.port_local = 6666;  // 设置监听端口
net_tcp_server_creat_socket(&net_tcp_server, 
    tcp_server_connected_back,      // 连接成功回调
    tcp_server_disconnected_back,   // 断开连接回调
    tcp_server_recv_back);           // 数据接收回调
```

### 回调函数示例

```c
// 接收到数据回调
void tcp_server_recv_back(socket_client *socket, unsigned char* data, unsigned long length)
{
    // 处理接收到的数据
    for (uint16_t i = 0; i < length; i++) {
        uart_send_byte(USART_1_IDNumber, data[i]);
    }
}

// 客户端连接回调
void tcp_server_connected_back(socket_client *socket_client_value)
{
    // 记录连接的客户端
}

// 客户端断开回调
void tcp_server_disconnected_back(socket_client *socket_client_value)
{
    // 处理客户端断开
}
```

### 发送数据

```c
unsigned char target_ip[4] = {192, 168, 1, 100};
tcp_send(target_ip, 8888, data, data_len);
```

## 串口使用说明

### 可用串口

| 串口 | TX 引脚 | RX 引脚 |
|------|---------|---------|
| UART0 | PA14 | PA15 |
| UART1 | PA9 | PA8 |
| UART2 | PA7 | PA6 |
| UART3 | PB21 | PB20 |

### 初始化

```c
uart_init();  // 初始化所有串口
```

### 发送数据

```c
// 发送单个字节
uart_send_byte(USART_1_IDNumber, 'A');

// 发送字符串
uart_send_bytes(USART_1_IDNumber, "Hello", 5);
```

## 主循环

主程序在 `user/main.c` 的 `main()` 函数中：

```c
int main()
{
    PWR_UnitModCfg(ENABLE, UNIT_SYS_PLL);  // 使能 PLL
    SetSysClock(CLK_SOURCE_HSE_32MHz);      // 配置时钟
    
    uart_init();     // 串口初始化
    net_init();      // 以太网初始化
    time0_init();    // 定时器初始化
    
    // 创建 TCP 服务器
    net_tcp_server.port_local = 6666;
    net_tcp_server_creat_socket(...);
    
    while (1) {
        CH57xNET_MainTask();           // 网络库主任务
        if (CH57xNET_QueryGlobalInt()) {
            CH57xNET_HandleGlobalInt(); // 处理网络事件
        }
    }
}
```

## 注意事项

1. 以太网功能需要使能 PLL 时钟 (`PWR_UnitModCfg`)
2. TCP 服务器默认监听端口为 6666
3. 网络库需要在主循环中不断调用 `CH57xNET_MainTask()`
4. 使用串口时注意引脚复用配置

## 联系方式

- 官网: http://wch.cn
- 技术支持: tech@wch.cn