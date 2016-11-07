% This is /lemsvxl/contrib/firat/matlab/fast_curve_matching/curve2signal.m.

% \file
% \author Firat Kalaycilar (firat@lems.brown.edu)
% \date Feb 1, 2011

function S = curve2signal(C, K, N)
    C = sample_curve(C, K, N);
    num_points = size(C, 1);
    T = zeros(num_points, num_points);
    S = zeros(num_points, num_points-2);
    for i = 1:num_points
        for j = 1:num_points
            V = C(j,:) - C(i,:);
            T(i,j) = atan2(V(2),V(1));
        end
        R = conv(T(i,:), [.5 0 -.5]);
        %whos
        S(i,:) = R(3:end-2);
        %plot(S(i,:)); axis([-Inf Inf -1.5 1.5])   
        %pause(.2)
    end 
end
