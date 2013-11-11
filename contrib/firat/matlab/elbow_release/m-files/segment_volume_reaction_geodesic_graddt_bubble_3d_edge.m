% This is /lemsvxl/contrib/firat/matlab/levelset2d_segmentation/main/segment_volume_reaction_geodesic_graddt_bubble_3d_edge.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Sep 1, 2011

function segment_volume_reaction_geodesic_graddt_bubble_3d_edge(vtk_file, outfolder, start_index, end_index, beta0, alpha, max_num_iter, hx, hy, narrow_band, x, y, z, nx, ny, nz, str, dtresh, stresh, edge_length)
	
	if ~exist(outfolder, 'dir')
		mkdir(outfolder);	
	end
	
		
	V = double(readVTK(vtk_file));
	Max = max(V(:));
	Min = min(V(:));
	V = 255 * (V - Min)/Max;
	
	[nrows,ncols,nslices] = size(V);
		
	[gridx, gridy] = meshgrid(1:hx:ncols, 1:hy:nrows);
	
	save([outfolder '/parameters.mat'], 'vtk_file', 'outfolder','beta0','alpha','max_num_iter', 'hx', 'hy', 'narrow_band', 'dtresh', 'stresh', 'edge_length');
	
	for i = start_index:end_index
		close all
		tic;
		slices = find(abs(z  - i) < dtresh & str > stresh);
		x1 = x(slices);
		y1 = y(slices);
		z1 = z(slices);
		nx1 = nx(slices);
		ny1 = ny(slices);
		nz1 = nz(slices);
		edg = [x1 y1 z1 nx1 ny1 nz1];
		%phi = autoinit_reaction_geodesic_graddt_edge_stop_bubble3(V(:,:,i), i, beta0, alpha, 1, max_num_iter, hx, hy, narrow_band, edg, edge_length, 0);toc	
		phi = autoinit_reaction_geodesic_graddt_edge_stop_bubble_with_sussman(V(:,:,i), i, beta0, alpha, 1, max_num_iter, hx, hy, narrow_band, edg, edge_length, 1);toc			
		%% split merged regions
		%phi = split_merged_regions(phi, gridx, gridy);
		%%
		show_segmentation_matlab(V(:,:,i), phi, gridx, gridy, 0);
		saveas(gcf, [outfolder '/' num2str(i) '.png']);
		save([outfolder '/' num2str(i) '.mat'], 'phi');
	end

end

