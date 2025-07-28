function noisy_symbols = channel_simulator(tx_symbols, EbN0_dB, k_bits, n_bits)
    % channel_simulator: 模拟 AWGN 信道
    Es = mean(abs(tx_symbols).^2); % 符号能量, 对于BPSK的+1/-1, Es = 1
    % 计算码率
    R = k_bits / n_bits;
    EbN0_linear = 10^(EbN0_dB / 10);
    EsN0_linear = EbN0_linear * R;
    sigma_squared = Es / (2 * EsN0_linear); 
    noise_std_dev = sqrt(sigma_squared);
    % 生成高斯噪声
    noise = noise_std_dev * randn(size(tx_symbols));
    
    noisy_symbols = tx_symbols + noise;

end

% % 示例用法:
% symbols = modulator([0 1 0 1 1 0]); % BPSK: [1 -1 1 -1 -1 1]
% EbN0_dB_val = 5;      % 假设 Eb/N0 = 5 dB
% k = 4; % 信息位数
% n = 7; % 编码后总位数 (对于(7,4)码)
% 
% % 如果是未编码系统，k=n (或 R=1)
% % noisy_sym_uncoded = channel_simulator(symbols, EbN0_dB_val, length(symbols), length(symbols));
% % disp(['原始符号: ', num2str(symbols)]);
% % disp(['未编码Eb/N0=',num2str(EbN0_dB_val),'dB后含噪符号: ', num2str(noisy_sym_uncoded)]);
% 
% % 假设这些符号是 (7,4) 编码后的序列的一部分 (长度应为7的倍数，这里仅作示例)
% % 为了演示，我们假设这6个符号是某个7比特码字的一部分，或者直接用k=6,n=6代表未编码
% noisy_sym_coded = channel_simulator(symbols, EbN0_dB_val, k, n); % R = 4/7
% disp(['原始符号: ', num2str(symbols)]);
% disp(['编码(R=',num2str(k/n),')后Eb/N0=',num2str(EbN0_dB_val),'dB后含噪符号: ', num2str(noisy_sym_coded)]);
% 
% % 比较：如果直接使用SNR (Es/N0)
% % SNR_dB = EbN0_dB_val + 10*log10(k/n);
% % noisy_sym_awgn_func = awgn(symbols, SNR_dB, 'measured'); % MATLAB's awgn
% % disp(['MATLAB awgn func (SNR=',num2str(SNR_dB),'dB) 输出: ', num2str(noisy_sym_awgn_func)]); 