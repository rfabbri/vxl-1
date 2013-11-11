% This is /lemsvxl/contrib/firat/matlab/third_order_edge_detector/get_edgemap_for_slice_given_3d_edgemap.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 25, 2011

function edg = get_edgemap_for_slice_given_3d_edgemap(edg3d, z, max_distance_from_edge)
	M = abs(edg3d(:,3)-z) <= max_distance_from_edge;
	edg = edg3d(M, [1:2 4:end]);
end
