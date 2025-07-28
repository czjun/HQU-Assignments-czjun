function [G, H] = generate_linear_code_matrices(n, k, method)
% generate_linear_code_matrices - 生成线性分组码的生成矩阵和校验矩阵
% 输入:
%   n - 码字长度
%   k - 信息位长度
%   method - 指定矩阵生成方法: 'hamming', 'custom', 或者其他
% 输出:
%   G - 生成矩阵 (k x n)
%   H - 校验矩阵 (n-k) x n

if strcmp(method, 'hamming') && n == 7 && k == 4
    % 生成(7,4)Hamming码的生成矩阵和校验矩阵
    % 生成矩阵G (系统形式)
    G = [eye(k), zeros(k, n-k)];  % 初始化为[I | 0]
    
    % 设置奇偶校验位
    G(1, 5) = 1; G(1, 6) = 1; G(1, 7) = 1;
    G(2, 5) = 1; G(2, 6) = 1; G(2, 7) = 0;
    G(3, 5) = 1; G(3, 6) = 0; G(3, 7) = 1;
    G(4, 5) = 0; G(4, 6) = 1; G(4, 7) = 1;
    
    % 校验矩阵H (系统形式)
    H = [G(:, k+1:n)', eye(n-k)];
    
elseif strcmp(method, 'custom')
    % 自定义生成矩阵 - 示例使用(6,3)线性码
    if n == 6 && k == 3
        % 生成矩阵G (系统形式 [I|P])
        G = [eye(k), zeros(k, n-k)];
        G(1, 4) = 1; G(1, 5) = 0; G(1, 6) = 1;
        G(2, 4) = 1; G(2, 5) = 1; G(2, 6) = 0;
        G(3, 4) = 0; G(3, 5) = 1; G(3, 6) = 1;
        
        % 校验矩阵H ([P^T|I])
        H = [G(:, k+1:n)', eye(n-k)];
    else
        error('还未实现该尺寸的自定义码');
    end
    
else
    % 默认使用随机方法生成矩阵
    % 确保G的形式为 [I | P]
    G = [eye(k), rand(k, n-k) > 0.5];
    
    % 确保H的形式为 [P^T | I]
    H = [G(:, k+1:n)', eye(n-k)];
end

% 确保所有运算在GF(2)上
G = mod(G, 2);
H = mod(H, 2);

% 验证 G*H^T = 0 (在GF(2)上)
if any(any(mod(G * H', 2)))
    warning('生成的矩阵不满足G*H^T = 0的条件');
end
end 