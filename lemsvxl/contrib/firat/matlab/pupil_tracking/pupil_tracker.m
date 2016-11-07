% This is /lemsvxl/contrib/firat/matlab/pupil_tracking/pupil_tracker.m.

% \file
% \author Firat Kalaycilar (firat@lems.brown.edu)
% \date Mar 9, 2011

function pupil_tracker(video_name, video_folder, mat_folder)
    mat_file = [mat_folder '/' video_name '.mat'];
    if ~exist(mat_file, 'file')
        video_file = [video_folder '/' video_name '.avi'];
        mov = read_video(video_file, 256, 256); 
        smov = smooth_movie(mov, 1);
        save(mat_file, 'mov', 'smov');
    else
        load(mat_file);
    end
    tracker(mov, smov, 2);
end
