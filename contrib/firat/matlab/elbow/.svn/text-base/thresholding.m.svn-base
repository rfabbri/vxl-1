% This is thresholding.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jun 24, 2011

function [S, mu, sigma, xa, ya] = thresholding(I, t, mu, sigma, xa, ya)
    I = double(I);
    if ~exist('mu','var') || ~exist('sigma', 'var')
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
    I = imfill(I > -Inf,'holes');        
    %I = medfilt2(I);                    
    L = bwlabel(I); 
    if ~exist('xa','var') || ~exist('ya', 'var')
    	close all;
    	fid = figure;
    	imagesc(L); colormap gray; axis image; axis off;
    	[xa,ya,b] = ginput(1); 
    	close(fid);    	
    end         
    l = L(uint8(ya),uint8(xa));
    S = L == l;
    
    %imagesc(S); colormap gray; axis image; axis off;    
end

