# 实验二、循环码编译码算法仿真

本项目旨在根据"实验指导.md"的要求，使用 MATLAB 实现循环码的编译码、调制解调以及整个通信链路的仿真。

## 项目结构

计划创建以下 MATLAB 文件来实现各项功能：

- `cyclic_encoder.m`: 实现循环码的编码器。
- `cyclic_decoder.m`: 实现循环码的译码器。
- `modulator.m`: 实现调制功能（例如 BPSK）。
- `demodulator.m`: 实现解调功能（例如 BPSK）。
- `channel_simulator.m`: 模拟信道特性（例如 AWGN 信道）。
- `main_simulation.m`: 主脚本，用于整合所有模块，运行仿真，并绘制性能曲线图。
- `README.md`: 项目说明文件。
- `实验指导.md`: 原始实验指导。
- `纠错码实验2（交电子版+纸质版）.doc`: 原始实验指导（与 .md 内容一致）。

## 默认参数

在没有特定指示的情况下，将使用以下默认参数进行仿真：

- **循环码**: (7,4) 汉明码，生成多项式 g(x) = x^3 + x + 1 (二进制表示为 `[1 0 1 1]`)。
- **调制方式**: BPSK (二进制相移键控)。
- **信道模型**: AWGN (加性高斯白噪声) 信道。

## 如何运行

1.  打开 MATLAB。
2.  将工作目录更改为本项目所在的文件夹。
3.  运行 `main_simulation.m` 脚本。

仿真结果（ BER vs Eb/N0 曲线）将会显示在 MATLAB 的图形窗口中。
