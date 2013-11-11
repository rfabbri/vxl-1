% This is detect_saddles_wc_ws.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date May 17, 2012

function [saddles, S] = detect_saddles_wc_ws(I, sigma)
	[nrows,ncols] = size(I);
	G = fspecial('gaussian', ceil(7*sigma)*[1 1], sigma);
	S = imfilter(I,G,'replicate');
	WS = watershed(S, 8) == 0; %watershed = max + saddle + ridges
	WC = watershed(-S, 8) == 0; %watercourse = min + saddle + valleys
	watershed_lines = edgelink(WS, 1);
	wsim = edgelist2image(watershed_lines, [nrows,ncols]);
	watercourse_lines = edgelink(WC, 1);
	wcim = edgelist2image(watercourse_lines, [nrows,ncols]);
	sad = wsim & wcim; %saddles
	[r,c] = find(sad);
	saddles = [c r];
end
