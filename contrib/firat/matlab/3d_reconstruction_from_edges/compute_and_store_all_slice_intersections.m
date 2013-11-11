% This is /lemsvxl/contrib/firat/matlab/3d_reconstruction_from_edges/compute_and_store_all_slice_intersections.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Sep 20, 2011

function compute_and_store_all_slice_intersections(ply_file, start_index, end_index, outfolder)
    [tri, pts] = ply_read(ply_file, 'tri');
    if ~exist(outfolder, 'dir')
        mkdir(outfolder);
    end
    for i = start_index:end_index
        fprintf('Processing slice %d...\n', i);
        contours = intersect_mesh_and_slice(tri, pts, i);
        contours = remove_inner_contours(contours);
        contours
        save_contours(contours, fullfile(outfolder, [num2str(i) '.txt']));            
    end
end
