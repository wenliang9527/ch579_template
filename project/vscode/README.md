# CH579 VSCode 项目模板

基于 CMSIS 的 CH579 微控制器开发 VSCode 项目模板。

## 项目结构

```
vscode/
├── .cmsis/                    # ARM CMSIS 设备代码
│   ├── device/                # 设备启动代码
│   │   └── ARM/              # 支持的 ARM 内核
│   │       ├── ARMCM0/       # Cortex-M0
│   │       ├── ARMCM0plus/   # Cortex-M0+
│   │       ├── ARMCM1/       # Cortex-M1
│   │       ├── ARMCM3/       # Cortex-M3
│   │       ├── ARMCM4/       # Cortex-M4 (带 DSP)
│   │       ├── ARMCM7/       # Cortex-M7
│   │       ├── ARMCM23/      # Cortex-M23
│   │       ├── ARMCM33/      # Cortex-M33
│   │       ├── ARMCM35P/     # Cortex-M35P
│   │       ├── ARMCM55/      # Cortex-M55
│   │       ├── ARMCM85/      # Cortex-M85
│   │       ├── ARMCA5/       # Cortex-A5
│   │       ├── ARMCA7/       # Cortex-A7
│   │       ├── ARMCA9/       # Cortex-A9
│   │       ├── ARMv8MBL/     # ARMv8-M Baseline
│   │       ├── ARMv8MML/     # ARMv8-M Mainline
│   │       ├── ARMv81MML/    # ARMv8.1-M MVE
│   │       ├── ARMSC000/     # SC000
│   │       └── ARMSC300/     # SC300
│   └── include/              # CMSIS 核心头文件
├── .clang-format              # C/C++ 代码格式化配置
├── .gitignore                 # Git 忽略文件配置
└── project.code-workspace     # VSCode 工作区配置
```

## 支持的编译器

| 编译器 | 目录 |
|--------|------|
| ARM Compiler 5 (AC5) | Source/AC5, Source/ARM |
| ARM Compiler 6 (AC6) | Source/AC6, Source/ARM |
| GCC (ARM GCC) | Source/GCC |
| IAR Embedded Workbench | Source/IAR |

## 配置说明

### .clang-format

代码格式化配置，基于 Microsoft 风格：
- 缩进宽度: 4 空格
- 不使用 Tab
- 列限制: 无限制

### .gitignore

配置忽略以下文件：
- `.vscode/launch.json` - 调试配置
- `.settings/` - IDE 设置
- `.eide/log` - EIDE 日志
- `.eide.usr.ctx.json` - EIDE 用户上下文
- `/build`, `/bin`, `/obj`, `/out` - 编译输出目录
- `*.ept`, `*.eide-template` - EIDE 模板文件

## 使用说明

1. 将此模板作为 CH579 项目的起点
2. 根据目标芯片选择合适的 CMSIS 启动代码
3. 配置编译器工具链
4. 在 VSCode 中打开 `project.code-workspace` 开始开发

## 相关链接

- [CH579 数据手册](http://www.wch.cn/products/CH579.html)
- [ARM CMSIS 文档](https://arm-software.github.io/CMSIS_5/)