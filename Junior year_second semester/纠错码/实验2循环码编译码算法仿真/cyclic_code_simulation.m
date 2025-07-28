%% 循环码编译码算法仿真
% 实现(7,4)循环码的编码/解码、BPSK调制/解调并通过AWGN信道的通信系统仿真
% 本脚本合并了所有相关功能，包括：
% 1. 循环码编码器
% 2. 循环码译码器
% 3. BPSK调制器
% 4. BPSK解调器
% 5. AWGN信道模拟器
% 6. 主仿真程序

clearvars;
close all;
clc;

%% 仿真参数设置
% 循环码参数
g_poly = [1 0 1 1]; % (7,4) 汉明码的生成多项式 g(x) = x^3 + x + 1
k = 4;              % 信息比特数
n_minus_k = length(g_poly) - 1;
n = k + n_minus_k;  % 编码后总比特数 (码字长度)

% Eb/N0 参数
EbN0_dB_range = 0:1:10; % Eb/N0 范围 (dB)
num_EbN0_points = length(EbN0_dB_range);

% 仿真控制参数
num_bits_to_simulate = 1 * 10^6; % 每个Eb/N0点模拟的总信息比特数
max_bit_errors = 5000;          % 每个Eb/N0点收集到的最大错误比特数 (提前终止条件)
num_blocks_to_simulate = ceil(num_bits_to_simulate / k); % 总共模拟的信息块数

% 显示生成多项式
g_poly_str = '';
for i = 1:length(g_poly)
    if g_poly(i) == 1
        if i == 1
            g_poly_str = [g_poly_str, sprintf('x^%d', length(g_poly)-i)];
        elseif i == length(g_poly)
            g_poly_str = [g_poly_str, ' + 1'];
        elseif length(g_poly)-i == 1
             g_poly_str = [g_poly_str, ' + x'];
        else
            g_poly_str = [g_poly_str, ' + x^', num2str(length(g_poly)-i)];
        end
    end
end
if startsWith(g_poly_str, ' + ')
    g_poly_str = g_poly_str(4:end);
end

fprintf('开始仿真 (7,4) 循环码在 AWGN 信道下的性能...\n');
fprintf('生成多项式 g(x): %s\n', g_poly_str);
fprintf('信息位数 k = %d, 码字长度 n = %d\n', k, n);
fprintf('Eb/N0 范围: %s dB\n', num2str(EbN0_dB_range));
fprintf('每个 Eb/N0 点模拟的信息比特数: %d (或最多 %d 比特错误)\n', num_bits_to_simulate, max_bit_errors);

%% 初始化结果存储
ber_coded = zeros(1, num_EbN0_points);
ber_uncoded = zeros(1, num_EbN0_points);

%% 主仿真循环
for idx_ebn0 = 1:num_EbN0_points
    EbN0_dB = EbN0_dB_range(idx_ebn0);
    fprintf('\n正在仿真 Eb/N0 = %.1f dB\n', EbN0_dB);

    total_coded_bit_errors = 0;
    total_coded_bits_transmitted = 0;
    
    total_uncoded_bit_errors = 0;
    total_uncoded_bits_transmitted = 0;
    
    num_sim_blocks_current_ebn0 = 0;

    % 循环直到达到足够的比特数或错误数
    while (total_coded_bits_transmitted < num_bits_to_simulate && total_coded_bit_errors < max_bit_errors) || ...
          (total_uncoded_bits_transmitted < num_bits_to_simulate && total_uncoded_bit_errors < max_bit_errors)
        
        if total_coded_bits_transmitted >= num_bits_to_simulate && total_uncoded_bits_transmitted >= num_bits_to_simulate
            if total_coded_bit_errors >= max_bit_errors || total_uncoded_bit_errors >= max_bit_errors
                 break; % 如果两个都达到最大错误数，也提前结束
            end
            if EbN0_dB < 5 % 对于低信噪比，错误数可能先达到
                if total_coded_bit_errors >= max_bit_errors && total_uncoded_bit_errors >= max_bit_errors
                    break;
                end
            else % 对于高信噪比，比特数可能先达到
                if total_coded_bits_transmitted >= num_bits_to_simulate && total_uncoded_bits_transmitted >= num_bits_to_simulate
                    break;
                end
            end
        end
        
        num_sim_blocks_current_ebn0 = num_sim_blocks_current_ebn0 + 1;

        % 1. 生成随机信息比特
        original_msg_bits = randi([0 1], 1, k);

        % --- 编码系统 ---
        if total_coded_bits_transmitted < num_bits_to_simulate || total_coded_bit_errors < max_bit_errors
            % 2. 循环编码
            encoded_bits = cyclic_encoder_local(original_msg_bits, g_poly);
            
            % 3. BPSK 调制
            tx_symbols_coded = modulator_local(encoded_bits);
            
            % 4. AWGN 信道
            rx_symbols_coded = channel_simulator_local(tx_symbols_coded, EbN0_dB, k, n);
            
            % 5. BPSK 解调
            demod_bits_coded = demodulator_local(rx_symbols_coded);
            
            % 6. 循环译码
            [decoded_msg_bits, ~, ~] = cyclic_decoder_local(demod_bits_coded, g_poly);
            
            % 7. 计算比特错误
            bit_errors_coded = sum(bitxor(original_msg_bits, decoded_msg_bits));
            total_coded_bit_errors = total_coded_bit_errors + bit_errors_coded;
            total_coded_bits_transmitted = total_coded_bits_transmitted + k;
        end

        % --- 未编码系统 ---
        if total_uncoded_bits_transmitted < num_bits_to_simulate || total_uncoded_bit_errors < max_bit_errors
            % 2. (无编码) BPSK 调制原始信息比特
            tx_symbols_uncoded = modulator_local(original_msg_bits);
            
            % 3. AWGN 信道
            rx_symbols_uncoded = channel_simulator_local(tx_symbols_uncoded, EbN0_dB, k, k); % k,k => R=1
            
            % 4. BPSK 解调
            demod_bits_uncoded = demodulator_local(rx_symbols_uncoded);
            
            % 5. 计算比特错误
            bit_errors_uncoded = sum(bitxor(original_msg_bits, demod_bits_uncoded));
            total_uncoded_bit_errors = total_uncoded_bit_errors + bit_errors_uncoded;
            total_uncoded_bits_transmitted = total_uncoded_bits_transmitted + k;
        end
        
        if mod(num_sim_blocks_current_ebn0, 10000) == 0
            fprintf('  ...已处理码块数: %d, 当前BER (编码): %.2e, 当前BER (未编码): %.2e\r', ...
                num_sim_blocks_current_ebn0, ...
                total_coded_bit_errors/total_coded_bits_transmitted, ...
                total_uncoded_bit_errors/total_uncoded_bits_transmitted);
        end
    end
    
    % 计算当前 Eb/N0 点的 BER
    if total_coded_bits_transmitted > 0
        ber_coded(idx_ebn0) = total_coded_bit_errors / total_coded_bits_transmitted;
    else
        ber_coded(idx_ebn0) = NaN;
    end
    
    if total_uncoded_bits_transmitted > 0
        ber_uncoded(idx_ebn0) = total_uncoded_bit_errors / total_uncoded_bits_transmitted;
    else
        ber_uncoded(idx_ebn0) = NaN;
    end

    fprintf('  Eb/N0 = %.1f dB: BER (编码) = %.2e (%d 错误 / %d 比特), BER (未编码) = %.2e (%d 错误 / %d 比特)\n', ...
        EbN0_dB, ber_coded(idx_ebn0), total_coded_bit_errors, total_coded_bits_transmitted, ...
        ber_uncoded(idx_ebn0), total_uncoded_bit_errors, total_uncoded_bits_transmitted);
end

%% 理论 BPSK BER 曲线
EbN0_linear_range = 10.^(EbN0_dB_range/10);
BER_theoretical_uncoded_bpsk = qfunc(sqrt(2 * EbN0_linear_range));

%% 绘图
figure;
semilogy(EbN0_dB_range, ber_coded, '-ro', 'LineWidth', 1.5, 'MarkerSize', 8, 'DisplayName', sprintf('(%d,%d) 编码 BPSK', n, k));
hold on;
semilogy(EbN0_dB_range, ber_uncoded, '-bs', 'LineWidth', 1.5, 'MarkerSize', 8, 'DisplayName', '未编码 BPSK');
semilogy(EbN0_dB_range, BER_theoretical_uncoded_bpsk, '--k', 'LineWidth', 1, 'DisplayName', '理论未编码 BPSK');
hold off;

grid on;
xlabel('Eb/N0 (dB)');
ylabel('误比特率 (BER)');
title(sprintf('(%d,%d) 循环码与BPSK在AWGN信道下的性能', n, k));
legend('show', 'Location', 'SouthWest');
axis([min(EbN0_dB_range) max(EbN0_dB_range) 10^-5 1]);

disp('仿真完成。');

%% 辅助函数

% Q函数
function y = qfunc(x)
    y = 0.5 * erfc(x / sqrt(2));
end

% 循环码编码器
function encoded_bits = cyclic_encoder_local(msg_bits, g_poly)
    % 输入:
    %   msg_bits: 1xK 行向量，代表 K 位信息比特
    %   g_poly: 1x(N-K+1) 行向量，代表生成多项式的系数
    % 输出:
    %   encoded_bits: 1xN 行向量，代表 N 位编码后的比特

    k = length(msg_bits);
    n_minus_k = length(g_poly) - 1;
    n = k + n_minus_k;

    % 构造被除数: m(x) * x^(n-k)
    dividend = [msg_bits, zeros(1, n_minus_k)];

    % 执行多项式除法 (GF(2)运算)
    remainder = dividend;
    for i = 1:k
        if remainder(i) == 1
            remainder(i:i+n_minus_k) = bitxor(remainder(i:i+n_minus_k), g_poly);
        end
    end
    
    % 提取余数 (最后 n-k 位)
    parity_bits = remainder(k+1:end);
    
    % 构造系统码: [信息位 校验位]
    encoded_bits = [msg_bits, parity_bits];
end

% 循环码译码器
function [decoded_msg, error_detected, error_corrected] = cyclic_decoder_local(received_codeword, g_poly)
    % 输入:
    %   received_codeword: 1xN 行向量，代表接收到的 N 位码字
    %   g_poly: 1x(N-K+1) 行向量，代表生成多项式的系数
    % 输出:
    %   decoded_msg: 1xK 行向量，代表译码后的 K 位信息比特
    %   error_detected: 布尔值，指示是否检测到错误
    %   error_corrected: 布尔值，指示是否成功纠正了错误

    n = length(received_codeword);
    n_minus_k = length(g_poly) - 1;
    k = n - n_minus_k;

    error_detected = false;
    error_corrected = false;

    % 计算综合征 s(x) = r(x) mod g(x)
    temp_received = received_codeword;
    for i = 1:(n - (length(g_poly)-1))
        if temp_received(i) == 1
            temp_received(i:i+length(g_poly)-1) = bitxor(temp_received(i:i+length(g_poly)-1), g_poly);
        end
    end
    syndrome = temp_received(n - (length(g_poly)-1) + 1 : end);

    corrected_codeword = received_codeword;

    if any(syndrome) % 如果综合征非零，则检测到错误
        error_detected = true;
        
        % 简化的单比特纠错
        found_correction = false;
        for err_pos = 1:n
            error_pattern = zeros(1, n);
            error_pattern(err_pos) = 1;
            
            % 计算 (error_pattern) mod g(x)
            temp_error_pattern = error_pattern;
            for i = 1:(n-(length(g_poly)-1))
                if temp_error_pattern(i) == 1
                    temp_error_pattern(i:i+length(g_poly)-1) = bitxor(temp_error_pattern(i:i+length(g_poly)-1), g_poly);
                end
            end
            error_syndrome = temp_error_pattern(k+1:end);
            
            if isequal(error_syndrome, syndrome)
                % 找到匹配的单比特错误模式
                corrected_codeword = bitxor(received_codeword, error_pattern);
                error_corrected = true;
                found_correction = true;
                break;
            end
        end
    end

    % 从（可能已纠正的）码字中提取信息比特
    decoded_msg = corrected_codeword(1:k);
end

% BPSK 调制器
function bpsk_symbols = modulator_local(bit_stream)
    % 输入:
    %   bit_stream: 1xL 行向量，代表 L 个二进制比特 (0 或 1)
    % 输出:
    %   bpsk_symbols: 1xL 行向量，代表 BPSK 调制后的符号 (+1, -1)

    % 映射规则: 0 -> +1, 1 -> -1
    bpsk_symbols = 1 - 2 * bit_stream;
end

% BPSK 解调器
function demod_bits = demodulator_local(received_symbols)
    % 输入:
    %   received_symbols: 1xL 行向量，代表接收到的 BPSK 符号
    % 输出:
    %   demod_bits: 1xL 行向量，代表解调后的二进制比特

    % 判决规则: symbol > 0 -> 0, symbol <= 0 -> 1
    demod_bits = zeros(size(received_symbols));
    demod_bits(received_symbols <= 0) = 1;
    demod_bits(received_symbols > 0) = 0;
end

% AWGN 信道模拟器
function noisy_symbols = channel_simulator_local(tx_symbols, EbN0_dB, k_bits, n_bits)
    % 输入:
    %   tx_symbols: 1xL 行向量，发送的调制符号
    %   EbN0_dB: 每比特能量与噪声功率谱密度之比 (dB)
    %   k_bits: 每个码字的信息比特数
    %   n_bits: 每个码字的总比特数
    % 输出:
    %   noisy_symbols: 1xL 行向量，添加了高斯噪声的符号

    % 假设发送符号的平均功率为 1
    Es = mean(abs(tx_symbols).^2);
    
    % 计算码率
    R = k_bits / n_bits;
    
    % 将 Eb/N0 (dB) 转换为线性值
    EbN0_linear = 10^(EbN0_dB / 10);
    
    % 计算 Es/N0
    EsN0_linear = EbN0_linear * R;
    
    % 计算噪声方差
    sigma_squared = Es / (2 * EsN0_linear);
    noise_std_dev = sqrt(sigma_squared);
    
    % 生成高斯噪声
    noise = noise_std_dev * randn(size(tx_symbols));
    
    noisy_symbols = tx_symbols + noise;
end 