#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
同态滤波实验程序
实现了实验指导中的同态滤波处理流程
"""

import os
import argparse
import numpy as np
import cv2
import matplotlib.pyplot as plt
from matplotlib.colors import LogNorm


class HomomorphicFilter:
    """同态滤波器类，用于处理图像中的乘性噪声"""
    
    def __init__(self):
        """初始化同态滤波器"""
        self.img = None
        self.filtered_img = None
        self.spectrum = None
        self.filtered_spectrum = None
        self.filter_mask = None
    
    def load_image(self, img_path):
        """加载图像"""
        if not os.path.exists(img_path):
            raise FileNotFoundError(f"图像文件 {img_path} 不存在")
        
        self.img = cv2.imread(img_path, cv2.IMREAD_GRAYSCALE)
        if self.img is None:
            raise ValueError(f"无法加载图像 {img_path}")
        
        self.img = self.img.astype(np.float32) / 255.0
    
    def log_transform(self, epsilon=1e-6):
        """对数变换，将乘性噪声转换为加性噪声"""
        if self.img is None:
            raise ValueError("未加载图像")
        
        return np.log(self.img + epsilon)
    
    def fft_transform(self, log_img):
        """对对数变换后的图像执行快速傅里叶变换(FFT)"""
        f_transform = np.fft.fft2(log_img)
        f_shift = np.fft.fftshift(f_transform)
        self.spectrum = np.abs(f_shift)
        return f_shift, self.spectrum
    
    def create_filter(self, shape, cutoff, gamma_l=0.5, gamma_h=2.0):
        """创建高斯同态滤波器"""
        rows, cols = shape
        crow, ccol = rows // 2, cols // 2
        
        # 使用numpy的ogrid创建网格，更高效
        y, x = np.ogrid[-crow:rows-crow, -ccol:cols-ccol]
        d_squared = x*x + y*y
        
        # 创建滤波器掩码
        mask = (gamma_h - gamma_l) * (1 - np.exp(-d_squared / (2 * cutoff**2))) + gamma_l
        self.filter_mask = mask
        return mask
    
    def apply_filter(self, f_shift, cutoff, gamma_l, gamma_h):
        """应用滤波器到频域图像"""
        mask = self.create_filter(f_shift.shape, cutoff, gamma_l, gamma_h)
        self.filtered_spectrum = f_shift * mask
        return self.filtered_spectrum
    
    def inverse_fft(self, filtered_spectrum):
        """进行逆傅里叶变换恢复图像"""
        f_ishift = np.fft.ifftshift(filtered_spectrum)
        img_back = np.fft.ifft2(f_ishift)
        return np.real(img_back)
    
    def exp_transform(self, ifft_img):
        """执行指数变换，复原对数变换"""
        img_exp = np.exp(ifft_img)
        
        # 归一化并增强对比度
        img_normalized = (img_exp - np.min(img_exp)) / (np.max(img_exp) - np.min(img_exp))
        img_adjusted = np.power(img_normalized, 0.1)  # gamma校正
        self.filtered_img = np.clip((img_adjusted - 0.2) * 1.5, 0, 1)  # 对比度增强
        
        return self.filtered_img
    
    def process_image(self, img_path, cutoff=40.0, gamma_l=0.5, gamma_h=2.0):
        """执行完整的同态滤波处理流程"""
        # 加载图像
        self.load_image(img_path)
        
        # 执行处理流程
        log_img = self.log_transform()
        f_shift, _ = self.fft_transform(log_img)
        filtered_spectrum = self.apply_filter(f_shift, cutoff, gamma_l, gamma_h)
        ifft_img = self.inverse_fft(filtered_spectrum)
        result = self.exp_transform(ifft_img)
        
        return result
    
    def save_result(self, output_path):
        """保存处理结果"""
        if self.filtered_img is None:
            raise ValueError("没有处理结果可保存")
        
        # 确保输出目录存在
        output_dir = os.path.dirname(output_path)
        if output_dir and not os.path.exists(output_dir):
            os.makedirs(output_dir)
            
        # 保存结果图像
        img_uint8 = (self.filtered_img * 255).astype(np.uint8)
        cv2.imwrite(output_path, img_uint8)
        
        # 保存对比图
        self.save_comparison(os.path.splitext(output_path)[0] + "_comparison.png")
    
    def save_comparison(self, output_path):
        """保存原图和处理图的对比图"""
        if self.img is None or self.filtered_img is None:
            raise ValueError("未处理图像，无法保存对比图")
        
        # 创建对比图像
        h, w = self.img.shape
        comparison = np.zeros((h, w*2), dtype=np.float32)
        comparison[:, :w] = self.img
        comparison[:, w:] = self.filtered_img
        comparison[:, w-1:w+1] = 1.0  # 添加分隔线
        
        # 保存对比图
        cv2.imwrite(output_path, (np.clip(comparison, 0, 1) * 255).astype(np.uint8))
    
    def visualize_process(self):
        """可视化处理过程"""
        if self.img is None or self.filtered_img is None:
            raise ValueError("未处理图像，无法可视化结果")
        
        # 创建图像布局
        fig, axes = plt.subplots(2, 3, figsize=(15, 8))
        
        # 准备数据
        log_img = self.log_transform()
        
        # 绘制处理步骤
        axes[0, 0].imshow(self.img, cmap='gray')
        axes[0, 0].set_title('1. 原始图像')
        axes[0, 0].axis('off')
        
        axes[0, 1].imshow(log_img, cmap='gray')
        axes[0, 1].set_title('2. 对数变换')
        axes[0, 1].axis('off')
        
        if self.spectrum is not None:
            axes[0, 2].imshow(np.log1p(self.spectrum), cmap='viridis', norm=LogNorm())
            axes[0, 2].set_title('3. 频谱图')
            axes[0, 2].axis('off')
        
        if self.filter_mask is not None:
            axes[1, 0].imshow(self.filter_mask, cmap='jet')
            axes[1, 0].set_title('4. 滤波器掩码')
            axes[1, 0].axis('off')
        
        if self.filtered_spectrum is not None:
            axes[1, 1].imshow(np.log1p(np.abs(self.filtered_spectrum)), cmap='viridis', norm=LogNorm())
            axes[1, 1].set_title('5. 滤波后频谱')
            axes[1, 1].axis('off')
        
        axes[1, 2].imshow(self.filtered_img, cmap='gray')
        axes[1, 2].set_title('6. 处理后图像')
        axes[1, 2].axis('off')
        
        # 保存并显示结果
        plt.tight_layout()
        os.makedirs("images", exist_ok=True)
        plt.savefig("images/process_result.png", dpi=150)
        plt.show()
        
        # 显示原图和处理图对比
        self.show_comparison()
    
    def show_comparison(self):
        """显示原图和处理后图像的对比"""
        plt.figure(figsize=(12, 6))
        plt.subplot(1, 2, 1)
        plt.imshow(self.img, cmap='gray')
        plt.title('原始图像')
        plt.axis('off')
        
        plt.subplot(1, 2, 2)
        plt.imshow(self.filtered_img, cmap='gray')
        plt.title('处理后图像')
        plt.axis('off')
        
        plt.tight_layout()
        plt.savefig("images/comparison.png", dpi=150)
        plt.show()


def parse_args():
    """解析命令行参数"""
    parser = argparse.ArgumentParser(description='同态滤波处理程序')
    
    parser.add_argument('-i', '--input', type=str, default="images/test.jpg",
                        help='输入图像路径 (默认: images/test.jpg)')
    parser.add_argument('-o', '--output', type=str, default='images/result.png',
                        help='输出图像路径 (默认: images/result.png)')
    parser.add_argument('-c', '--cutoff', type=float, default=40.0,
                        help='高斯滤波器的截止频率 (默认: 40.0)')
    parser.add_argument('-gl', '--gamma_l', type=float, default=0.5,
                        help='同态滤波器低频增益 (默认: 0.5)')
    parser.add_argument('-gh', '--gamma_h', type=float, default=2.0,
                        help='同态滤波器高频增益 (默认: 2.0)')
    
    return parser.parse_args()


def main():
    """主函数"""
    args = parse_args()
    os.makedirs("images", exist_ok=True)
    
    try:
        # 创建并运行同态滤波器
        hf = HomomorphicFilter()
        hf.process_image(
            args.input, 
            cutoff=args.cutoff,
            gamma_l=args.gamma_l,
            gamma_h=args.gamma_h
        )
        
        # 保存结果并显示
        hf.save_result(args.output)
        hf.visualize_process()
        print(f"处理完成！结果已保存至 {args.output}")
            
    except Exception as e:
        print(f"处理过程中出现错误: {e}")


if __name__ == "__main__":
    main() 