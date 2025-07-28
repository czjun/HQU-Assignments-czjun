function decoded_message = linear_block_decoder(received_codeword, H, G)
% linear_block_decoder - 实现线性分组码的译码
% 输入:
%   received_codeword - 接收到的码字 (1 x n)
%   H - 校验矩阵 (m x n)
%   G - 生成矩阵 (k x n)
% 输出:
%   decoded_message - 译码后的信息比特 (1 x k)

% 确保received_codeword是行向量
if size(received_codeword, 1) > 1
    received_codeword = received_codeword';
end

% 计算校验方程的结果（症状）
syndrome = mod(received_codeword * H', 2);

% 如果症状全为零，则接收码字无错误
if all(syndrome == 0)
    % 从码字中提取信息位
    k = size(G, 1);  % 信息位长度
    % 假设G是系统形式，即G = [I P]，其中I是k×k单位矩阵
    decoded_message = received_codeword(1:k);
else
    % 错误图样查找: 实现标准阵列译码
    % 这里使用简化的方法：
    % 对所有可能的单比特错误图样，计算其症状，找到匹配的
    n = length(received_codeword);
    error_found = false;
    
    % 检查每个可能的单比特错误
    for i = 1:n
        error_pattern = zeros(1, n);
        error_pattern(i) = 1;
        
        % 计算错误图样的症状
        error_syndrome = mod(error_pattern * H', 2);
        
        % 如果症状匹配，纠正错误
        if isequal(error_syndrome, syndrome)
            corrected_codeword = mod(received_codeword + error_pattern, 2);
            % 从码字中提取信息位
            k = size(G, 1);
            decoded_message = corrected_codeword(1:k);
            error_found = true;
            break;
        end
    end
    
    % 如果找不到匹配的单比特错误，尝试按信息位进行硬判决
    if ~error_found
        k = size(G, 1);
        decoded_message = received_codeword(1:k);
    end
end
end 