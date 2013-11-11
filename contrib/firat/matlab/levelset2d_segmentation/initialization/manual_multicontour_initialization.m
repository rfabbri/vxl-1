% This is /lemsvxl/contrib/firat/matlab/levelset2d_segmentation/initialization/manual_multicontour_initialization.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 16, 2011

function phi = manual_multicontour_initialization(I, num, hx, hy)
	[nrows,ncols] = size(I);
	contours = cell(num,1);
	fid = figure;
	imagesc(I);colormap gray; axis image; axis off;
	hold on; 	  
	for i = 1:num
		[x(1),y(1),b] = ginput(1);
		plot(x(1),y(1),'rd');    
		[x(2),y(2),b] = ginput(1);        
		plot([x(1) x(2)], [y(1) y(2)], 'rd-')
		t = 3;
		while b == 1
			[x(t),y(t),b] = ginput(1);
			plot([x(t-1) x(t)], [y(t-1) y(t)], 'rd-');
			t = t + 1;    
		end  
		plot([x(t-1) x(1)], [y(t-1) y(1)], 'rd-');  
		contours{i} = [x' y'];
		clear x 
		clear y
	end
	hold off;	 
	phi = polygon_distance_transform(contours, nrows, ncols, hx, hy);		
end
