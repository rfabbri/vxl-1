% This is /lemsvxl/contrib/firat/matlab/pupil_tracking/get_circle2.m.

% \file
% \author Firat Kalaycilar (firat@lems.brown.edu)
% \date Mar 9, 2011

function [center, radius, circ_points] = get_circle2(I, delta_intensity, x, y, radius)
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
        newcenter = [x y];
        newradius = radius;
    else
        if(1)
        newcenter = mean(points);
        Center = repmat(newcenter, length(row), 1);
        newradius = mean(sqrt(sum((points - Center).^2,2)));
        else
        Par = CircleFitByTaubin(points);
        newcenter = Par(1:2);
        newradius = Par(3);
        end
    end 
    lambda1 = 0.5;
    lambda2 = 0.5;    
    try
        radius = (1-lambda2)*radius + lambda2*newradius; 
        center = (1-lambda1)*[x y] + lambda1*newcenter;
    catch
        radius = newradius;
        center = newcenter;
    end
    angles = ((0:1:359)/180*pi)';
    circ_points = radius * [cos(angles) sin(angles)] + repmat(center, length(angles), 1);  
     
end
