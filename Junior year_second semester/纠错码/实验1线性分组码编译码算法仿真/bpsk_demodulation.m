function binary_data = bpsk_demodulation(received_signal)
% bpsk_demodulation - 实现BPSK解调
% 输入:
%   received_signal - 接收到的BPSK信号
% 输出:
%   binary_data - 解调后的二进制数据 (0和1)

% BPSK解调: 正值 -> 0, 负值 -> 1
binary_data = received_signal < 0;
end 