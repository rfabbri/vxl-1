% This is /lemsvxl/contrib/firat/matlab/levelset2d_segmentation/show_segmentations_for_feedback.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 23, 2011

function problematic = show_segmentations_for_feedback(segmentation_folder, s_index, e_index)
	close all;
	load([segmentation_folder '/parameters.mat'], 'vtk_file', 'hx', 'hy');
	N = e_index - s_index + 1;
	r = 5;
	c = 5;
	num_pages = floor((N-1)/(r*c))+1;
	exit_loop = false;
	
	V = double(readVTK(vtk_file));
	Max = max(V(:));
	Min = min(V(:));
	V = 255 * (V - Min)/Max;
	
	[nrows,ncols,nslices] = size(V);
	[gridx, gridy] = meshgrid(1:hx:ncols, 1:hy:nrows);
	
	problematic = [];
	
	for  p = 1:num_pages
	figure;
		for ri = 1:r
			for ci = 1:c
				i = (p-1)*r*c + (ri-1)*c + ci - 1 + s_index;
				if i > e_index
					exit_loop = true;
					break
				end
				load([segmentation_folder '/' num2str(i) '.mat']);
				subplot(r,c,mod(i-s_index,r*c)+1);
				show_segmentation_matlab(V(:,:,i), phi, gridx, gridy, 0);
				title(num2str(i));
			end
			if exit_loop
				break
			end			
		end
		inp = input('Please enter the index of problematic slices in []: ');		
		problematic= [problematic, inp];
		if exit_loop
			break
		end
	end	
	close all
end
