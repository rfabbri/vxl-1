% This is /lemsvxl/contrib/firat/matlab/pupil_tracking/get_circle3.m.

% \file
% \author Firat Kalaycilar (firat@lems.brown.edu)
% \date Mar 10, 2011

function [center, radius, circ_points] = get_circle3(I, delta_intensity, x, y, radius)
    points = get_candidate_circle_points(I, x, y, delta_intensity);
    num_points = size(points,1);
    if num_points < 2
        newcenter = [x y];
        newradius = radius;
    else
        
        newcenter1 = mean(points);
        Center = repmat(newcenter1, num_points, 1);
        newradius1 = mean(sqrt(sum((points - Center).^2,2)));
        
        Par = CircleFitByTaubin(points);
        newcenter2 = Par(1:2);
        newradius2 = Par(3);
        
        newradius = (newradius1+newradius2)/2;
        newcenter = (newcenter1+newcenter2)/2;
    end 
    lambda1 = 0.2;
    lambda2 = 0.05;    
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
