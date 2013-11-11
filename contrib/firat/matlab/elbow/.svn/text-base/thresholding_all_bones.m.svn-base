% This is /lemsvxl/contrib/firat/matlab/elbow/thresholding_all_bones.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jul 4, 2011

function [S, mu, sigma] = thresholding_all_bones(I, t, showplots, mu, sigma)
	I = double(I);
	J = I;    
	if ~exist('mu','var')
		fid = figure;
		imagesc(I);colormap gray; axis image; axis off;
		[x,y,b] = ginput(2);
		max_xy = max([x y]);
		min_xy = min([x y]);
		bb = [min_xy max_xy-min_xy];
		hold on;
		rectangle('Position', bb, 'EdgeColor', 'r');    
		P = imcrop(I, bb);
		P = P(:);    
		mu = mean(P)
		sigma = std(P,1)  
		close(fid);	 
	end
    I((I - mu)/sigma < t) = -Inf; 
    I = padarray(I, [1 1], 1); 
    I(2) = -Inf;          
    I = imfill(I > -Inf,'holes'); 
    I(1,:) = [];
    I(end,:) = [];
    I(:,1) =[];
    I(:,end) =[];             
    if showplots
		figure; imagesc(J); colormap gray; axis image; axis off;
		hold on;
		contour(I, [0,0], 'r', 'LineWidth', 2);
		hold off;
	end	    
	S = I;
end

