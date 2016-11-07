% This is /lemsvxl/contrib/firat/matlab/pupil_tracking/tracker2.m.

% \file
% \author Firat Kalaycilar (firat@lems.brown.edu)
% \date Mar 8, 2011

function tracker2(mov, index, delta_intensity)
    if ischar(mov)
        mov = read_video(mov, 256, 256);
    end
    figure;
    imagesc(mov(:,:,index)); colormap gray; axis image; axis off;
    [x,y] = ginput(1);
    x = round(x); y = round(y);
    [center, radius, circ_points] = get_circle(mov(:,:,index), delta_intensity, x, y);
    hold on
    plot(circ_points(:,1), circ_points(:,2), 'r.')   
    ginput(1);
    %% initial circle found
    %% bbox1 = [minx miny maxx maxy]
    bbox1 = [floor(center - radius)-3, ceil(center + radius)+3];
    %% bbox = [minx miny width height]
    bbox = [bbox1(1:2) bbox1(3:4)-bbox1(1:2)];    
    int_hist = imhist(uint8(imcrop(mov(:,:,index), bbox)),100);    
    norm_hist = int_hist/norm(int_hist);
    %%start tracking
    circles = zeros([size(circ_points) size(mov,3)]);
    circles(:,:,index) = circ_points;    
    
    old_bbox = bbox;
    old_center = center;
    old_radius = radius;
    old_norm_hist = norm_hist;    
        
    for f_index = index+1:size(mov,3)               
        max_similarity = -1;        
        for dminx = -1:1
            for dminy = -1:1
                for dsize = -1:1
                    bb = bbox + [dminx dminy dsize dsize];
                    int_hist = imhist(uint8(imcrop(mov(:,:,f_index), bb)),100);
                    similarity = sum(int_hist .* norm_hist)/norm(int_hist);
                    if similarity > max_similarity
                        max_similarity = similarity;
                        max_sim_index = [dminx dminy dsize];
                        max_sim_bb = bb;                        
                    end
                end
            end
        end
        center = center + max_sim_index(1:2);
        radius = radius + max_sim_index(3);
        angles = ((0:1:359)/180*pi)';
        circles(:,:,f_index) = radius * [cos(angles) sin(angles)] + repmat(center, length(angles), 1);          
        bbox = max_sim_bb;                         
    end
    
    
    for f_index = index-1:-1:1               
                
    end
    
    for f_index = 1:size(mov,3)
        circ_points = circles(:,:,f_index);
        imagesc(mov(:,:,f_index)); colormap gray; axis image; axis off; hold on;
        plot(circ_points(:,1), circ_points(:,2), 'r.'); hold off
        pause(0.05)
    end
end
