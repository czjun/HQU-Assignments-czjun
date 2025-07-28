function [decoded_msg, error_detected, error_corrected] = cyclic_decoder(received_codeword, g_poly)
    n = length(received_codeword);
    n_minus_k = length(g_poly) - 1;
    k = n - n_minus_k;

    error_detected = false;
    error_corrected = false;

    % 计算综合征 s(x) = r(x) mod g(x)
    % 这里我们同样手动实现 GF(2) 长除法来计算余数
    temp_received = received_codeword;
    for i = 1:(n - (length(g_poly)-1)) % 或者直接用 k
        if temp_received(i) == 1
            temp_received(i:i+length(g_poly)-1) = bitxor(temp_received(i:i+length(g_poly)-1), g_poly);
        end
    end
    syndrome = temp_received(n - (length(g_poly)-1) + 1 : end); % 最后 n-k 位是综合征
    % syndrome = temp_received(k+1:end);

    corrected_codeword = received_codeword;

    if any(syndrome) % 如果综合征非零，则检测到错误
        error_detected = true;
        

        found_correction = false;
        for err_pos = 1:n
            error_pattern = zeros(1, n);
            error_pattern(err_pos) = 1;
            
          
            temp_error_pattern = error_pattern;
            for i = 1:(n-(length(g_poly)-1))
                if temp_error_pattern(i) == 1
                    temp_error_pattern(i:i+length(g_poly)-1) = bitxor(temp_error_pattern(i:i+length(g_poly)-1), g_poly);
                end
            end
            error_syndrome = temp_error_pattern(k+1:end);
            
            if isequal(error_syndrome, syndrome)
               
                corrected_codeword = bitxor(received_codeword, error_pattern);
                error_corrected = true;
                found_correction = true;
                break;
            end
        end
        
        if ~found_correction
            
        end
    end

   
    decoded_msg = corrected_codeword(1:k);

end
