% This is /lemsvxl/contrib/firat/matlab/pupil_tracking/post_process_frames.m.

% \file
% \author Firat Kalaycilar (firat@lems.brown.edu)
% \date Mar 8, 2011

function bin_mov = post_process_frames(mov, min_thresh, max_thresh)
    [h,w,f] = size(mov);
    bin_mov = zeros(h,w,f);
    for i = 1:f
        bin_mov(:,:,i) = imfill( min_thresh < mov(:,:,i) & mov(:,:,i) < max_thresh, 'holes')
    end
end
