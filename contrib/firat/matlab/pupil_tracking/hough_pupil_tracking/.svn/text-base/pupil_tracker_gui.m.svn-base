% This is pupil_tracker_gui.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Mar 9, 2011
% \update Feb 20, 2012

% Copyright (c) 2011 
% Firat Kalaycilar
% Benjamin Kimia
% The Laboratory for Engineering Man/Machine Systems (LEMS)
% School of Engineering
% Brown University
% http://vision.lems.brown.edu

% parameters
% video_folder: the path of the folder containing the avi files
% mat_folder: the path of the folder containing the mat files
% image_max_size: (Default: 256) 
% num_votes: an integer between 1-60 (Default: 45)
%            num_votes < 45 is faster but less reliable 
% delta_radius: (Default: 0) delta_radius > 0 is safer but slower
% radius_history: a real number between 0 and 1 (Default: 0.999)
% center_history: a real number between 0 and 1 (Default: 0.75)
% video_name: a string like 'JackieBottom4'
% index: indicates the frame to be used for initialization 

function [centers, radii, mov] = pupil_tracker_gui...
    (video_folder, mat_folder, image_max_size, ...
    num_votes, delta_radius, radius_history, ...
    center_history, video_name, index)
    
    fprintf('Loading...\n');
    tic;
    mat_file = [mat_folder '/' video_name '_' num2str(image_max_size) '.mat'];
    video_file = [video_folder '/' video_name '.avi'];
    if ~exist(mat_file, 'file')        
        mov = read_video(video_file, image_max_size, image_max_size); 
        save(mat_file, 'mov', '-v7.3');
    else
        load(mat_file);        
    end 
    toc
    num_frames = size(mov,3);
    fprintf('Total number of frames: %d\n', num_frames);
    fprintf('User interaction...\n');
    tic;
    button = 3;
    index = index - 40;
    while button == 3
        index = index + 40;
        close all        
        figure;
        imagesc(mov(:,:,index)); colormap gray; axis image; axis off;
        title('Left click to pick a point inside the pupil')       
        [x,y] = ginput(1);
        x = round(x); y = round(y);
        hold on;
        plot(x,y,'g.')
        [x1,y1] = ginput(1);        
        x1 = round(x1); y1 = round(y1);
        plot(x1, y1, 'r.')
        hold off
        %[init_center, init_radius, circ_points] = get_circle(mov(:,:,index), ...
        %    x, y, round(6*image_max_size/256), round(20*image_max_size/256), ...
        %    radius_history, center_history, num_votes, [], []);
        init_radius = sqrt((x1-x)^2+(y1-y)^2);
        init_center = [x,y];        
        %plot(circ_points(:,1), circ_points(:,2), 'r.')   	        
        [ign, ign, button] = ginput(1);
    end
    toc
    fprintf('Tracking...\n');    
    Rmin = round(init_radius - delta_radius);
    Rmax = round(init_radius + delta_radius);
    title('Tracking...Please wait')
    drawnow
    tic;
    [circles, centers, radii] = ...
        tracker(mov, index, init_center(1), init_center(2),...
        Rmin, Rmax, radius_history, center_history, num_votes);    
    time_elapsed = toc;
    fprintf('Tracking speed = %f frames/second\n', num_frames/time_elapsed);
    close all;
    %fprintf('Playing tracking video...\n');
    %tic;
    %for f_index = 1:num_frames
    %    circ_points = circles(:,:,f_index);
    %    imagesc(mov(:,:,f_index)); colormap gray; axis image; axis off; hold on;        
    %    plot(circ_points([1:end 1],1), circ_points([1:end 1],2), 'r'); hold off
    %    drawnow
    %end
    %toc
end
