import cv2
import numpy as np
import matplotlib.pyplot as plt
import tkinter as tk
from tkinter import filedialog

def 读取灰度图像(image_path):
    """读取图像并转换为灰度图"""
    # 读取图像
    img = cv2.imread(image_path)
    if img is None:
        raise FileNotFoundError(f"错误：无法读取图像 {image_path}")
    
    # 转换为灰度图
    if len(img.shape) == 2 or img.shape[2] == 1:  # 已经是灰度图
        gray_img = img.copy()
        original_img = img.copy()
    else:  # 彩色图
        gray_img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        original_img = img.copy()
    
    return gray_img, original_img

def 计算直方图(gray_img):
    """计算灰度图像的直方图"""
    # 计算直方图
    hist = cv2.calcHist([gray_img], [0], None, [256], [0, 256])
    hist = hist.flatten()  # 将直方图展平为一维数组
    
    # 归一化直方图
    total_pixels = gray_img.shape[0] * gray_img.shape[1]
    normalized_hist = hist / total_pixels
    
    return hist, normalized_hist

def 大津法计算阈值(gray_img, hist_counts):
    """使用大津法计算最佳阈值"""
    total_pixels = gray_img.shape[0] * gray_img.shape[1]
    normalized_hist = hist_counts / total_pixels
    
    max_sigma_b_squared = -1
    optimal_threshold = 0
    sigma_b_squared_values = np.zeros(256)
    
    # 忽略灰度频率低于10的灰度值
    valid_bins = np.where(hist_counts >= 10)[0]
    if len(valid_bins) == 0:
        min_gray_level = 0
        max_gray_level = 255
    else:
        min_gray_level = valid_bins[0]
        max_gray_level = valid_bins[-1]
    
    # 遍历所有可能的阈值 T
    for t in range(min_gray_level, max_gray_level + 1):
        # 背景 (灰度值 <= t)
        w0 = np.sum(normalized_hist[0:t+1])
        if w0 > 0:
            mu0 = np.sum(np.arange(0, t+1) * normalized_hist[0:t+1]) / w0
        else:
            mu0 = 0
        
        # 前景 (灰度值 > t)
        w1 = np.sum(normalized_hist[t+1:256])
        if w1 > 0:
            mu1 = np.sum(np.arange(t+1, 256) * normalized_hist[t+1:256]) / w1
        else:
            mu1 = 0
        
        # 类间方差 σB² = w0 * w1 * (μ0 - μ1)²
        sigma_b_squared = w0 * w1 * ((mu0 - mu1) ** 2)
        sigma_b_squared_values[t] = sigma_b_squared
        
        if sigma_b_squared > max_sigma_b_squared:
            max_sigma_b_squared = sigma_b_squared
            optimal_threshold = t
    
    return optimal_threshold, sigma_b_squared_values

def 二值化图像(gray_img, threshold):
    """根据阈值对图像进行二值化"""
    # 灰度值 >= threshold 的像素设为255（白色，前景）
    # 灰度值 < threshold 的像素设为0（黑色，背景）
    binary_img = np.where(gray_img >= threshold, 255, 0).astype(np.uint8)
    return binary_img

def 显示结果(original_img, gray_img, hist, normalized_hist, 
           sigma_b_squared_values, threshold, binary_img):
    """显示原图、灰度图、直方图、类间方差曲线和二值化结果"""
    # 解决中文显示问题
    import matplotlib
    matplotlib.rcParams['font.sans-serif'] = ['YouYuan', 'Microsoft YaHei', 'SimSun', 'KaiTi', 'FangSong']  # 优先使用的中文字体
    matplotlib.rcParams['axes.unicode_minus'] = False  # 解决负号显示问题
    
    # 检查系统中是否有支持中文的字体
    import platform
    if platform.system() == 'Windows':
        # Windows系统一般有中文字体
        pass  
    else:
        # 在Linux/Mac系统中，如果没有中文字体，尝试使用其他通用字体
        import matplotlib.font_manager as fm
        fonts = [f.name for f in fm.fontManager.ttflist]
        print("可用字体:", fonts[:5], "等...")  # 只显示前几个字体，避免输出过多
        # 在非Windows系统中尝试使用通用字体
        if not any(font in fonts for font in ['SimHei', 'Microsoft YaHei']):
            matplotlib.rcParams['font.sans-serif'] = ['DejaVu Sans', 'Arial Unicode MS', 'Noto Sans CJK JP'] + matplotlib.rcParams['font.sans-serif']
            
    # 设置漂亮的绘图风格
    plt.style.use('ggplot')
    
    fig, axes = plt.subplots(2, 3, figsize=(15, 8))
    fig.suptitle("大津法图像二值化实验", fontsize=16)
    
    # 原始图像
    if len(original_img.shape) == 3:
        axes[0, 0].imshow(cv2.cvtColor(original_img, cv2.COLOR_BGR2RGB))
    else:
        axes[0, 0].imshow(original_img, cmap='gray')
    axes[0, 0].set_title("原始图像")
    axes[0, 0].axis('off')
    
    # 灰度图像
    axes[0, 1].imshow(gray_img, cmap='gray')
    axes[0, 1].set_title("灰度图像")
    axes[0, 1].axis('off')
    
    # 直方图
    axes[0, 2].plot(hist)
    axes[0, 2].set_title("灰度直方图")
    axes[0, 2].set_xlabel("灰度值")
    axes[0, 2].set_ylabel("像素数量")
    axes[0, 2].set_xlim([0, 255])
    
    # 归一化直方图
    axes[1, 0].plot(normalized_hist)
    axes[1, 0].set_title("归一化直方图")
    axes[1, 0].set_xlabel("灰度值")
    axes[1, 0].set_ylabel("概率")
    axes[1, 0].set_xlim([0, 255])
    
    # 类间方差曲线
    axes[1, 1].plot(sigma_b_squared_values)
    axes[1, 1].axvline(x=threshold, color='r', linestyle='--', 
                     label=f'最佳阈值: {threshold}')
    axes[1, 1].set_title("类间方差曲线")
    axes[1, 1].set_xlabel("阈值")
    axes[1, 1].set_ylabel("类间方差")
    axes[1, 1].legend()
    axes[1, 1].set_xlim([0, 255])
    
    # 二值化图像
    axes[1, 2].imshow(binary_img, cmap='gray')
    axes[1, 2].set_title(f"二值化图像 (阈值 = {threshold})")
    axes[1, 2].axis('off')
    
    plt.tight_layout(rect=[0, 0, 1, 0.96])
    plt.show()

def 选择图像文件():
    """打开文件对话框让用户选择图像文件"""
    root = tk.Tk()
    root.withdraw()  # 隐藏主窗口
    file_path = filedialog.askopenfilename(
        title="选择图像文件",
        filetypes=[("图像文件", "*.jpg *.jpeg *.png *.bmp *.tiff *.gif"),
                   ("所有文件", "*.*")]
    )
    root.destroy()
    return file_path

# 主程序
if __name__ == '__main__':
    # 选择图像文件
    image_path = 选择图像文件()
    
    if not image_path:
        print("未选择图像，程序退出")
    else:
        try:
            print(f"正在处理图像: {image_path}")
            
            # 1. 读取图像并转为灰度图
            gray_img, original_img = 读取灰度图像(image_path)
            print("图像已转换为灰度图")
            
            # 2. 计算灰度直方图
            hist, normalized_hist = 计算直方图(gray_img)
            print("灰度直方图已计算")
            
            # 3. 计算最佳阈值
            threshold, sigma_b_squared_values = 大津法计算阈值(gray_img, hist)
            print(f"大津法计算的最佳阈值为: {threshold}")
            
            # 4. 图像二值化
            binary_img = 二值化图像(gray_img, threshold)
            print("图像已二值化")
            
            # 5. 显示结果
            显示结果(original_img, gray_img, hist, normalized_hist,
                 sigma_b_squared_values, threshold, binary_img)
            
            # 对比OpenCV内置的OTSU方法
            ret, opencv_otsu = cv2.threshold(gray_img, 0, 255, cv2.THRESH_BINARY + cv2.THRESH_OTSU)
            print(f"OpenCV OTSU计算的阈值为: {ret}")
            
        except Exception as e:
            print(f"处理过程中出错: {e}") 