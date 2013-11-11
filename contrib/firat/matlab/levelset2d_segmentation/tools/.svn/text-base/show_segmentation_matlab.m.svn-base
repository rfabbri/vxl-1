% This is /lemsvxl/contrib/firat/matlab/levelset2d_segmentation/tools/show_segmentation_matlab.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 22, 2011

function show_segmentation_matlab(I, phi, gridx, gridy, is_smooth)
	if is_smooth == 0
		imagesc(I); colormap gray; axis image; axis off;		
		hold on;	
		C = contour(gridx, gridy, phi, [0,0], 'LineWidth', 1, 'Color', [1 0 0]);
		hold off;
	else
		fid = figure;
		C = contour(gridx, gridy, phi, [0,0], 'LineWidth', 1, 'Color', [1 0 0]);
		close(fid);
		imagesc(I); colormap gray; axis image; axis off;		
		hold on;	
		L = size(C,2);		
		i = 1;
		while i < L		
			vertices = C(:,i+1:i+C(2,i)-1)';
			A = sample_curve(vertices, is_smooth, 0);
			plot(A(:,1),A(:,2),'r')			
			i = i + C(2,i) + 1;			
		end	
		hold off
	end
end
