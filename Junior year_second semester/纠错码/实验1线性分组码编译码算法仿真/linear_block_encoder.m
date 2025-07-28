function codeword = linear_block_encoder(message, G)
% linear_block_encoder - 实现线性分组码的编码
% 输入:
%   message - 信息比特向量 (1 x k)
%   G - 生成矩阵 (k x n)
% 输出:
%   codeword - 编码后的码字 (1 x n)

% 确保message是行向量
if size(message, 1) > 1
    message = message';
end

% 编码: c = mG
codeword = mod(message * G, 2);
end 