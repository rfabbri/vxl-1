% This is /lemsvxl/contrib/firat/courses/engn2500/interactive_crop_slices.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date May 12, 2011

function W = interactive_crop_slices(V, init_slice)
    I = double(V(:,:,init_slice));
    fid = figure;
    imagesc(I);colormap gray
    axis image;
    axis off;
    [x,y,b] = ginput(2);
    max_xy = max([x y]);
    min_xy = min([x y]);
    bb = round([min_xy max_xy-min_xy]);
    hold on;
    rectangle('Position', bb, 'EdgeColor', 'r');        
    hold off;
    pause(0.1)
    W = crop_slices(V, bb);
end

