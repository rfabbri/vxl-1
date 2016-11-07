% This is /lemsvxl/contrib/firat/matlab/levelset2d_segmentation/main/segment_volume_reaction_geodesic_graddt_bubble_projected_edge.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Sep 7, 2011

function segment_volume_reaction_geodesic_graddt_bubble_projected_edge(vtk_file, outfolder, start_index, end_index, beta0, alpha, max_num_iter, hx, hy, narrow_band, x, y, z, nx, ny, nz, str, dtresh, stresh, edge_length)
	
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
		edg = filter_and_project_edges(x,y,z,nx,ny,nz,i,str,dtresh,stresh);
		edg = filter_outlier_edges_orient(edg, nrows, ncols);
		phi = autoinit_reaction_geodesic_graddt_edge_stop_bubble2(V(:,:,i), beta0, alpha, 1, max_num_iter, hx, hy, narrow_band, [edg(:,1:2) atan(-edg(:,4)./ -edg(:,5))], edge_length, 0);toc			
		show_segmentation_matlab(V(:,:,i), phi, gridx, gridy, 0);
		saveas(gcf, [outfolder '/' num2str(i) '.png']);
		save([outfolder '/' num2str(i) '.mat'], 'phi');
	end

end

