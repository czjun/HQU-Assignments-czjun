function demod_bits = demodulator(received_symbols)
    % demodulator: 对接收到的 BPSK 符号进行解调 (硬判决)
    % 输入:
    %   received_symbols: 1xL 行向量，代表接收到的 BPSK 符号 (实数值)
    % 输出:
    %   demod_bits: 1xL 行向量，代表解调后的二进制比特 (0 或 1)

    % 判决规则 (对应调制: 0 -> +1, 1 -> -1):
    % 如果 symbol > 0, bit = 0
    % 如果 symbol < 0, bit = 1
    % 如果 symbol = 0, 可以随机判给 0 或 1，或根据具体情况处理
    % 这里简化处理： symbol <= 0 判为 1, >0 判为 0
    
    demod_bits = zeros(size(received_symbols));
    demod_bits(received_symbols <= 0) = 1; % 如果符号 <=0 (即接近 -1), 判为比特 1
    demod_bits(received_symbols > 0) = 0;  % 如果符号 > 0 (即接近 +1), 判为比特 0


end