% This is /lemsvxl/contrib/firat/matlab/pupil_tracking/pupil_tracker_gui.m.

% \file
% \author Firat Kalaycilar (firat@lems.brown.edu)
% \date Mar 9, 2011

% parameters
% edge_window: local edge detection window size (Default: 25)
% num_neighbors: number of nearest neighbors to detect pupil contour (Default: 70)
% naive_circle: contribution of the naive circle fitting algorithm compared to Taubin's method ([0,1]) (Default: 0.5)
% radius_history: 0 -> don't remember previous radii, 1 -> always use the initial radius. (Default: 0.95)
% center_history: 0 -> don't remember previous centers, 1 -> always use the initial center. (Default: 0.8)


function pupil_tracker_gui(...
    video_folder, mat_folder, edge_window, num_neighbors, naive_circle, radius_history, center_history, video_name, index)

    mat_file = [mat_folder '/' video_name '.mat'];
    if ~exist(mat_file, 'file')
        video_file = [video_folder '/' video_name '.avi'];
        mov = read_video(video_file, 256, 256); 
        smov = smooth_movie(mov, 1);
        save(mat_file, 'mov', 'smov');
    else
        load(mat_file);
    end
    button = 3;
    index = index - 10;
    while button == 3
        index = index + 10;
        close all        
        figure;
        imagesc(mov(:,:,index)); colormap gray; axis image; axis off;
        title('Left click to pick a point inside the pupil')       
        [x,y] = ginput(1);
        x = round(x); y = round(y);
        [center, radius, circ_points] = get_circle(smov(:,:,index), x, y, edge_window, num_neighbors, naive_circle, radius_history, center_history);
        hold on
        plot(circ_points(:,1), circ_points(:,2), 'r.')   
        hold off        
        [ign, ign, button] = ginput(1);
    end
    title('Tracking...Please wait')
    pause(.1)
    [circles, centers, radii] = tracker(mov, smov, index, x, y, edge_window, num_neighbors, naive_circle, radius_history, center_history);
    for f_index = 1:size(mov,3)
        circ_points = circles(:,:,f_index);
        imagesc(mov(:,:,f_index)); colormap gray; axis image; axis off; hold on;        
        plot(circ_points(:,1), circ_points(:,2), 'r.'); hold off
        pause(0.01)
    end
end
