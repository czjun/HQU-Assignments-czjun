function encoded_bits = cyclic_encoder(msg_bits, g_poly)
    % cyclic_encoder: 对输入的信息比特进行循环编码
    % 输入:
    %   msg_bits: 1xK 行向量，代表 K 位信息比特 (例如 [1 0 1 0])
    %   g_poly: 1x(N-K+1) 行向量，代表生成多项式的系数 (例如 [1 0 1 1])
    % 输出:
    %   encoded_bits: 1xN 行向量，代表 N 位编码后的比特

    k = length(msg_bits);
    n_minus_k = length(g_poly) - 1;
    n = k + n_minus_k;

    % 将信息比特构造成多项式形式 (高位在前)
    msg_poly = msg_bits;

    % 构造被除数: m(x) * x^(n-k)
    dividend = [msg_poly, zeros(1, n_minus_k)];

    % 执行多项式除法 (GF(2)运算)  
    remainder = dividend;
    for i = 1:k
        if remainder(i) == 1
            remainder(i:i+n_minus_k) = bitxor(remainder(i:i+n_minus_k), g_poly);
        end
    end
    
    % 提取余数 (最后 n-k 位)
    parity_bits = remainder(k+1:end);
    
    % 构造系统码: c(x) = m(x) * x^(n-k) + r(x)，对于系统码是 [校验位 信息位]
    encoded_bits = [msg_bits, parity_bits];

    % 确保输出是期望的长度
    if length(encoded_bits) ~= n
        warning('编码后长度与预期不符');
    end

end

