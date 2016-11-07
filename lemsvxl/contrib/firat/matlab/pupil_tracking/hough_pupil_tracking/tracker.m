% This is tracker.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Mar 8, 2011
% \update Feb 16, 2012

% Copyright (c) 2011 
% Firat Kalaycilar
% Benjamin Kimia
% The Laboratory for Engineering Man/Machine Systems (LEMS)
% School of Engineering
% Brown University
% http://vision.lems.brown.edu

% the main tracking code

function [circles, centers, radii] = ...
    tracker(mov, index, x, y, Rmin, Rmax, radius_history, ...
    center_history, num_votes)
    
    %%added to increase performance!
    s = Rmax + 3;
    Z = zeros(2*s+1, 2*s+1);
	Z(s+1, s+1) = 1;
	DT = bwdist(Z);
	search_mask = DT >= Rmin & DT <= Rmax;

    [center, radius, circ_points] = get_circle(mov(:,:,index), x, y,...
        Rmin, Rmax, radius_history, center_history, num_votes, [], search_mask);
    %%start tracking
    circles = zeros([size(circ_points) size(mov,3)]);
    centers = zeros(size(mov,3), 2);
    radii = zeros(1, size(mov,3));
    
    circles(:,:,index) = circ_points;
    centers(index,:) = center;
    radii(index) = radius;    
    
    px = round(center(1));
    py = round(center(2));
    pr = radius;
    
    ox = px; oy = py; or = pr;    
    for f_index = index+1:size(mov,3)               
        x = ox;
        y = oy;
        radius = or;
        [center, radius, circ_points] = get_circle(mov(:,:,f_index), ...
            x, y, Rmin, Rmax, radius_history, center_history, num_votes, ...
            radius, search_mask);
        ox = round(center(1));
        oy = round(center(2));
        or = radius;
        circles(:,:,f_index) = circ_points;
        centers(f_index,:) = center;
        radii(f_index) = radius;        
    end
    
    ox = px; oy = py; or = pr;
    for f_index = index-1:-1:1               
        x = ox;
        y = oy;
        radius = or;
        [center, radius, circ_points] = get_circle(mov(:,:,f_index), ...
            x, y, Rmin, Rmax, radius_history, center_history, num_votes, ...
            radius, search_mask);
        ox = round(center(1));
        oy = round(center(2));
        or = radius;
        circles(:,:,f_index) = circ_points;   
        centers(f_index,:) = center;
        radii(f_index) = radius;     
    end    
end
