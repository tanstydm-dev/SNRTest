# SNRTest - 导航接收机数据处理测试程序

![Language](https://img.shields.io/badge/Language-C%2B%2B-blue.svg)
![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey.svg)
![Framework](https://img.shields.io/badge/Framework-MFC-orange.svg)

这是一个基于 C++ 和 MFC 框架开发的数据测试桌面应用程序。本项目主要用于GNSS导航接收机数据可视化以及自动化测试流程，代替传统串口助手测试，为相关技术人员提供高效、直观的测试工具。

---

## 🎥 软件运行演示

通过串口连接接收机输入NMEA实时数据，或读取本地NMEA文件进行数据回放，本项目提供了一个NMEA-Test.dat文件用于测试。

![](D:\OpenPrj\输入.png)



*此处为程序的核心功能动态演示，直观展现软件的交互界面与执行结果：*

![软件运行演示](D:\OpenPrj\demo.gif)

---

## ✨ 核心功能亮点

- **直观的图形用户界面 (GUI)**：基于经典 MFC 架构搭建，操作简便，响应迅速。
- **动态路径自适应**：程序内部采用动态绝对路径拼接算法，无论如何双击或快捷方式启动，均能精准识别同级目录下的配置文件。
- **模块化代码设计**：核心逻辑与 UI 界面深度解耦，便于后续二次开发与算法升级（如 `NavMsg`、`PtlProc` 等核心模块）。
- **轻量化工程**：已通过严格的 `.gitignore` 过滤，剔除了所有编译垃圾（如 `x64`、`obj`、`.vs` 等），下载即可秒级编译。

---

## 🛠️ 技术栈与开发环境

- **开发语言**：C++ (ISO C++14 或更高)
- **开发工具**：Visual Studio 2022 / 2019
- **操作系统**：Windows 10 / 11 (64-bit)
- **核心框架**：Microsoft Foundation Classes (MFC)

---

## 🚀 如何在本地运行与编译

针对**技术面试官/开发者**，可通过以下步骤在本地部署并运行本项目：

### 1. 克隆/下载本仓库
```bash
git clone [https://github.com/tanstydm-dev/SNRTest.git](https://github.com/tanstydm-dev/SNRTest.git)

2. 使用 Visual Studio 打开
打开 Visual Studio。

点击“打开项目或解决方案”，选择项目根目录下的 UTest.sln（或相应的解决方案文件）。

3. 编译与运行
将编译配置调整为 Release / x64（或您需要的对应架构）。

点击键盘 Ctrl + F5 即可直接运行程序。

⚠️ 注意：请确保将res文件夹放置在程序最终生成的 .exe 的同级目录下。

📦 针对 HR / 测试人员（免安装一键运行）
如果您不是开发者，或当前电脑未安装 Visual Studio，您无需编译代码：

点击本仓库页面右侧的 [Releases] 标签。

下载最新发布的 UTest_v1.0.zip 压缩包。

解压到本地任意文件夹，双击文件夹内的 .exe 文件即可直接启动体验。

文件夹结构简述
├── UTest/               # 主要源代码与工程目录
│   ├── UTest.sln       # vs工程入口文件
│   └── UTest/           # MFC 界面资源与窗体类、核心算法与业务逻辑代码
├── .gitignore           # Git 忽略文件规则
├── demo.gif             # 软件运行效果动图
└── README.md            # 本说明文档

