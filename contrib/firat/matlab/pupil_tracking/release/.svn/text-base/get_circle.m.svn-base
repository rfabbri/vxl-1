% This is /lemsvxl/contrib/firat/matlab/pupil_tracking/get_circle.m.

% \file
% \author Firat Kalaycilar (firat@lems.brown.edu)
% \date Mar 10, 2011

function [center, radius, circ_points] = ...
    get_circle(I, x, y, edge_window, num_neighbors, naive_circle, radius_history, center_history, radius)

    points = get_candidate_circle_points(I, x, y, edge_window, num_neighbors);
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
        
        newradius = naive_circle*newradius1+(1-naive_circle)*newradius2;
        newcenter = naive_circle*newcenter1+(1-naive_circle)*newcenter2;
    end 
    lambda1 = 0.2;
    lambda2 = 0.05;    
    try
        radius = radius_history*radius + (1-radius_history)*newradius; 
        center = center_history*[x y] + (1-center_history)*newcenter;
    catch
        radius = newradius;
        center = newcenter;
    end
    angles = ((0:1:359)/180*pi)';
    circ_points = radius * [cos(angles) sin(angles)] + repmat(center, length(angles), 1);     
end
