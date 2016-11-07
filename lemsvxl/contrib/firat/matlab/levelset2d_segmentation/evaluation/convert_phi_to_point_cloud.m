% This is /lemsvxl/contrib/firat/matlab/levelset2d_segmentation/evaluation/convert_phi_to_point_cloud.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Sep 29, 2011

function convert_phi_to_point_cloud(infolder, outfile, start_index, end_index, hx, hy)

	  for k = start_index:end_index
		load([infolder '/' num2str(k) '.mat']);
		if ~exist('height','var')
			[t1, t2] = size(phi);
			height = (t1-1)*hy+1;
			width = (t2-1)*hx+1;
			[gridx, gridy] = meshgrid(1:hx:width, 1:hy:height);
		end
		C = contour(gridx, gridy, phi, [0,0]);
		close all;
		L = size(C,2);		
		i = 1;
		while i < L		
			vertices = C(:,i+1:i+C(2,i)-1)';
			vertices = [vertices, repmat(k, size(vertices,1), 1)];
		    dlmwrite(outfile, vertices, '-append', 'delimiter', ' ');			
			i = i + C(2,i) + 1;			
		end	
		
	end
end


