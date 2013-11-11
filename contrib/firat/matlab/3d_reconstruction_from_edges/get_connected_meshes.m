% This is /lemsvxl/contrib/firat/matlab/3d_reconstruction_from_edges/get_connected_meshes.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Sep 25, 2011

function get_connected_meshes(ply_file, outfolder)
    [tri,pts] = ply_read(ply_file, 'tri');
    groups = cell(size(tri,2),1);
    for i = 1:size(tri,2)
        groups{i} = tri(:,i)';
    end
    while ~isempty(tri)
        T = tri(:,1);
        [r,c] = find(tri == T(1) | tri == T(2) | tri == T(3));
        G = tri(:, unique(c));
    end

end
