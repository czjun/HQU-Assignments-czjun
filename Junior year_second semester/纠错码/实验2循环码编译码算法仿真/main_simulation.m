% main_simulation.m - 主仿真脚本
% 实现循环码编码/解码、BPSK调制/解调并通过AWGN信道的通信系统仿真

clearvars;
close all;
clc;

%% 仿真参数
% 循环码参数
g_poly = [1 0 1 1]; % (7,4) 汉明码的生成多项式 g(x) = x^3 + x + 1
k = 4;              % 信息比特数
n_minus_k = length(g_poly) - 1;
n = k + n_minus_k;  % 编码后总比特数 (码字长度)

% Eb/N0 参数
EbN0_dB_range = 0:1:10; % Eb/N0 范围 (dB)
num_EbN0_points = length(EbN0_dB_range);

% 仿真控制参数
% 为了得到平滑曲线，每个Eb/N0点需要足够的比特错误数 (例如，至少100个错误)
% 或者传输足够多的比特数
num_bits_to_simulate = 2 * 10^5; % 每个Eb/N0点模拟的总信息比特数
max_bit_errors = 200;          % 每个Eb/N0点收集到的最大错误比特数 (提前终止条件)
min_block_errors_for_coded = 50; % (可选) 针对编码情况，至少统计这么多块错误
num_blocks_to_simulate = ceil(num_bits_to_simulate / k); % 总共模拟的信息块数

fprintf('开始仿真 (7,4) 循环码在 AWGN 信道下的性能...\\n');
% fprintf('生成多项式 g(x): %s\\n', poly2str(gf(g_poly),'x')); % Communications Toolbox needed for gf. 使用下面的手动字符串替代。
fprintf('信息位数 k = %d, 码字长度 n = %d\\n', k, n);
fprintf('Eb/N0 范围: %s dB\\n', num2str(EbN0_dB_range));
fprintf('每个 Eb/N0 点模拟的信息比特数: %d (或最多 %d 比特错误)\\n', num_bits_to_simulate, max_bit_errors);

%% 初始化结果存储
ber_coded = zeros(1, num_EbN0_points);
ber_uncoded = zeros(1, num_EbN0_points);

%% 主仿真循环
for idx_ebn0 = 1:num_EbN0_points
    EbN0_dB = EbN0_dB_range(idx_ebn0);
    fprintf('\\n正在仿真 Eb/N0 = %.1f dB\\n', EbN0_dB);

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
            encoded_bits = cyclic_encoder(original_msg_bits, g_poly);
            
            % 3. BPSK 调制
            tx_symbols_coded = modulator(encoded_bits);
            
            % 4. AWGN 信道
            % 对于编码系统，k_val = k, n_val = n
            rx_symbols_coded = channel_simulator(tx_symbols_coded, EbN0_dB, k, n);
            
            % 5. BPSK 解调
            demod_bits_coded = demodulator(rx_symbols_coded);
            
            % 6. 循环译码
            [decoded_msg_bits, ~, ~] = cyclic_decoder(demod_bits_coded, g_poly);
            
            % 7. 计算比特错误
            bit_errors_coded = sum(bitxor(original_msg_bits, decoded_msg_bits));
            total_coded_bit_errors = total_coded_bit_errors + bit_errors_coded;
            total_coded_bits_transmitted = total_coded_bits_transmitted + k;
        end

        % --- 未编码系统 ---
        if total_uncoded_bits_transmitted < num_bits_to_simulate || total_uncoded_bit_errors < max_bit_errors
            % 2. (无编码) BPSK 调制原始信息比特
            tx_symbols_uncoded = modulator(original_msg_bits);
            
            % 3. AWGN 信道
            % 对于未编码系统，信息比特直接传输，所以有效码率 R=1 (k_val = k, n_val = k)
            rx_symbols_uncoded = channel_simulator(tx_symbols_uncoded, EbN0_dB, k, k); % k,k => R=1
            
            % 4. BPSK 解调
            demod_bits_uncoded = demodulator(rx_symbols_uncoded);
            
            % 5. 计算比特错误 (与原始信息比特比较)
            bit_errors_uncoded = sum(bitxor(original_msg_bits, demod_bits_uncoded));
            total_uncoded_bit_errors = total_uncoded_bit_errors + bit_errors_uncoded;
            total_uncoded_bits_transmitted = total_uncoded_bits_transmitted + k;
        end
        
        if mod(num_sim_blocks_current_ebn0, 10000) == 0
            fprintf('  ...已处理码块数: %d, 当前BER (编码): %.2e, 当前BER (未编码): %.2e\\r', ...
                num_sim_blocks_current_ebn0, ...
                total_coded_bit_errors/total_coded_bits_transmitted, ...
                total_uncoded_bit_errors/total_uncoded_bits_transmitted);
        end
    end
    
    % 计算当前 Eb/N0 点的 BER
    if total_coded_bits_transmitted > 0
        ber_coded(idx_ebn0) = total_coded_bit_errors / total_coded_bits_transmitted;
    else
        ber_coded(idx_ebn0) = NaN; % Should not happen if loop runs
    end
    
    if total_uncoded_bits_transmitted > 0
        ber_uncoded(idx_ebn0) = total_uncoded_bit_errors / total_uncoded_bits_transmitted;
    else
        ber_uncoded(idx_ebn0) = NaN;
    end

    fprintf('  Eb/N0 = %.1f dB: BER (编码) = %.2e (%d 错误 / %d 比特), BER (未编码) = %.2e (%d 错误 / %d 比特)\\n', ...
        EbN0_dB, ber_coded(idx_ebn0), total_coded_bit_errors, total_coded_bits_transmitted, ...
        ber_uncoded(idx_ebn0), total_uncoded_bit_errors, total_uncoded_bits_transmitted);
end

%% 理论 BPSK BER 曲线
% BER_theoretical_bpsk = 0.5 * erfc(sqrt(10.^(EbN0_dB_range/10)));
% MATLAB uses qfunc, Q(x) = 0.5 * erfc(x/sqrt(2))
% For BPSK, argument is sqrt(2*Eb/N0) for Q function.
% So, Eb/N0_linear = 10.^(EbN0_dB_range/10)
% arg_qfunc = sqrt(2 * 10.^(EbN0_dB_range/10));
% BER_theoretical_bpsk = qfunc(arg_qfunc);
% Note: This theoretical curve is for UNCODED BPSK where Eb is bit energy.

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
axis([min(EbN0_dB_range) max(EbN0_dB_range) 10^-5 1]); % Adjust y-axis limit if needed

disp('仿真完成。');

fprintf('(g(x)的简单多项式字符串: %s)\\n', g_poly_str);

function y = qfunc(x)
    y = 0.5 * erfc(x / sqrt(2));
end 