% This is get_circle.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Mar 10, 2011
% \update Feb 16, 2012

% Copyright (c) 2011 
% Firat Kalaycilar
% Benjamin Kimia
% The Laboratory for Engineering Man/Machine Systems (LEMS)
% School of Engineering
% Brown University
% http://vision.lems.brown.edu

function [center, radius, circ_points] = ...
    get_circle(I, x, y, Rmin, Rmax, radius_history, center_history, ...
    num_votes, radius, search_mask)
    
    s = Rmax + 3;
    points = get_candidate_circle_points(I, x, y, s, search_mask);
    num_points = size(points,1);
    if num_points < 3
        newcenter = [x y];
        newradius = radius;
    else        
       [newcenter, newradius] = ...
           hough_circle(points, size(I,1), size(I,2), Rmin, Rmax, num_votes);        
    end 
    if ~isempty(radius)
        radius = radius_history*radius + (1-radius_history)*newradius; 
        center = center_history*[x y] + (1-center_history)*newcenter;
    else
        radius = newradius;
        center = newcenter;
    end
    angles = ((0:8:352)/180*pi)';
    circ_points = radius * [cos(angles) sin(angles)] ...
        + repmat(center, length(angles), 1);     
end
