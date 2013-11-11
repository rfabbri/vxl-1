% This is hough_circle.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Mar 12, 2011
% \update Mar 18, 2011

% Copyright (c) 2011 
% Firat Kalaycilar
% Benjamin Kimia
% The Laboratory for Engineering Man/Machine Systems (LEMS)
% School of Engineering
% Brown University
% http://vision.lems.brown.edu

% circle detection using hough transform

function [center, radius] = hough_circle(points, H, W, Rmin, Rmax, num_votes)    
    angle_increment = 360 / num_votes;    
    angles = ((0:angle_increment:(360-angle_increment))/180*pi)';
    cos_angles = cos(angles);
    sin_angles = sin(angles);
    cos_sin_angles = [cos_angles sin_angles];
    num_angles = length(angles);
    r_values = Rmin:Rmax;
    accum = zeros(H, W, length(r_values));
    max_val = -Inf;
    max_index = [];
    for p_index = 1:size(points,1)
        p = points(p_index,:);
        P = repmat(p, num_angles, 1);
        for r_index = 1:length(r_values)
            r = r_values(r_index);
            C = round(r * cos_sin_angles + P); 
            for t = 1:num_angles
                try
                    accum(C(t, 2), C(t, 1), r_index) = ...
                        accum(C(t, 2), C(t, 1), r_index) + 1;
                    if accum(C(t, 2), C(t, 1), r_index) > max_val
                        max_val = accum(C(t, 2), C(t, 1), r_index);
                        max_index = [C(t, 1) C(t, 2) r_index];
                    end
                catch
                    
                end
            end
        end
    end
    center = max_index(1:2);
    radius = r_values(max_index(3));    
end
