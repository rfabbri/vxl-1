% This is /lemsvxl/contrib/firat/courses/engn2500/visualize_foreground.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jul 22, 2011


function vertices = visualize_foreground(F, isovalue, hx, hy, hz)
    skin_color = [170 170 240]/255;
    %V = smooth3(F, 'gaussian', 3);
    V = F;
    [num_y, num_x, num_z] = size(V);
    x = 1:hx:(num_x-1)*hx+1;
    y = 1:hy:(num_y-1)*hy+1;
    z = 1:hz:(num_z-1)*hz+1;
    IS = isosurface(x, y, z, V, isovalue);
    vertices = IS.vertices;    
    P = patch(IS);
    isonormals(x, y, z, V, P);
    set(P, 'FaceColor', skin_color, 'EdgeColor', 'none');
    daspect([1 1 1])
    view(3)
    camlight; %lighting phong
    axis([1 (num_x-1)*hx+1 1 (num_y-1)*hy+1 1 (num_z-1)*hz+1])
end
