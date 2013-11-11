% This is /lemsvxl/contrib/firat/matlab/levelset2d_segmentation/main/segment_volume_reaction_geodesic_graddt_bubble.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 22, 2011

function segment_volume_reaction_geodesic_graddt_bubble(vtk_file, outfolder, start_index, end_index, beta0, alpha, max_num_iter, hx, hy, edge_thresh, edge_sigma, narrow_band)
	
	if ~exist(outfolder, 'dir')
		mkdir(outfolder);	
	end
	
		
	V = double(readVTK(vtk_file));
	Max = max(V(:));
	Min = min(V(:));
	V = 255 * (V - Min)/Max;
	
	[nrows,ncols,nslices] = size(V);
	
	%tissue distribution parameters
	%imagesc(V(:,:,1));colormap gray; axis image; axis off;
	%[x,y,b] = ginput(2);
	%max_xy = max([x y]);
	%min_xy = min([x y]);
	%bb = [min_xy max_xy-min_xy];
	%hold on;
	%rectangle('Position', bb, 'EdgeColor', 'r');    
	%P = imcrop(V(:,:,1), bb);
	%P = P(:);    
	%reg_mu = mean(P)
	%reg_sigma = std(P,1) 	
	
	[gridx, gridy] = meshgrid(1:hx:ncols, 1:hy:nrows);
	
	save([outfolder '/parameters.mat']);
	
	for i = start_index:end_index
		close all
		phi = autoinit_reaction_geodesic_graddt_edge_stop_bubble(V(:,:,i), beta0, alpha, 1, max_num_iter, hx, hy, edge_thresh, edge_sigma, 0, 0, narrow_band, nan, 0);			
		show_segmentation_matlab(V(:,:,i), phi, gridx, gridy);
		saveas(gcf, [outfolder '/' num2str(i) '.png']);
		save([outfolder '/' num2str(i) '.mat'], 'phi');
	end

end

