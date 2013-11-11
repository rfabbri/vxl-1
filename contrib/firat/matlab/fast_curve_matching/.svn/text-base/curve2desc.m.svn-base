% This is /lemsvxl/contrib/firat/matlab/fast_curve_matching/curve2desc.m.
% \file
% \author Firat Kalaycilar (firat@lems.brown.edu)
% \date Feb 15, 2011
% C: ordered set of points: M x 2 -> M: the number of points
% K: number of curve smoothing iterations
% N: resampling rate (new number of points)

function S = curve2desc(C, K, N, is_closed)
    C = sample_curve(C, N, is_closed);
    num_points = size(C, 1);
    T = zeros(num_points, num_points);
    if is_closed
        S = zeros(num_points, num_points);
    else
        S = zeros(num_points, num_points-2);
    end
    for i = 1:num_points
        for j = 1:num_points
            V = C(j,:) - C(i,:);
            T(i,j) = atan2(V(2),V(1));
        end
        if is_closed
            R = conv([T(i, end), T(i,:), T(i, 1)], [.5 0 -.5]);
        else
            R = conv(T(i,:), [.5 0 -.5]);
        end       
        S(i,:) = R(3:end-2);        
    end 
end
