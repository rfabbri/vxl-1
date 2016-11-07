% This is /lemsvxl/contrib/firat/matlab/pupil_tracking/resize_all_videos.m.

% \file
% \author Firat Kalaycilar (firat@lems.brown.edu)
% \date Mar 10, 2011

addpath ~/lemsvxl/src/contrib/firat/matlab/
files = dir('/vision/video/eye_tracking/*.avi');
mat_folder = '/vision/video/eye_tracking/mat';
video_folder = '/vision/video/eye_tracking';
for i = 1:length(files)
    video_name = get_objectname_from_filename(files(i).name)
    mat_file = [mat_folder '/' video_name '.mat'];
    if ~exist(mat_file, 'file')
        video_file = [video_folder '/' video_name '.avi'];
        mov = read_video(video_file, 256, 256); 
        smov = smooth_movie(mov, 1);
        save(mat_file, 'mov', 'smov');
    end
end
