% This is /lemsvxl/contrib/firat/courses/engn2500/visualize_foreground.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Apr 21, 2011


function visualize_foreground(F, isovalue)
    skin_color = [170 170 240]/255;
    %V = smooth3(F, 'gaussian', 3);
    V = F;
    [num_y, num_x, num_z] = size(V);
    Orig_x = floor((num_x+1)/2);
    Orig_y = floor((num_y+1)/2);
    Orig_z = floor((num_z+1)/2);
    x = ((1:num_x) - Orig_x);
    y = ((1:num_y) - Orig_y);
    z = ((1:num_z) - Orig_z);
    figure;
    IS = isosurface(x, y, z, V, isovalue);
    P = patch(IS);
    isonormals(x, y, z, V, P);
    set(P, 'FaceColor', skin_color, 'EdgeColor', 'none');
    daspect([1 1 1])
    view(3)
    camlight;
    title(sprintf('Isosurface at isovalue = %.2f', isovalue));
end
