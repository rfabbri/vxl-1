% This is /lemsvxl/contrib/firat/matlab/pupil_tracking/tracker.m.

% \file
% \author Firat Kalaycilar (firat@lems.brown.edu)
% \date Mar 8, 2011

function tracker(mov, smov, delta_intensity)
    button = 3;
    index = -9;
    while button == 3
        index = index + 10;
        close all        
        figure;
        imagesc(mov(:,:,index)); colormap gray; axis image; axis off;
        title('Left click to pick a point inside the pupil')       
        [x,y] = ginput(1);
        x = round(x); y = round(y);
        [center, radius, circ_points] = get_circle3(smov(:,:,index), delta_intensity, x, y);
        hold on
        plot(circ_points(:,1), circ_points(:,2), 'r.')   
        hold off        
        [ign, ign, button] = ginput(1);
    end
    title('Tracking...Please wait')
    pause(.1)
    %%start tracking
    circles = zeros([size(circ_points) size(mov,3)]);
    circles(:,:,index) = circ_points;    
    
    px = round(center(1));
    py = round(center(2));
    pr = radius;
    
    ox = px; oy = py; or = pr;    
    for f_index = index+1:size(mov,3)               
        x = ox;
        y = oy;
        radius = or;
        [center, radius, circ_points] = get_circle3(smov(:,:,f_index), delta_intensity, x, y, radius);
        ox = round(center(1));
        oy = round(center(2));
        or = radius;
        circles(:,:,f_index) = circ_points;        
    end
    
    ox = px; oy = py; or = pr;
    for f_index = index-1:-1:1               
        x = ox;
        y = oy;
        radius = or;
        [center, radius, circ_points] = get_circle3(smov(:,:,f_index), delta_intensity, x, y, radius);
        ox = round(center(1));
        oy = round(center(2));
        or = radius;
        circles(:,:,f_index) = circ_points;        
    end
    for f_index = 1:size(mov,3)
        circ_points = circles(:,:,f_index);
        imagesc(mov(:,:,f_index)); colormap gray; axis image; axis off; hold on;        
        plot(circ_points(:,1), circ_points(:,2), 'r.'); hold off
        pause(0.01)
    end
end
