%% 线性分组码编译码算法仿真

clear all;
close all;
clc;

%% 参数设置
n = 7;              % 码字长度
k = 4;              % 信息位长度
method = 'hamming'; % 码字生成方法: 'hamming', 'custom'
num_frames = 40000; % 模拟的帧数
snr_db_range = 0:1:10; % SNR范围(dB)

%% 生成线性码的矩阵
[G, H] = generate_linear_code_matrices(n, k, method);

% 显示生成矩阵和校验矩阵
disp('生成矩阵G:');
disp(G);
disp('校验矩阵H:');
disp(H);

%% 初始化结果存储
ber_uncoded = zeros(1, length(snr_db_range));
ber_coded = zeros(1, length(snr_db_range));

%% 主仿真循环
for snr_idx = 1:length(snr_db_range)
    snr_db = snr_db_range(snr_idx);
    
    % 计数器初始化
    errors_uncoded = 0;
    errors_coded = 0;
    total_bits_uncoded = 0;
    total_bits_coded = 0;
    
    for frame = 1:num_frames
        % 生成随机信息比特
        msg_bits = randi([0 1], 1, k);
        
        %% 1. 无编码系统
        % 调制
        uncoded_modulated = bpsk_modulation(msg_bits);
        
        % 通过AWGN信道
        uncoded_received = awgn_channel(uncoded_modulated, snr_db);
        
        % 解调
        uncoded_demodulated = bpsk_demodulation(uncoded_received);
        
        % 统计误比特
        errors_uncoded = errors_uncoded + sum(msg_bits ~= uncoded_demodulated);
        total_bits_uncoded = total_bits_uncoded + length(msg_bits);
        
        %% 2. 编码系统
        % 编码
        coded_bits = linear_block_encoder(msg_bits, G);
        
        % 调制
        coded_modulated = bpsk_modulation(coded_bits);
        
        % 通过AWGN信道
        coded_received = awgn_channel(coded_modulated, snr_db);
        
        % 解调
        coded_demodulated = bpsk_demodulation(coded_received);
        
        % 译码
        decoded_msg = linear_block_decoder(coded_demodulated, H, G);
        
        % 统计误比特
        errors_coded = errors_coded + sum(msg_bits ~= decoded_msg);
        total_bits_coded = total_bits_coded + length(msg_bits);
    end
    
    % 计算误比特率
    ber_uncoded(snr_idx) = errors_uncoded / total_bits_uncoded;
    ber_coded(snr_idx) = errors_coded / total_bits_coded;
    
    % 显示当前进度
    fprintf('SNR = %d dB, 未编码BER = %.6f, 编码BER = %.6f\n', ...
        snr_db, ber_uncoded(snr_idx), ber_coded(snr_idx));
end

%% 绘制结果
figure;
semilogy(snr_db_range, ber_uncoded, 'ro-', 'LineWidth', 2, 'MarkerSize', 8);
hold on;
semilogy(snr_db_range, ber_coded, 'bs-', 'LineWidth', 2, 'MarkerSize', 8);
grid on;
xlabel('信噪比 (dB)');
ylabel('误比特率 (BER)');
title(sprintf('(%d,%d)线性分组码性能对比', n, k));
legend('未编码', '编码');

% 添加理论BPSK误比特率曲线
ber_theory = 0.5 * erfc(sqrt(10.^(snr_db_range/10)));
semilogy(snr_db_range, ber_theory, 'k--', 'LineWidth', 1.5);
legend('未编码', '编码', '理论BPSK');