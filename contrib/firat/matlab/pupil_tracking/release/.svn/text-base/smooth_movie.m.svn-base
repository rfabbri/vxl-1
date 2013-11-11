% This is /lemsvxl/contrib/firat/matlab/pupil_tracking/smooth_movie.m.

% \file
% \author Firat Kalaycilar (firat@lems.brown.edu)
% \date Mar 9, 2011

function mov = smooth_movie(mov, sigma)
    h = fspecial('gaussian', 7, sigma); 
    for f_index = 1:size(mov,3)
        mov(:,:,f_index) = conv2(mov(:,:,f_index), h, 'same');
    end 
end
