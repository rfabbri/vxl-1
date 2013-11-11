% This is /lemsvxl/contrib/firat/matlab/3d_reconstruction_from_edges/cluster_edges.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Sep 19, 2011

function clusters = cluster_edges(edg)
    Max = round(max(edg(:,1:3)));
    ncols = Max(1);
    nrows = Max(2);
    nbands = Max(3);
    voxel_grid = cell(nrows, ncols, nbands);
	for i = 1:size(edg,1)		
		voxel_r = round(edg(i,2));
		voxel_c = round(edg(i,1));
		voxel_b = round(edg(i,3));
		if voxel_r > 0 && voxel_c > 0 && voxel_b > 0 && voxel_r <= nrows && voxel_c <= ncols && voxel_b <= nbands
			voxel_grid{voxel_r, voxel_c, voxel_b} = [voxel_grid{voxel_r, voxel_c, voxel_b}; edg(i,:)];
		end
	end
	size_array = zeros(nrows, ncols, nbands);
	for i = 1:nrows
	    for j = 1:ncols
	        for k = 1:nbands
	            size_array(i,j,k) = size(voxel_grid{i,j,k}, 1);
	        end
	    end
	end
	[L, num] = bwlabeln(size_array);
	clusters = cell(num,1);
	for i = 1:num
	    clusters{i} = cell2mat(voxel_grid(L == i));
	end	
end
