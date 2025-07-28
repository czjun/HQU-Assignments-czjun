% 语音信号的基音周期检测
% 实验三：使用自相关函数检测基音周期

% 基本参数设置
fs = 16000;         % 采样率
frame_len = 25e-3;  % 帧长25ms
frame_shift = 10e-3;% 帧移10ms
win_len = round(frame_len * fs);
win_shift = round(frame_shift * fs);
min_pitch = 50;     % 最小基音频率 (Hz)
max_pitch = 400;    % 最大基音频率 (Hz)
min_period = round(fs/max_pitch); % 最小周期（样本数）
max_period = round(fs/min_pitch); % 最大周期（样本数）

% 读取音频文件
[A1, fs_A1] = audioread('实验二素材/A1.wav');
[A2, fs_A2] = audioread('实验二素材/A2.wav');

% 检测基音周期
[pitch_A1, times_A1] = detect_pitch(A1, fs_A1, win_len, win_shift, min_period, max_period);
[pitch_A2, times_A2] = detect_pitch(A2, fs_A2, win_len, win_shift, min_period, max_period);

% 绘制语谱图和基音频率轨迹
plot_spectrogram_with_pitch(A1, fs_A1, pitch_A1, times_A1, '说话人1 - 元音A');
plot_spectrogram_with_pitch(A2, fs_A2, pitch_A2, times_A2, '说话人2 - 元音A');

% 计算平均基音频率
mean_pitch_A1 = mean(pitch_A1(pitch_A1 > 0));
mean_pitch_A2 = mean(pitch_A2(pitch_A2 > 0));
   
fprintf('说话人1元音A的平均基音频率: %.2f Hz\n', mean_pitch_A1);
fprintf('说话人2元音A的平均基音频率: %.2f Hz\n', mean_pitch_A2);

% 2. 分析真实语音和深度学习生成的语音
fprintf('\n分析真实语音和深度学习生成的语音...\n');

% 读取音频文件
[true_speech, fs_true] = audioread('实验一素材/true.wav');
[gen_speech, fs_gen] = audioread('实验一素材/study.wav');

% 检测基音周期
[pitch_true, times_true] = detect_pitch(true_speech, fs_true, win_len, win_shift, min_period, max_period);
[pitch_gen, times_gen] = detect_pitch(gen_speech, fs_gen, win_len, win_shift, min_period, max_period);

% 绘制语谱图和基音频率轨迹
plot_spectrogram_with_pitch(true_speech, fs_true, pitch_true, times_true, '真实语音');
plot_spectrogram_with_pitch(gen_speech, fs_gen, pitch_gen, times_gen, '深度学习生成语音');

% 计算平均基音频率
mean_pitch_true = mean(pitch_true(pitch_true > 0));
mean_pitch_gen = mean(pitch_gen(pitch_gen > 0));

fprintf('真实语音的平均基音频率: %.2f Hz\n', mean_pitch_true);
fprintf('深度学习生成语音的平均基音频率: %.2f Hz\n', mean_pitch_gen);

% 计算相关系数 - 处理不同长度数据的情况
if length(times_true) ~= length(times_gen)
    % 如果长度不同，进行插值处理
    fprintf('真实语音和生成语音长度不同，进行插值处理...\n');
    
    % 在共同的时间范围内进行比较
    min_time = max(min(times_true), min(times_gen));
    max_time = min(max(times_true), max(times_gen));
    
    % 创建共同的时间轴
    common_time = min_time:frame_shift/fs:max_time;
    
    % 插值到共同时间轴
    pitch_true_interp = interp1(times_true, pitch_true, common_time, 'linear', 0);
    pitch_gen_interp = interp1(times_gen, pitch_gen, common_time, 'linear', 0);
    
    % 计算相关系数
    valid_idx_interp = pitch_true_interp > 0 & pitch_gen_interp > 0;
    if sum(valid_idx_interp) > 1
        corr_coef = corrcoef(pitch_true_interp(valid_idx_interp), pitch_gen_interp(valid_idx_interp));
        fprintf('基音频率轨迹的相关系数: %.4f\n', corr_coef(1,2));
    else
        fprintf('共同时间轴上无足够的有效数据点计算相关系数\n');
    end
else
    % 长度相同，直接计算相关系数
    valid_idx = pitch_true > 0 & pitch_gen > 0;
    if sum(valid_idx) > 1
        corr_coef = corrcoef(pitch_true(valid_idx), pitch_gen(valid_idx));
        fprintf('基音频率轨迹的相关系数: %.4f\n', corr_coef(1,2));
    else
        fprintf('无法计算相关系数，有效数据点不足\n');
    end
end

% 函数：基音周期检测（使用自相关法）
function [pitch_contour, times] = detect_pitch(audio, fs, win_len, win_shift, min_period, max_period)
    % 将输入信号分帧
    num_frames = floor((length(audio) - win_len) / win_shift) + 1;
    pitch_contour = zeros(num_frames, 1);
    times = zeros(num_frames, 1);
    
    for i = 1:num_frames
        % 获取当前帧
        frame_start = (i-1) * win_shift + 1;
        frame_end = frame_start + win_len - 1;
        frame = audio(frame_start:frame_end);
        
        % 加窗（Hamming窗）
        frame = frame .* hamming(win_len);
        
        % 计算自相关函数
        r = xcorr(frame);
        r = r(win_len:end); % 只保留延迟大于等于0的部分
        
        % 在最小周期和最大周期范围内寻找峰值
        [peaks, locs] = findpeaks(r(min_period:max_period));
        locs = locs + min_period - 1;
        
        % 找出最大峰值位置作为基音周期
        if ~isempty(peaks)
            [~, max_idx] = max(peaks);
            pitch_period = locs(max_idx);
            pitch_contour(i) = fs / pitch_period;
        else
            pitch_contour(i) = 0; % 无声段
        end
        
        % 记录时间点
        times(i) = (frame_start + win_len/2) / fs;
    end
end

% 函数：绘制语谱图和基音频率变化轨迹
function plot_spectrogram_with_pitch(audio, fs, pitch_contour, times, title_str)
    figure;
    
    % 上图：语谱图
    subplot(2,1,1);
    spectrogram(audio, hamming(round(0.03*fs)), round(0.02*fs), 1024, fs, 'yaxis');
    title([title_str ' - 语谱图']);
    
    % 下图：基音频率轨迹
    subplot(2,1,2);
    plot(times, pitch_contour, 'b-');
    ylim([50 500]);
    xlabel('时间 (秒)');
    ylabel('基音频率 (Hz)');
    title('基音频率变化轨迹');
    grid on;
end 