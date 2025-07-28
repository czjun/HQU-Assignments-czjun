%% 实验三：基于Matlab的卷积码仿真及性能分析
% 主仿真脚本: communication_system_simulation.m
% 使用 MATLAB 通信工具箱内置函数 (部分结构参考 new.m)

clearvars;
close all;
clc;

%% 仿真参数定义
% 卷积码参数
K = 3; % 约束长度
generator_polynomials_octal = [7 5]; % 生成多项式 (八进制 g0, g1)
code_rate = 1/2; % 码率
trellis_structure = poly2trellis(K, generator_polynomials_octal); % 定义网格
tblen = 5 * K; % Viterbi译码器回溯深度

% 仿真控制参数
num_ber_bits = 1e7;   % 用于BER计算的信息比特数
num_info_bits_to_generate = num_ber_bits + tblen; % 为编码路径生成的总比特数
EbN0_dB_range = 0:1:9;  % Eb/N0 范围 (dB)

% 初始化结果存储
ber_coded_hard = zeros(size(EbN0_dB_range)); % 硬判决
ber_coded_soft = zeros(size(EbN0_dB_range)); % 软判决
ber_uncoded = zeros(size(EbN0_dB_range));
ber_theoretical_bpsk = zeros(size(EbN0_dB_range));

%% 开始仿真循环
total_sim_loops = length(EbN0_dB_range);
fprintf('开始仿真 (使用MATLAB内置函数, continuous Viterbi mode)...\n');

for idx_ebn0 = 1:total_sim_loops
    EbN0_dB = EbN0_dB_range(idx_ebn0);
    fprintf('正在仿真 Eb/N0 = %.1f dB (循环 %d/%d)...\n', EbN0_dB, idx_ebn0, total_sim_loops);
    
    ebn0_linear = 10^(EbN0_dB / 10);
    
    % --- 生成信息比特流 ---
    info_bits_stream = randi([0 1], num_info_bits_to_generate, 1);
    
    % --- 编码通信系统 (硬判决和软判决共享编码和信道) ---
    info_for_convenc = info_bits_stream;
    coded_bits_temp = convenc(info_for_convenc, trellis_structure);
    coded_bits = coded_bits_temp(:);
    modulated_symbols_coded = pskmod(coded_bits, 2, 0, 'gray');
    EsN0_coded_dB = EbN0_dB + 10*log10(code_rate);
    received_symbols_coded = awgn(modulated_symbols_coded, EsN0_coded_dB, 'measured');
    
    % --- 硬判决译码 ---
    demodulated_bits_hard_coded = pskdemod(received_symbols_coded, 2, 0, 'gray');
    decoded_stream_hard = vitdec(demodulated_bits_hard_coded.', trellis_structure, tblen, 'cont', 'hard');
    decoded_stream_hard = decoded_stream_hard.';
    
    % BER计算 (硬判决)
    original_for_ber_hard = info_bits_stream(1:num_ber_bits);
    decoded_for_ber_hard = decoded_stream_hard(tblen+1 : tblen+num_ber_bits);

    if length(decoded_for_ber_hard) == num_ber_bits
        [~, ratio_coded_hard] = biterr(original_for_ber_hard, decoded_for_ber_hard);
        ber_coded_hard(idx_ebn0) = ratio_coded_hard;
    else
        fprintf('警告: EbN0_dB=%.1f dB. 硬判决解码后用于BER比较的比特长度 (%d) 与预期 (%d) 不符!\n', ...
                EbN0_dB, length(decoded_for_ber_hard), num_ber_bits);
        ber_coded_hard(idx_ebn0) = NaN;
    end

    % --- 软判决译码 ---
    % 使用接收到的符号的实部作为Viterbi译码器的软输入 
    soft_inputs_for_vitdec = real(received_symbols_coded);
    decoded_stream_soft = vitdec(soft_inputs_for_vitdec.', trellis_structure, tblen, 'cont', 'unquant');
    decoded_stream_soft = decoded_stream_soft.';
    
    % BER计算 (软判决)
    original_for_ber_soft = info_bits_stream(1:num_ber_bits); % Or info_bits_stream(tblen+1:end)
    decoded_for_ber_soft = decoded_stream_soft(tblen+1 : tblen+num_ber_bits);

    if length(decoded_for_ber_soft) == num_ber_bits
        [~, ratio_coded_soft] = biterr(info_bits_stream(1:num_ber_bits), decoded_stream_soft(tblen+1 : tblen+num_ber_bits));
        ber_coded_soft(idx_ebn0) = ratio_coded_soft;
    else
        fprintf('警告: EbN0_dB=%.1f dB. 软判决解码后用于BER比较的比特长度 (%d) 与预期 (%d) 不符!\n', ...
                EbN0_dB, length(decoded_for_ber_soft), num_ber_bits);
        ber_coded_soft(idx_ebn0) = NaN;
    end
    
    % --- 未编码通信系统 ---
    info_for_uncoded = info_bits_stream(1:num_ber_bits); % 使用流的初始部分进行未编码BER计算
    modulated_symbols_uncoded = pskmod(info_for_uncoded, 2, 0, 'gray');
    EsN0_uncoded_dB = EbN0_dB;
    received_symbols_uncoded = awgn(modulated_symbols_uncoded, EsN0_uncoded_dB, 'measured');
    demodulated_bits_uncoded = pskdemod(received_symbols_uncoded, 2, 0, 'gray');
    [~, ratio_uncoded] = biterr(info_for_uncoded, demodulated_bits_uncoded);
    ber_uncoded(idx_ebn0) = ratio_uncoded;
    
    % 计算理论未编码BPSK BER
    ber_theoretical_bpsk(idx_ebn0) = 0.5 * erfc(sqrt(ebn0_linear));
    
    fprintf('  硬判决编码BER: %e, 软判决编码BER: %e, 未编码BER: %e, 理论未编码BER: %e\n', ...
            ber_coded_hard(idx_ebn0), ber_coded_soft(idx_ebn0), ber_uncoded(idx_ebn0), ber_theoretical_bpsk(idx_ebn0));
end

fprintf('仿真完成.\n\n');

%% 绘制仿真图
figure;

semilogy(EbN0_dB_range, ber_coded_hard, 'bo-', 'LineWidth', 1.5, 'MarkerSize', 6, 'DisplayName', sprintf('编码BPSK (硬判决, K=%d, Cont)', K));
hold on;
semilogy(EbN0_dB_range, ber_coded_soft, 'gd-', 'LineWidth', 1.5, 'MarkerSize', 6, 'DisplayName', sprintf('编码BPSK (软判决, K=%d, Cont)', K));
semilogy(EbN0_dB_range, ber_uncoded, 'rs-', 'LineWidth', 1.5, 'MarkerSize', 6, 'DisplayName', '未编码BPSK');
semilogy(EbN0_dB_range, ber_theoretical_bpsk, 'k--', 'LineWidth', 1, 'DisplayName', '理论未编码BPSK');

grid on;
title(sprintf('卷积码 (K=%d, g=[%d %d]) 软/硬判决(Cont. Mode)与未编码BPSK BER', K, generator_polynomials_octal(1), generator_polynomials_octal(2)));
xlabel('Eb/N0 (dB)');
ylabel('误比特率 (BER)');
legend('show', 'Location', 'southwest');
axis([min(EbN0_dB_range) max(EbN0_dB_range) 1e-7 1]); 

disp('请查看BER性能曲线图 (包含软硬判决对比, continuous Viterbi mode)。'); 
% 保存图像
set(gcf, 'Color', 'w');       
print('ber_performance_soft_hard_cont.png', '-dpng', '-r300');