% This is /lemsvxl/contrib/firat/matlab/levelset2d_segmentation/manual_bubble_initialization.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 10, 2011

function phi = manual_bubble_initialization(I, r, hx, hy)
	[nrows,ncols] = size(I);
	fid = figure;
	imagesc(I);colormap gray; axis image; axis off;
	hold on;  		  
	[x(1),y(1),b] = ginput(1);
	plot(x(1),y(1),'r.');    
	t = 2;
	while b == 1
		[x(t),y(t),b] = ginput(1);
		plot([x(t-1) x(t)], [y(t-1) y(t)], 'r.');
		t = t + 1;    
	end  
	plot([x(t-1) x(1)], [y(t-1) y(1)], 'r.');  
	hold off;  
	pause(.1) 	
	phi = subpixelDT_fast([x' y'], nrows, ncols, hx, hy)-r;
end
