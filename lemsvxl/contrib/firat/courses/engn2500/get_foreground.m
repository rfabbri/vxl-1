% This is /lemsvxl/contrib/firat/courses/engn2500/get_foreground.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Apr 21, 2011

function [F,C] = get_foreground(V, t, init_slice)
    I = double(V(:,:,init_slice));
    F = zeros(size(V));
    C = zeros(size(V));
    fid = figure;
    imagesc(I);colormap gray
    axis image;
    axis off;
    [x,y,b] = ginput(2);
    max_xy = max([x y]);
    min_xy = min([x y]);
    bb = [min_xy max_xy-min_xy];
    hold on;
    rectangle('Position', bb, 'EdgeColor', 'r');    
    pause(0.1)
    P = imcrop(I, bb);
    P = P(:);    
    mu = mean(P);
    sigma = std(P,1);    
    for i = 1:size(V,3)
        I = double(V(:,:,i));
        I((I - mu)/sigma < t) = -Inf;
        if i == 55 && 0
        figure; imagesc(I > -Inf); colormap gray; axis image; axis off
        end
        I = imfill(I > -Inf,'holes');
        if i == 55 && 0
        figure; imagesc(I); colormap gray; axis image; axis off
        end
        I = medfilt2(I);
        if i == 55 && 0
        figure; imagesc(I); colormap gray; axis image; axis off
        end
        se = strel('disk',1);
        I2 = imerode(I,se); 
        if i == 55 && 0
        figure; imagesc(I2); colormap gray; axis image; axis off
        end       
        F(:,:,i) = I;
        C(:,:,i) = I - I2;    
        if i == 55 && 0
        figure; imagesc(C(:,:,i)); colormap gray; axis image; axis off
        end         
    end   
    close(fid);
    show_slices(F, .1);
end

