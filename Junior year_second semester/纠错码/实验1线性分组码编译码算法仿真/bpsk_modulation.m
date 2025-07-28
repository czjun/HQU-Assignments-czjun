function modulated_signal = bpsk_modulation(binary_data)
% bpsk_modulation - 实现BPSK调制
% 输入:
%   binary_data - 二进制数据 (0和1)
% 输出:
%   modulated_signal - BPSK调制后的信号 (+1和-1)

% BPSK调制: 0 -> +1, 1 -> -1
modulated_signal = 1 - 2 * binary_data;
end 