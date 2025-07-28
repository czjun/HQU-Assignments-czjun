function bpsk_symbols = modulator(bit_stream)
    % modulator: 将比特流进行 BPSK 调制
    % 输入:
    %   bit_stream: 1xL 行向量，代表 L 个二进制比特 (0 或 1)
    % 输出:
    %   bpsk_symbols: 1xL 行向量，代表 BPSK 调制后的符号 (+1, -1)

    % 映射规则: 0 -> +1, 1 -> -1
    % 也可以使用 0 -> -1, 1 -> +1，这只影响绝对相位，不影响 BER性能
    bpsk_symbols = 1 - 2 * bit_stream; % 当 bit_stream 为 0 时, 1-0 = 1; 当 bit_stream 为 1 时, 1-2 = -1

    % 另一种常见的映射: 0 -> -1, 1 -> +1
    % bpsk_symbols = 2 * bit_stream - 1; % 当 bit_stream 为 0 时, 0-1 = -1; 当 bit_stream 为 1 时, 2-1 = +1
end

% % 示例用法:
% bits = [0 1 0 0 1 1 0];
% symbols = modulator(bits);
% disp(['原始比特: ', num2str(bits)]);
% disp(['BPSK符号: ', num2str(symbols)]); 