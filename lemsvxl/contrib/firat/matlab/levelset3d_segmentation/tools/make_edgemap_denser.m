% This is /lemsvxl/contrib/firat/matlab/levelset3d_segmentation/tools/make_edgemap_denser.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Dec 1, 2011

function edg1 = make_edgemap_denser(edg, l, inc)
    x = edg(:,1); y = edg(:,2); z = edg(:,3);
    nx = edg(:,4); ny = edg(:,5); nz = edg(:,6);
    [v1, v2] = arrayfun(@get_vectors_on_plane, x, y, z, nx, ny, nz, 'UniformOutput', false);
    v1 = cell2mat(v1);
    v2 = cell2mat(v2);
    N = 2/inc*l+1;
    edg1 = cell(N*N,1);    
    k = 1;
    R0 = [x y z];
    for a = -l:inc:l
        for b = -l:inc:l
            edg1{k} = a*v1 + b*v2 + R0;
            k = k + 1;
        end
    end
    edg1 = cell2mat(edg1);
end

function [v1,v2] = get_vectors_on_plane(x0, y0, z0, nx, ny, nz)    
    if nz
        x = -10.3455;
        y = -10.1232;
        z = (nx*(x0-x) + ny*(y0-y))/nz + z0;        
    elseif ny
        x = -10.3455;
        z = -10.1232;
        y = (nx*(x0-x) + nz*(z0-z))/ny + y0;
    elseif nx
        y = -10.3455;
        z = -10.1232;
        x = (ny*(y0-y) + nz*(z0-z))/nx + x0;
    end
    v1 = [x-x0, y-y0, z-z0];
    v2 = cross(v1, [nx ny nz]);
    v1 = v1/norm(v1);
    v2 = v2/norm(v2);    
end
