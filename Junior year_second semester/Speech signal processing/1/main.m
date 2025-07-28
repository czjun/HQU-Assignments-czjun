%% 语音信号的时频域分析
% 实现语音信号的时域和频域分析，包括预处理、短时特征计算和语谱图分析
clear all;
close all;

%% 1. 读取音频文件
% 分别读取真实音频和合成音频
[y_real, fs_real] = audioread('3.wav');  % 替换真实音频文件名
[y_synth, fs_synth] = audioread('33.wav');  % 替换合成音频文件名

% 如果是双声道，只取第一个声道
if size(y_real, 2) > 1
    y_real = y_real(:, 1);
end
if size(y_synth, 2) > 1
    y_synth = y_synth(:, 1);
end

% 显示原始波形
figure;
subplot(2,1,1);
t_real = (0:length(y_real)-1)/fs_real;
plot(t_real, y_real);
title('真实音频波形');
xlabel('时间 (秒)');
ylabel('幅度');

subplot(2,1,2);
t_synth = (0:length(y_synth)-1)/fs_synth;
plot(t_synth, y_synth);
title('合成音频波形');
xlabel('时间 (秒)');
ylabel('幅度');

%% 2. 预处理
% 预加重
alpha = 0.97;  % 预加重系数
y_real_pre = filter([1 -alpha], 1, y_real);
y_synth_pre = filter([1 -alpha], 1, y_synth);

% 分帧参数
frame_len = round(0.025 * fs_real);  % 帧长25ms
frame_shift = round(0.010 * fs_real);  % 帧移10ms
win = hamming(frame_len);  % 汉明窗

% 分帧函数
framing = @(signal, frame_len, frame_shift) buffer(signal, frame_len, frame_len-frame_shift, 'nodelay')';

% 对真实音频分帧
try
    frames_real = framing(y_real_pre, frame_len, frame_shift);
catch
    % 如果出错使用零填充
    padded_signal = [y_real_pre; zeros(frame_len, 1)];
    frames_real = framing(padded_signal, frame_len, frame_shift);
end

% 对合成音频分帧
try
    frames_synth = framing(y_synth_pre, frame_len, frame_shift);
catch
    padded_signal = [y_synth_pre; zeros(frame_len, 1)];
    frames_synth = framing(padded_signal, frame_len, frame_shift);
end

% 加窗
frames_real_win = frames_real .* win';
frames_synth_win = frames_synth .* win';

%% 3. 短时特征计算
% 计算短时能量
energy_real = sum(frames_real_win.^2, 2);
energy_synth = sum(frames_synth_win.^2, 2);


% 绘制短时特征
frame_time_real = (0:size(frames_real,1)-1) * frame_shift / fs_real;
frame_time_synth = (0:size(frames_synth,1)-1) * frame_shift / fs_synth;

figure;
% 短时能量
subplot(1,1,1);
plot(frame_time_real, energy_real);
title('真实音频短时能量');
xlabel('时间 (秒)');
ylabel('能量');


%% 4. 计算语谱图
% STFT参数
nfft = 2^nextpow2(frame_len);  % FFT点数
overlap = frame_len - frame_shift;  % 重叠点数

% 窄带语谱图（长窗口，频率分辨率高）
window_narrow = hamming(frame_len * 4);
noverlap_narrow = length(window_narrow) - frame_shift;

% 宽带语谱图（短窗口，时间分辨率高）
window_wide = hamming(frame_len / 2);
noverlap_wide = length(window_wide) - frame_shift;

% 绘制真实音频语谱图
figure;
% 窄带语谱图
subplot(2,2,1);
spectrogram(y_real, window_narrow, noverlap_narrow, nfft, fs_real, 'yaxis');
title('真实音频窄带语谱图');
colorbar;

% 宽带语谱图
subplot(2,2,2);
spectrogram(y_real, window_wide, noverlap_wide, nfft, fs_real, 'yaxis');
title('真实音频宽带语谱图');
colorbar;

% 绘制合成音频语谱图
% 窄带语谱图
subplot(2,2,3);
spectrogram(y_synth, window_narrow, noverlap_narrow, nfft, fs_synth, 'yaxis');
title('合成音频窄带语谱图');
colorbar;

% 宽带语谱图
subplot(2,2,4);
spectrogram(y_synth, window_wide, noverlap_wide, nfft, fs_synth, 'yaxis');
title('合成音频宽带语谱图');
colorbar;

