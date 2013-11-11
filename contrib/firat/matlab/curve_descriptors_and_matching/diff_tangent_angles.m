% This is /lemsvxl/contrib/firat/matlab/curve_descriptors/diff_tangent_angles.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Apr 29, 2011

function theta_prime = diff_tangent_angles(P, is_closed)
    P1 = circshift(P, 1);
    P2 = circshift(P, -1);
    T = P2 - P1;
    theta = atan2(T(:,2),T(:,1));
    bw = theta < 0;
    theta(bw) = theta(bw) + 2*pi;
    theta1 = circshift(theta, 1);
    theta2 = circshift(theta, -1);
    theta_prime = (theta2 - theta1)';
    if ~is_closed
        theta_prime([1 2 (end-1) end]) = 0;
    end
end
