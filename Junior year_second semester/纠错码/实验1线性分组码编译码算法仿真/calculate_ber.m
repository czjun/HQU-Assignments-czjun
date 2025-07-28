function ber = calculate_ber(original_data, decoded_data)
% calculate_ber - 计算误比特率
% 输入:
%   original_data - 原始发送的数据
%   decoded_data - 接收端解码后的数据
% 输出:
%   ber - 误比特率 (错误比特数/总比特数)

% 确保两个向量长度相同
if length(original_data) ~= length(decoded_data)
    error('原始数据和解码数据长度不一致');
end

% 计算误比特数
num_errors = sum(original_data ~= decoded_data);

% 计算误比特率
ber = num_errors / length(original_data);
end 