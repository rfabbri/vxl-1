% This is /lemsvxl/contrib/firat/matlab/pupil_tracking/read_video.m.

% \file
% \author Firat Kalaycilar (firat@lems.brown.edu)
% \date Mar 8, 2011

function mov = read_video(video_path, max_h, max_w)
    orig_mov = aviread(video_path);
    I = orig_mov(1).cdata;
    [h,w] = size(I);
    rh = max_h / h; rw = max_w / w;
    rmin = min([rh rw]);    
    if rmin < 1
        I = imresize(I, rmin, 'bicubic');
    end
    [h,w] = size(I);
    f = length(orig_mov);
    mov = zeros(h,w,f);
    mov(:,:,1) = I;
    for i = 2:f
        I = orig_mov(i).cdata;
        if rmin < 1
            I = imresize(I, rmin, 'bicubic');
        end
        mov(:,:,i) = I;
    end    
end
