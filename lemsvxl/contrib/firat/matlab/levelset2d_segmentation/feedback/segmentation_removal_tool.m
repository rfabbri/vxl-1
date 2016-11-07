% This is /lemsvxl/contrib/firat/matlab/levelset2d_segmentation/feedback/segmentation_removal_tool.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 23, 2011

function phi = segmentation_removal_tool(I, phi, hx, hy, fill_value)
	[nrows,ncols] = size(I);
	[gridx, gridy] = meshgrid(1:hx:ncols, 1:hy:nrows);
	show_segmentation_matlab(I, phi, gridx, gridy);
	hold on
	[x,y,b] = ginput(1);
	x = (x-1)/hx + 1;
	y = (y-1)/hy + 1;
	if phi(round(y), round(x)) < 0
		L = bwlabel(double(phi <= 0));		
		l = L(round(y), round(x));
		phi(L == l) = fill_value;
	else
		L = bwlabel(double(phi >= 0));		
		l = L(round(y), round(x));
		phi(L == l) = -fill_value;
	end	
	show_segmentation_matlab(I, phi, gridx, gridy);
	while b == 1
		[x,y,b] = ginput(1);	
		x = (x-1)/hx + 1;
	    y = (y-1)/hy + 1;
		if phi(round(y), round(x)) < 0
			L = bwlabel(double(phi <= 0));		
			l = L(round(y), round(x));
			phi(L == l) = fill_value;
		else
			L = bwlabel(double(phi >= 0));		
			l = L(round(y), round(x));
			phi(L == l) = -fill_value;
		end	
		show_segmentation_matlab(I, phi, gridx, gridy);
	end	
end

