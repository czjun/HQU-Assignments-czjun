function received_signal = awgn_channel(transmitted_signal, snr_db)
% awgn_channel - 实现加性高斯白噪声信道
% 输入:
%   transmitted_signal - 发送信号
%   snr_db - 信噪比(dB)
% 输出:
%   received_signal - 通过信道后的接收信号

% 计算线性信噪比
snr_linear = 10^(snr_db/10);

% 计算信号能量
signal_power = mean(transmitted_signal.^2);

% 计算噪声功率
noise_power = signal_power / snr_linear;

% 生成高斯白噪声
noise = sqrt(noise_power) * randn(size(transmitted_signal));

% 将噪声添加到信号中
received_signal = transmitted_signal + noise;
end 