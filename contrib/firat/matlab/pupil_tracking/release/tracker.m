% This is /lemsvxl/contrib/firat/matlab/pupil_tracking/tracker.m.

% \file
% \author Firat Kalaycilar (firat@lems.brown.edu)
% \date Mar 8, 2011

function [circles, centers, radii] = ...
    tracker(mov, smov, index, x, y, edge_window, num_neighbors, naive_circle, radius_history, center_history)

    [center, radius, circ_points] = get_circle(smov(:,:,index), x, y, edge_window, num_neighbors, naive_circle, radius_history, center_history);
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
        [center, radius, circ_points] = get_circle(smov(:,:,f_index), x, y, edge_window, num_neighbors, naive_circle, radius_history, center_history, radius);
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
        [center, radius, circ_points] = get_circle(smov(:,:,f_index), x, y, edge_window, num_neighbors, naive_circle, radius_history, center_history, radius);
        ox = round(center(1));
        oy = round(center(2));
        or = radius;
        circles(:,:,f_index) = circ_points;   
        centers(f_index,:) = center;
        radii(f_index) = radius;     
    end    
end
