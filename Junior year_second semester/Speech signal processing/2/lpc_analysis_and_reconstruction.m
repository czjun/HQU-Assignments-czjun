% LPC频谱分析、共振峰识别与语音重构
clear all; close all; clc;

% 加载音频文件
% 第一组：实验一的素材（真实语音和深度学习生成的语音）
[true_speech, Fs1] = audioread('true.wav');
[synth_speech, Fs2] = audioread('study.wav');

% 第二组：元音素材
[A1, FsA1] = audioread('A1.wav');
[A2, FsA2] = audioread('A2.wav');
[E1, FsE1] = audioread('E1.wav');
[E2, FsE2] = audioread('E2.wav');

% 确保所有音频都是单通道
if size(true_speech, 2) > 1, true_speech = true_speech(:, 1); end
if size(synth_speech, 2) > 1, synth_speech = synth_speech(:, 1); end
if size(A1, 2) > 1, A1 = A1(:, 1); end
if size(A2, 2) > 1, A2 = A2(:, 1); end
if size(E1, 2) > 1, E1 = E1(:, 1); end
if size(E2, 2) > 1, E2 = E2(:, 1); end

% 所有六个样本进行分析
samples = {true_speech, synth_speech, A1, A2, E1, E2};
sample_names = {'真实语音', '合成语音', '元音A-说话人1', '元音A-说话人2', '元音E-说话人1', '元音E-说话人2'};
fs_array = [Fs1, Fs2, FsA1, FsA2, FsE1, FsE2];

% LPC分析参数
lpc_order = 20;  % LPC阶数
colors = {'r', 'g', 'b', 'c', 'm'};  % 共振峰标记颜色

% 创建结果保存目录
if ~exist('reconstructed_audio', 'dir')
    mkdir('reconstructed_audio');
end

% 1. 绘制LPC频谱和共振峰分析（三张图，每张图两个样本）
for fig_idx = 1:3
    figure('Name', ['LPC谱图与共振峰分析 (图' num2str(fig_idx) '/3)'], 'Position', [100+50*fig_idx, 100+50*fig_idx, 1000, 600]);
    
    % 每张图处理两个样本
    for subplot_idx = 1:2
        sample_idx = (fig_idx-1)*2 + subplot_idx;
        current_signal = samples{sample_idx};
        fs = fs_array(sample_idx);
        signal_name = sample_names{sample_idx};
        
        % 计算LPC系数和频率响应
        [a, g] = lpc(current_signal, lpc_order);
        [h, f] = freqz(1, a, 1024, fs);
        
        % 绘制LPC频谱
        subplot(2, 1, subplot_idx);
        plot(f, 20*log10(abs(h)), 'LineWidth', 1.5);
        title([signal_name ' 的LPC频谱（阶数 = ' num2str(lpc_order) '）'], 'FontSize', 12);
        xlabel('频率 (Hz)', 'FontSize', 10);
        ylabel('幅度 (dB)', 'FontSize', 10);
        grid on;
        hold on;
        
        % 平滑频谱以便更容易找到共振峰
        smoothed_spectrum = smooth(20*log10(abs(h)), 7);
        
        % 寻找局部最大值（共振峰）
        [pks, locs] = findpeaks(smoothed_spectrum, 'MinPeakHeight', max(smoothed_spectrum)-30, 'MinPeakDistance', 0.01);
        formant_freqs = f(locs);
        
        % 只保留合理范围内的共振峰（0-5000Hz范围内）
        formant_mask = formant_freqs < 5000 & formant_freqs > 50;
        formant_freqs = formant_freqs(formant_mask);
        formant_pks = pks(formant_mask);
        
        % 按频率排序
        [sorted_freqs, sort_idx] = sort(formant_freqs);
        sorted_pks = formant_pks(sort_idx);
        
        % 取前5个共振峰
        num_formants = min(5, length(sorted_freqs));
        if num_formants > 0
            main_formants = sorted_freqs(1:num_formants);
            
            % 在图上标记共振峰频率
            for i = 1:num_formants
                % 获取共振峰位置的幅度
                freq = main_formants(i);
                freq_idx = min(1024, max(1, round(freq/fs*1024) + 1));
                amp = 20*log10(abs(h(freq_idx)));
                
                % 绘制共振峰点，使用不同颜色
                color_idx = mod(i-1, length(colors)) + 1;
                plot(freq, amp, [colors{color_idx} 'o'], 'MarkerSize', 8, 'LineWidth', 2);
                
                % 添加频率标签
                text(freq, amp+2, ['F' num2str(i) '=' num2str(round(freq)) ' Hz'], ...
                    'FontSize', 9, 'Color', colors{color_idx});
            end
            
            % 输出共振峰信息到控制台
            fprintf('%s 的主要共振峰频率: ', signal_name);
            for i = 1:num_formants
                fprintf('F%d=%.0f Hz  ', i, main_formants(i));
            end
            fprintf('\n');
        end
        
        legend('LPC频谱', 'Location', 'best');
        xlim([0, 5000]);  % 限制x轴范围为0-5000Hz
    end
    
    sgtitle(['LPC频谱分析与共振峰识别 - 第' num2str(fig_idx) '组'], 'FontSize', 14, 'FontWeight', 'bold');
end

% 2. 只对真实语音和合成语音进行LPC重构
selected_samples = [1, 2];  % 只选择真实语音和合成语音

for s = 1:length(selected_samples)
    sample_idx = selected_samples(s);
    original_signal = samples{sample_idx};
    fs = fs_array(sample_idx);
    signal_name = sample_names{sample_idx};
    
    % 计算LPC系数
    [a, g] = lpc(original_signal, lpc_order);
    
    % 计算残差信号作为激励源
    residual = filter([1], a, original_signal);
    modified_residual = residual * 0.5; 
    reconstructed = filter(a, [1], modified_residual);
    
    % 保存重构的音频
    reconstructed_filename = fullfile('reconstructed_audio', ...
        [strrep(signal_name, ' ', '_'), '_reconstructed.wav']);
    audiowrite(reconstructed_filename, reconstructed, fs);
    
    % 计算时间轴
    t = (0:length(original_signal)-1)/fs;
    
    % 3. 绘制原始信号波形和重构信号波形（分开显示）
    figure('Name', [signal_name ' - 波形图'], 'Position', [150+100*s, 150, 1000, 800]);
    
    % 原始信号波形
    subplot(2, 1, 1);
    plot(t, original_signal, 'b-', 'LineWidth', 1.2);
    title([signal_name ' - 原始信号波形'], 'FontSize', 12);
    xlabel('时间 (秒)', 'FontSize', 10);
    ylabel('幅度', 'FontSize', 10);
    grid on;
    
    % 重构信号波形
    subplot(2, 1, 2);
    plot(t, reconstructed, 'r-', 'LineWidth', 1.2);
    title([signal_name ' - LPC重构信号波形 (阶数=' num2str(lpc_order) ')'], 'FontSize', 12);
    xlabel('时间 (秒)', 'FontSize', 10);
    ylabel('幅度', 'FontSize', 10);
    grid on;
    
    % 4. 绘制功率谱（分开显示）
    figure('Name', [signal_name ' - 功率谱'], 'Position', [200+100*s, 200, 1000, 800]);
    
    % 计算原始信号功率谱
    N = 2^nextpow2(length(original_signal));
    Y_orig = fft(original_signal, N);
    P_orig = abs(Y_orig/N).^2;
    P_orig = P_orig(1:N/2+1);
    
    % 计算重构信号功率谱
    Y_recon = fft(reconstructed, N);
    P_recon = abs(Y_recon/N).^2;
    P_recon = P_recon(1:N/2+1);
    
    f = fs*(0:(N/2))/N;  % 频率轴
    
    % 原始信号功率谱
    subplot(2, 1, 1);
    semilogx(f, 10*log10(P_orig+eps), 'b-', 'LineWidth', 1.2);
    title([signal_name ' - 原始信号功率谱'], 'FontSize', 12);
    xlabel('频率 (Hz)', 'FontSize', 10);
    ylabel('功率/频率 (dB/Hz)', 'FontSize', 10);
    grid on;
    xlim([50, fs/2]);
    
    % 重构信号功率谱
    subplot(2, 1, 2);
    semilogx(f, 10*log10(P_recon+eps), 'r-', 'LineWidth', 1.2);
    title([signal_name ' - LPC重构信号功率谱 (阶数=' num2str(lpc_order) ')'], 'FontSize', 12);
    xlabel('频率 (Hz)', 'FontSize', 10);
    ylabel('功率/频率 (dB/Hz)', 'FontSize', 10);
    grid on;
    xlim([50, fs/2]);
end

fprintf('LPC频谱分析与语音重构完成！\n');
