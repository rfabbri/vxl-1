% This is /lemsvxl/contrib/firat/matlab/levelset2d_segmentation/initialization/show_all_auto_bubble_initialization.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 24, 2011

function show_all_auto_bubble_initialization(vtk_file, w, hx, hy)
	V = double(readVTK(vtk_file));
	Max = max(V(:));
	Min = min(V(:));
	V = 255 * (V - Min)/Max;
	
	N = size(V,3);
	
	r = 5;
	c = 5;
	num_pages = floor((N-1)/(r*c))+1;
	exit_loop = false;
	
	[nrows,ncols,nslices] = size(V);
	[gridx, gridy] = meshgrid(1:hx:ncols, 1:hy:nrows);
	
	for  p = 1:num_pages
	figure;
		for ri = 1:r
			for ci = 1:c
				i = (p-1)*r*c + (ri-1)*c + ci;
				if i > N
					exit_loop = true;
					break
				end
				phi = auto_bubble_initialization(V(:,:,i), w, 5, 80, hx, hy);
				subplot(r,c,mod(i-1,r*c)+1);
				show_segmentation_matlab(V(:,:,i), phi, gridx, gridy);
				title(num2str(i));
			end
			if exit_loop
				break
			end			
		end		
		if exit_loop
			break
		end
	end	

end
