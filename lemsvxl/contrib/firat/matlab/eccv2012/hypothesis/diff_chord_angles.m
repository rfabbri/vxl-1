% This is diff_chord_angles.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Apr 29, 2011

function alpha_prime = diff_chord_angles(P, is_closed, indices)
    M = size(P, 1);    
    if is_closed
        num_rows = M;
        alpha_prime = zeros(M, M);
        indices = 1:M;
    else           
        if ~exist('indices','var')
            indices = 1:2;
        end
        num_rows = length(indices);  
        alpha_prime = zeros(num_rows, M);
    end
    for j = 1:num_rows
        i = indices(j);
        T = P - repmat(P(i,:),M,1);
        alpha = atan2(T(:,2),T(:,1));
        bw = alpha < 0;
        alpha(bw) = alpha(bw) + 2*pi;
        alpha1 = circshift(alpha, 1);
        alpha2 = circshift(alpha, -1);
        alpha_prime(j,:) = alpha2 - alpha1;            
    end 
    if ~is_closed
        alpha_prime(:,[1 end]) = 0;
    end 
end

