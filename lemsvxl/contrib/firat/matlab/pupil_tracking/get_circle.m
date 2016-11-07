% This is /lemsvxl/contrib/firat/matlab/pupil_tracking/get_circle.m.

% \file
% \author Firat Kalaycilar (firat@lems.brown.edu)
% \date Mar 8, 2011

function [center, radius, circ_points] = get_circle(I, delta_intensity, x, y, radius)
    C = mean(mean(I(y-2:y+2, x-2:x+2)));
    BW = I > C - delta_intensity & I < C + delta_intensity;
    BW2 = imfill(BW, 'holes');
    L = bwlabel(BW2);
    l = L(y, x);
    BW3 = L == l;
    se = strel('disk',1);
    BW4 = imerode(BW3, se);
    BW5 = BW3 - BW4;
    [row,col] = find(BW5);
    % simple circle fitting    
    points = [col row];
    if exist('radius','var')
        old_center = repmat([x y], length(row), 1);
        points(abs(sum((points - old_center).^2,2) - repmat(radius, length(row),1)) > 10000) = [];        
    end
    if size(points,1) < 2
        center = [x y];
    else
        center = mean(points);
        Center = repmat(center, length(row), 1);
        radius = mean(sqrt(sum((points - Center).^2,2)));
    end    
    angles = ((0:1:359)/180*pi)';
    circ_points = radius * [cos(angles) sin(angles)] + repmat(center, length(angles), 1);    
end
