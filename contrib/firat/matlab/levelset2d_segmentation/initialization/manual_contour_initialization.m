% This is /lemsvxl/contrib/firat/matlab/levelset2d_segmentation/manual_contour_initialization.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 10, 2011

function phi = manual_contour_initialization(I, hx, hy)
	[nrows,ncols] = size(I);
	fid = figure;
	imagesc(I);colormap gray; axis image; axis off;
	hold on; 	  
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
	hold off;  
	pause(.1) 
	contours = {[x' y']}; 
	phi = polygon_distance_transform(contours, nrows, ncols, hx, hy);
	%bw = poly2mask(x, y, size(I,1), size(I,2));	
	%phi = sbwdist(bw, hx, hy);	
end
